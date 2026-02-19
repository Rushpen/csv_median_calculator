#include "main_application.hpp"

main_application::main_application() = default;
main_application::~main_application() = default;

bool main_application::initialize(int argc, char* argv[]) {
    // Настройка логирования
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::trace);
    spdlog::info("Логирование настроено на уровень TRACE");

    config_ = std::make_unique<config_manager>();
    if (!config_->load_configuration(argc, argv)) {
        return false;
    }

    // Проверяем на ошибки
    if (config_->has_error()) {
        spdlog::error("Ошибка: неудачная загрузка конфигурации");
        return false;
    }
    
    // Вывод настроек для отладки
    spdlog::info("Входная директория: {}", config_->get_input_dir());
    spdlog::info("Выходная директория: {}", config_->get_output_dir());
    
    auto masks = config_->get_filename_masks();
    if (!masks.empty()) {
        spdlog::info("Маски, полученные из файлов конфига: ");
        for (const auto& m : masks) {
            spdlog::info(" [{}] ", m);
        }
    }
    
    // Создаём остальные объекты
    data_ = std::make_unique<data_manager>();
    calculator_ = std::make_unique<median_calculator>();

    // Новый калькулятор для всех метрик (кроме квантилей)
    calculator_metrics_ = std::make_unique<metrics_calculator>();
    
    // Создаём writer с путём из конфига
    std::string output_median_path = config_->get_output_dir() 
        + "/median_results.csv";
    median_writer_ = std::make_unique<csv_writer>(output_median_path);

    std::string output_metrics_path = config_->get_output_dir() 
        + "/metrics_results.csv";
    metrics_writer_ = std::make_unique<csv_writer>(output_metrics_path);
    
    return true;
}

bool main_application::scan_and_read_files() {
    // Поиск всех CSV-файлов по маскам
    file_scanner scanner(config_->get_input_dir(),
        config_->get_filename_masks());
    auto files = scanner.scan_files();
    
    if (files.empty()) {
        spdlog::error("Ошибка: Не найдено CSV файлов");
        return false;
    }
    
    spdlog::info("Найдено файлов: {}", files.size());
    for (const auto& file : files) {
        spdlog::info("  - {}", file.filename().string());
    }
    
    // Чтение каждого файла
    for (const auto& file : files) {
        spdlog::info("Считывание файла: {}", file.filename().string());
        
        csv_reader reader(file.string());
        if (!reader.file_open()) {
            spdlog::error("Ошибка: Не удалось открыть файл {}", 
                file.filename().string());
            continue;
        }
        
        // Находим индексы колонок
        int ts_idx = -1, price_idx = -1;
        for (size_t i = 0; i < reader.get_headers().size(); ++i) {
            if (reader.get_headers()[i] == "receive_ts") ts_idx = i;
            if (reader.get_headers()[i] == "price") price_idx = i;
        }
        
        if (ts_idx == -1 || price_idx == -1) {
            spdlog::warn("Предупреждение: нет колонок receive_ts/price");
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
            } 
            catch (const std::exception& err) {
                spdlog::error("\nОшибка: неудача парсинга: {}", err.what());
            }
        }
        
        spdlog::info("найдено записей {}", count);
        reader.file_close();
    }
    
    // Подготовка данных к обработке
    data_->sort_by_timestamp();
    spdlog::info("Прочитано записей: {}", data_->size());
    
    return true;
}

bool main_application::calculate_and_write_results() {
    if (!median_writer_->file_open() || !metrics_writer_->file_open()) {
        spdlog::error("Ошибка: Не удалось создать выходные файлы");
        return false;
    }
    
    // Вычисление медианы и других метрик
    const auto& records = data_->get_records();
    int changes_count = 0;
    
    for (const auto& record : records) {
        calculator_->add_price(record.price);
        calculator_metrics_->add_value(record.price);
        double current_median = calculator_->get_median();
        
        if (calculator_->has_median_changed(current_median)) {
            median_writer_->write_median_to_csv(record.timestamp,
                current_median);
            
            all_metrics["median"] = calculator_metrics_->get_median();
            all_metrics["mean"] = calculator_metrics_->get_mean();
            all_metrics["stddev"] = calculator_metrics_->get_stddev();
            all_metrics["min"] = calculator_metrics_->get_min();
            all_metrics["max"] = calculator_metrics_->get_max();

            metrics_writer_->write_metrics(record.timestamp, all_metrics);
            changes_count++;
        }
    }
    
    spdlog::info("Изменений медианы(кол.-во): {}", changes_count);
    
    median_writer_->file_close();
    metrics_writer_->file_close();
    return true;
}

// Вывод в терминал
void main_application::print_statistics() const {
    if (!data_ || data_->empty()) return;
    
    const auto& records = data_->get_records();
    
    spdlog::info("Статистика:");
    spdlog::debug("  Первая запись: ts={}, price={}",
        records.front().timestamp, records.front().price);
        
    spdlog::debug("  Последняя запись: ts={}, price={}",
        records.back().timestamp, records.back().price);
    

    spdlog::debug("Результат сохранён в: {} /median_results.csv",
        config_->get_output_dir());
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