#include "config_manager.hpp"

config_manager::config_manager() = default;
config_manager::~config_manager() = default;

bool config_manager::parse_command_line(int argc, char* argv[]) {
    try {
        boost::program_options::options_description desc("Использование");
        desc.add_options()
            ("config", boost::program_options::value<std::string>()->required(), 
                "Путь к конфигу (--config)")
            ("cfg", boost::program_options::value<std::string>()->required(), 
                "Путь к конфигу (-cfg)")
            ("c", boost::program_options::value<std::string>()->required(), 
                "Путь к конфигу (-c)")
            ("conf", boost::program_options::value<std::string>()->required(), 
                "Путь к конфигу (-conf)");
                
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::
            parse_command_line(argc, argv, desc), vm);
        
        // Определяем, какой флаг использован
        if (vm.count("config")) {
            cmd_.config_file = vm["config"].as<std::string>();
        } else if (vm.count("cfg")) {
            cmd_.config_file = vm["cfg"].as<std::string>();
        } else if (vm.count("c")) {
            cmd_.config_file = vm["c"].as<std::string>();
        } else if (vm.count("conf")) {
            cmd_.config_file = vm["conf"].as<std::string>();
        } else {
            // Если ничего не указано - используем значение по умолчанию
            cmd_.config_file = "./config.toml";
            spdlog::warn("Предупреждение: используем конфиг по{}", 
                        cmd_.config_file);
        }
        
        // Проверяем существует ли файл
        if (!std::filesystem::exists(cmd_.config_file)) {
            spdlog::warn("Предупреждение: Файл конфигурации не найден: {}", 
                cmd_.config_file);
        }
        return true;
        
    } catch (const boost::program_options::error& e) {
        spdlog::error("Ошибка: парсинг аргументов не удался: {}", e.what());
        cmd_.error = true;
        return false;
    }
}

bool config_manager::parse_toml_file(const std::string& filename) {
    try {
        auto toml = toml::parse_file(filename);
        
        auto main = toml["main"];
        
        // Обязательный параметр input
        auto input = main["input"].value<std::string>();
        if (!input) {
            spdlog::error("Ошибка: в TOML не указан 'input'");
            return false;
        }
        config_.input_dir_ = *input;
        
        // Опциональный параметр output
        auto output = main["output"].value<std::string>();
        if (output) {
            config_.output_dir_ = *output;
        } else {
            spdlog::warn("Предупреждение: в TOML не указан 'output'");
            config_.output_dir_ = "./output";
        }
        
        // Опциональные маски файлов
        if (auto masks = main["filename_mask"].as_array()) {
            for (auto&& elem : *masks) {
                if (auto mask = elem.value<std::string>()) {
                    config_.filename_masks_.push_back(*mask);
                }
            }
        }
        
        return true;
        
    } catch (const toml::parse_error& error) {
        spdlog::error("Ошибка: парсинг TOML-файла не удался: {}", 
            error.description());
        return false;
    }
}

bool config_manager::validate_and_create_output_dir() {
    try {
        // Проверка входной директории
        if (!std::filesystem::exists(config_.input_dir_)) {
            spdlog::error("Ошибка: Входная директория не существует: {}",
                config_.input_dir_);
            return false;
        }
        
        if (!std::filesystem::is_directory(config_.input_dir_)) {
            spdlog::error("Ошибка: Указанный путь не является директорией: ",
                config_.input_dir_);
            return false;
        }
        
        // Создаём выходную директорию, если нужно
        if (!std::filesystem::exists(config_.output_dir_)) {
            if (std::filesystem::create_directories(config_.output_dir_)) {
                config_.output_dir_created = true;
                spdlog::info("Создана выходная директория: {}", 
                    config_.output_dir_);
            } else {
                spdlog::error("Ошибка: Не удалось создать выходную дир.-рию: ",
                    config_.output_dir_);
                return false;
            }
        }
        
        return true;
        
    } catch (const std::filesystem::filesystem_error& error) {
        spdlog::error("Ошибка файловой системы: {}", error.what());
        return false;
    }
}

bool config_manager::load_configuration(int argc, char* argv[]) {
    if (!parse_command_line(argc, argv)) {
        return false;
    }
    
    if (!parse_toml_file(cmd_.config_file)) {
        return false;
    }
    
    if (!validate_and_create_output_dir()) {
        return false;
    }
    
    return true;
}