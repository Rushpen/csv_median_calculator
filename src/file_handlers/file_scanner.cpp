#include "file_scanner.hpp"

file_scanner::file_scanner(const std::string& directory,
    const std::vector<std::string>& masks) 
        : directory_(directory), masks_(masks) {}

file_scanner::~file_scanner() = default;

std::string file_scanner::to_lowercase(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) 
        { 
            return std::tolower(c); 
        });

    return result;
}

bool file_scanner::matches_mask(const std::string& filename) const {
    if (masks_.empty()) {
        return true;
    }
    std::string filename_lower = to_lowercase(filename);
    
    for (const auto&mask : masks_) {
        if (filename_lower.find(to_lowercase(mask)) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> file_scanner::scan_files() const {
    std::vector<std::filesystem::path> result;

    if (!std::filesystem::exists(directory_) 
        || !std::filesystem::is_directory(directory_)) {
        spdlog::error("Ошибка: директория {} не существует или ею не является",
            directory_);
        return result;
    }

    try {
        for (const auto& entry : 
            std::filesystem::directory_iterator(directory_)) {
    
        if (!entry.is_regular_file()) continue; //Проверка файл ли это вообще
        
        // Рассматриваем только файлы с расширением .csv
        if (entry.path().extension() != ".csv") continue; 

        if (matches_mask(entry.path().filename().string())) {
            result.push_back(entry.path());
        }
        }
    }

    catch(const std::filesystem::filesystem_error& error) {
        spdlog::error("Ошибка: сканирование файлов не удалось: {}",
            error.what());
    }

    return result;
}