#include <iostream>
#include <filesystem>
#include <sstream>
#include <boost/program_options.hpp>
#include <toml++/toml.h>
#include "spdlog/spdlog.h"

/**
 * \brief Структура с настройками приложения из TOML-файла
 */
struct app_config {
    std::string input_dir_;                 // Путь к папке с входными CSV
    std::string output_dir_;                 // Путь для выходных файлов
    std::vector<std::string> filename_masks_;
    bool output_dir_created = false;        // Флаг создания выходной папки
};

/**
 * \brief Результат парсинга командной строки
 */
struct cmd_arguments {
    std::string config_file;
    bool error = false;
};

/**
 * \brief Класс для управления конфигурацией приложения
 * \details Объединяет парсинг аргументов командной строки,
 *          чтение TOML-файла и валидацию путей.
 */
class config_manager {
private:
    cmd_arguments cmd_;
    app_config config_;
    std::string usage_string_;
    
    bool parse_command_line(int argc, char* argv[]);
    bool parse_toml_file(const std::string& filename);
    bool validate_and_create_output_dir();

public:
    config_manager();
    ~config_manager();

    /**
     * \brief Загружает конфигурацию из аргументов и TOML-файла
     */
    bool load_configuration(int argc, char* argv[]);
    
    std::string get_usage() const { 
        return usage_string_; 
    }
    
    bool has_error() const {
        return cmd_.error || config_.input_dir_.empty(); 
    }
    
    // Геттеры для настроек
    std::string get_input_dir() const {
        return config_.input_dir_; 
    }

    std::string get_output_dir() const { 
        return config_.output_dir_; 
    }

    std::vector<std::string> get_filename_masks() const { 
        return config_.filename_masks_; 
    }
    
    bool was_output_dir_created() const { 
        return config_.output_dir_created; 
    }
};