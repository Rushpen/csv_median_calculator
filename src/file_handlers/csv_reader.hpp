#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include "spdlog/spdlog.h"

/**
 * \brief Класс для чтения CSV-файлов с разделителем ';'
 */
class csv_reader {
private:
    std::string filename_;
    std::ifstream file_;
    std::vector<std::string> headers_; ///< Заголовки колонок
    std::vector<std::vector<std::string>> rows_; ///< Все строки (опционально)
    int line_number_ = 0;
    bool is_open_ = false; ///< Флаг факта открытия файла
    
    /**
     * \brief Разделяет строку на части по разделителю
     * \param str исходная строка
     * \param delimiter разделитель
     * \return вектор строк
     */
    std::vector<std::string> split(const std::string& str, char delimiter);

public:
    explicit csv_reader(const std::string& filename); //конструктор
    ~csv_reader();

    bool file_open();
    void file_close();
    
    bool file_is_open() const { 
        return is_open_; 
    }
    
    const std::vector<std::string>& get_headers() const { 
        return headers_; 
    }

    /**
     * \brief Читает следующую строку из файла
     * \return optional с вектором строк или nullopt при конце файла
     */
    std::optional<std::vector<std::string>> read_next_row();

    /**
     * \brief Читает все строки из файла
     * \return вектор всех строк
     */
    std::vector<std::vector<std::string>> read_all_rows();
    int get_line_number() const { 
        return line_number_; 
    }
};