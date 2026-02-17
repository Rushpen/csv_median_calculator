#include "csv_reader.hpp"
#include "file_scanner.hpp"
#include <iostream>
#include <iomanip>

int main() {
    // // Создаём читатель для файла
    // csv_reader reader("../examples/input/btcusdt_level_2024.csv");
    
    // // Открываем файл и читаем заголовки
    // if (!reader.file_open()) {
    //     return 1;
    // }
    
    // // Показываем заголовки
    // std::cout << "Заголовки:" << std::endl;
    // const auto& headers = reader.get_headers();
    // for (size_t i = 0; i < headers.size(); ++i) {
    //     std::cout << "  " << i << ": " << headers[i] << std::endl;
    // }
    // std::cout << std::endl;
    
    // std::cout << "Первые 5 строк:" << std::endl;
    // int rows_shown = 0;
    // while (auto row = reader.read_next_row()) {
    //     for (const auto& cell : *row) {
    //         std::cout << cell << " | ";
    //     }
    //     std::cout << std::endl;
        
    //     rows_shown++;
    //     if (rows_shown >= 5) break;
    // }

    file_scanner scanner("../examples/input", {"level", "trade"});
    auto files = scanner.scan_files();

    std::cout << "Найдено файлов: " << files.size() << std::endl;
    
    // 2. Для каждого файла выводим первые 3 строки
    for (const auto& file : files) {
        std::cout << "\n📄 Файл: " << file.filename().string() << std::endl;
        
        csv_reader reader(file.string());
        if (!reader.file_open()) {
            std::cout << "  ❌ Не открывается" << std::endl;
            continue;
        }
        
        // Заголовки
        std::cout << "  Заголовки: ";
        for (const auto& h : reader.get_headers())
            std::cout << h << " ";
        std::cout << std::endl;
        
        // Первые 3 строки
        int rows = 0;
        while (auto row = reader.read_next_row()) {
            for (size_t i = 0; i < row->size() && i < 3; i++)
                std::cout << (*row)[i] << " | ";
            if (row->size() > 3) std::cout << "...";
            std::cout << std::endl;
            
            if (rows >= 3) break;
        }
    }

    return 0;
}