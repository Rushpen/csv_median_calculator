#include "config_manager.hpp"

config_manager::config_manager() = default;
config_manager::~config_manager() = default;

bool config_manager::parse_command_line(int argc, char* argv[]) {
    try {
        boost::program_options::options_description desc("Использование");
        desc.add_options()
            ("config,c", boost::program_options::value<std::string>()->
                required(),
             "Путь к TOML-файлу конфигурации");
        
        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::
            parse_command_line(argc, argv, desc), vm);
        
        boost::program_options::notify(vm);
        cmd_.config_file = vm["config"].as<std::string>();
        return true;
        
    } catch (const boost::program_options::error& e) {
        spdlog::error("Ошибка парсинга аргументов: {}", e.what());
        cmd_.error = true;
        return false;
    }
}

bool config_manager::parse_toml_file(const std::string& filename) {
    try {
        auto toml = toml::parse_file(filename);
        
        auto main = toml["main"];
        
        auto input = main["input"].value<std::string>();
        if (!input) {
            spdlog::error("Ошибка: в TOML не указан 'input'");
            return false;
        }
        config_.input_dir_ = *input;
        
        auto output = main["output"].value<std::string>();
        if (output) {
            config_.output_dir_ = *output;
        } else {
            spdlog::warn("Предупреждение: в TOML не указан 'output'");
            config_.output_dir_ = "./output";
        }
        
        if (auto masks = main["filename_mask"].as_array()) {
            for (auto&& elem : *masks) {
                if (auto mask = elem.value<std::string>()) {
                    config_.filename_masks_.push_back(*mask);
                }
            }
        }
        
        return true;
        
    } catch (const toml::parse_error& error) {
        spdlog::error("Ошибка парсинга TOML: {}", error.description());
        return false;
    }
}

bool config_manager::validate_and_create_output_dir() {
    try {
        if (!std::filesystem::exists(config_.input_dir_)) {
            spdlog::error("Входная директория не существует: {}",
                config_.input_dir_);
            return false;
        }
        
        if (!std::filesystem::is_directory(config_.input_dir_)) {
            spdlog::error("Указанный путь не является директорией: ",
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
                spdlog::error("Не удалось создать выходную директорию: ",
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