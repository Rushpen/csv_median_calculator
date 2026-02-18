#include "csv_reader.hpp"
#include "file_scanner.hpp"
#include "data_manager/data_manager.hpp"
#include <iostream>
#include <iomanip>

int main() {
    file_scanner scanner("../examples/input", {"level", "trade"}); //!!!
    auto files = scanner.scan_files();
    data_manager data;

    std::cout << "Найдено файлов: " << files.size() << std::endl;

    std::cout << "\n📄 Файлы: ";
    for (size_t i = 0; i < files.size(); ++i) {
        std::cout << files[i].filename().string();
        if (i < files.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    for (const auto& file : files) {
        csv_reader reader(file.string());
        if (!reader.file_open()) {
            std::cout << "  ❌ Не открывается" << std::endl;
            continue;
        }
        
        // Находим индексы
        int ts_idx = -1, price_idx = -1;
        for (size_t i = 0; i < reader.get_headers().size(); ++i) {
            if (reader.get_headers()[i] == "receive_ts") ts_idx = i;
            if (reader.get_headers()[i] == "price") price_idx = i;
        }
        
        if (ts_idx == -1 || price_idx == -1) {
            std::cerr << "Пропускаем файл: нет нужных колонок" << std::endl;
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
                std::cerr << "Ошибка парсинга строки: " << err.what() << std::endl;
            }
        }
        reader.file_close();
    }
    
    data.sort_by_timestamp();

    data.print_n_records(5);
    return 0;
}