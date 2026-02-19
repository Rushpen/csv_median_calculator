#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include "spdlog/spdlog.h"

/**
 * \brief Класс для поиска CSV-файлов по маскам
 */
class file_scanner {
private:
    std::string directory_;             ///< Директория для поиска
    std::vector<std::string> masks_;    ///< Маски имён файлов
    
    /**
     * \brief Приводит строку к нижнему регистру
     * \param str исходная строка
     * \return строка в нижнем регистре
     */
    std::string to_lowercase(const std::string& str) const;

    /**
     * \brief Проверяет, соответствует ли файл хотя бы одной маске
     * \param filename имя файла
     * \return true если соответствует
     */
    bool matches_mask(const std::string& filename) const;
    
public:
    file_scanner(const std::string& directory, 
        const std::vector<std::string>& masks);
    
    ~file_scanner();

    std::string get_directory() const {
        return directory_;
    }
    
    std::vector<std::string> get_masks() const {
        return masks_;
    }

    /**
     * \brief Сканирует директорию и возвращает подходящие файлы
     * \return вектор путей к найденным файлам
     */
    std::vector<std::filesystem::path> scan_files() const;
};