#include "csv_reader.hpp"
#include "file_scanner.hpp"
#include "data_manager/data_manager.hpp"
#include "median_calculator.hpp"
#include "csv_writer.hpp"
#include <iostream>
#include <iomanip>

int main() {
    // 1. Сканируем файлы
    file_scanner scanner("../examples/input", {"level", "trade"});
    auto files = scanner.scan_files();
    data_manager data;

    std::cout << "Найдено файлов: " << files.size() << std::endl;
    std::cout << "\n📄 Файлы: ";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << files[i].filename().string();
        if (i < files.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    // 2. Читаем все файлы
    for (const auto& file : files) {
        csv_reader reader(file.string());
        if (!reader.file_open()) {
            std::cout << "  ❌ Не открывается: " << file.filename().string() << std::endl;
            continue;
        }
        
        // Находим индексы колонок
        int ts_idx = -1, price_idx = -1;
        for (size_t i = 0; i < reader.get_headers().size(); ++i) {
            if (reader.get_headers()[i] == "receive_ts") ts_idx = i;
            if (reader.get_headers()[i] == "price") price_idx = i;
        }
        
        if (ts_idx == -1 || price_idx == -1) {
            std::cerr << "  ⚠️ Пропускаем файл: нет нужных колонок" << std::endl;
            continue;
        }
        
        // Читаем строки
        int count = 0;
        while (auto row = reader.read_next_row()) {
            try {
                uint64_t ts = std::stoull((*row)[ts_idx]);
                double price = std::stod((*row)[price_idx]);
                data.add_record(ts, price);
                count++;
            } catch (const std::exception& err) {
                std::cerr << "  ⚠️ Ошибка парсинга: " << err.what() << std::endl;
            }
        }
        std::cout << "  ✅ " << file.filename().string() << ": " << count << " записей" << std::endl;
        reader.file_close();
    }
    
    // 3. Сортируем по времени
    std::cout << "\n🔄 Сортируем записи по времени..." << std::endl;
    data.sort_by_timestamp();
    std::cout << "✅ Всего записей: " << data.size() << std::endl;
    
    // 4. Создаём writer для результатов
    csv_writer writer("median_results.csv");
    if (!writer.file_open()) {
        std::cerr << "❌ Не удалось создать файл результатов" << std::endl;
        return 1;
    }
    
    // 5. Вычисляем медиану
    std::cout << "\n📊 Вычисляем медиану..." << std::endl;
    median_calculator calculator;
    const auto& records = data.get_records();
    int changes_count = 0;
    
    for (const auto& record : records) {
        calculator.add_price(record.price);
        double current_median = calculator.get_median();
        
        if (calculator.has_median_changed(current_median)) {
            writer.write_median_to_csv(record.timestamp, current_median);
            changes_count++;
        }
    }
    
    // 6. Выводим статистику
    std::cout << "✅ Готово!" << std::endl;
    std::cout << "📈 Изменений медианы: " << changes_count << std::endl;
    std::cout << "📁 Результат сохранён в: median_results.csv" << std::endl;
    
    // 7. Показываем первые несколько записей из результата (опционально)
    std::cout << "\n📋 Первые 5 записей результата:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << std::setw(15) << "timestamp" << " | " 
              << std::setw(12) << "median" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Просто для демонстрации выведем первые 5 из records
    for (size_t i = 0; i < std::min(size_t(5), records.size()); ++i) {
        std::cout << std::setw(15) << records[i].timestamp << " | ";
        calculator.add_price(records[i].price);  // пересчитываем заново для демо
        std::cout << std::setw(12) << std::fixed << std::setprecision(8) 
                  << calculator.get_median() << std::endl;
    }
    
    writer.file_close();
    return 0;
}