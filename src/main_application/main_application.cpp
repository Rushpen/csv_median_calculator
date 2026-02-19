#include "main_application.hpp"

main_application::main_application() = default;
main_application::~main_application() = default;

bool main_application::initialize(int argc, char* argv[]) {
    // Загружаем конфигурацию
    config_ = std::make_unique<config_manager>();
    if (!config_->load_configuration(argc, argv)) {
        return false;
    }

    // Проверяем на ошибки
    if (config_->has_error()) {
        std::cerr << "Ошибка загрузки конфигурации" << std::endl;
        return false;
    }
    
    // Выводим настройки
    std::cout << "Входная директория: " <<
        config_->get_input_dir() << std::endl;
    std::cout << "Выходная директория: " <<
        config_->get_output_dir() << std::endl;
    
    auto masks = config_->get_filename_masks();
    if (!masks.empty()) {
        std::cout << "\nМаски, полученные из файлов конфига: ";
        for (const auto& m : masks) std::cout << m << " ";
        std::cout << std::endl;
    }
    
    // Создаём остальные объекты
    data_ = std::make_unique<data_manager>();
    calculator_ = std::make_unique<median_calculator>();
    
    // Создаём writer с путём из конфига
    std::string output_path = config_->get_output_dir() 
        + "/median_results.csv";
    writer_ = std::make_unique<csv_writer>(output_path);
    
    return true;
}

bool main_application::scan_and_read_files() {
    // Сканируем файлы
    file_scanner scanner(config_->get_input_dir(),
        config_->get_filename_masks());
    auto files = scanner.scan_files();
    
    if (files.empty()) {
        std::cerr << "Не найдено CSV файлов" << std::endl;
        return false;
    }
    
    std::cout << "\nНайдено файлов: " << files.size() << std::endl;
    for (const auto& file : files) {
        std::cout << "  - " << file.filename().string() << std::endl;
    }
    
    // Читаем каждый файл
    for (const auto& file : files) {
        std::cout << "\nСчитывание файла: " <<
            file.filename().string() << "... ";
        
        csv_reader reader(file.string());
        if (!reader.file_open()) {
            std::cout << "Не удалось открыть" << std::endl;
            continue;
        }
        
        // Находим индексы колонок
        int ts_idx = -1, price_idx = -1;
        for (size_t i = 0; i < reader.get_headers().size(); ++i) {
            if (reader.get_headers()[i] == "receive_ts") ts_idx = i;
            if (reader.get_headers()[i] == "price") price_idx = i;
        }
        
        if (ts_idx == -1 || price_idx == -1) {
            std::cout << "Пропущено: нет колонок receive_ts/price" <<
                std::endl;
            continue;
        }
        
        // Читаем строки
        int count = 0;
        while (auto row = reader.read_next_row()) {
            try {
                uint64_t ts = std::stoull((*row)[ts_idx]);
                double price = std::stod((*row)[price_idx]);
                data_->add_record(ts, price);
                count++;
            } catch (const std::exception& err) {
                std::cerr << "\nОшибка парсинга: " <<
                    err.what() << std::endl;
            }
        }
        
        std::cout << "найдено записей" << count  << std::endl;
        reader.file_close();
    }
    
    // Сортируем записи по времени
    data_->sort_by_timestamp();
    std::cout << "Прочитано записей: " << data_->size() << std::endl;
    
    return true;
}

bool main_application::calculate_and_write_results() {
    if (!writer_->file_open()) {
        std::cerr << "Не удалось создать выходной файл" << std::endl;
        return false;
    }
    
    // Вычисление медианы
    const auto& records = data_->get_records();
    int changes_count = 0;
    
    for (const auto& record : records) {
        calculator_->add_price(record.price);
        double current_median = calculator_->get_median();
        
        if (calculator_->has_median_changed(current_median)) {
            writer_->write_median_to_csv(record.timestamp, current_median);
            changes_count++;
        }
    }
    
    std::cout << "Изменений медианы(кол.-во): " <<
        changes_count << std::endl;
    
    writer_->file_close();
    return true;
}

// Вывод в терминал
void main_application::print_statistics() const {
    if (!data_ || data_->empty()) return;
    
    const auto& records = data_->get_records();
    
    std::cout << "\nСтатистика:" << std::endl;
    std::cout << "  Первая запись: ts=" << records.front().timestamp 
              << ", price=" << records.front().price << std::endl;
    std::cout << "  Последняя запись: ts=" << records.back().timestamp 
              << ", price=" << records.back().price << std::endl;
    
    std::cout << "\nРезультат сохранён в: " 
        << config_->get_output_dir() << "/median_results.csv" << std::endl;
}

int main_application::run(int argc, char* argv[]) {
    if (!initialize(argc, argv)) {
        return 1;
    }
    
    if (!scan_and_read_files()) {
        return 1;
    }

    if (!calculate_and_write_results()) {
        return 1;
    }

    print_statistics();
    
    return 0;
}