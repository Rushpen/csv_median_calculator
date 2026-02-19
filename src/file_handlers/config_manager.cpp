#include "config_manager.hpp"

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
        std::cerr << "Ошибка парсинга аргументов: " << e.what() << std::endl;
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
            std::cerr << "Ошибка: в TOML не указан 'input'" << std::endl;
            return false;
        }
        config_.input_dir = *input;
        
        auto output = main["output"].value<std::string>();
        if (output) {
            config_.output_dir = *output;
        } else {
            config_.output_dir = "./output";
        }
        
        if (auto masks = main["filename_mask"].as_array()) {
            for (auto&& elem : *masks) {
                if (auto mask = elem.value<std::string>()) {
                    config_.filename_masks.push_back(*mask);
                }
            }
        }
        
        return true;
        
    } catch (const toml::parse_error& error) {
        std::cerr << "Ошибка парсинга TOML: " <<
            error.description() << std::endl;
        return false;
    }
}

bool config_manager::validate_and_create_output_dir() {
    try {
        if (!std::filesystem::exists(config_.input_dir)) {
            std::cerr << "Входная директория не существует: " 
                      << config_.input_dir << std::endl;
            return false;
        }
        
        if (!std::filesystem::is_directory(config_.input_dir)) {
            std::cerr << "Указанный путь не является директорией: " 
                      << config_.input_dir << std::endl;
            return false;
        }
        
        // Создаём выходную директорию, если нужно
        if (!std::filesystem::exists(config_.output_dir)) {
            if (std::filesystem::create_directories(config_.output_dir)) {
                config_.output_dir_created = true;
                std::cout << "Создана выходная директория: " 
                          << config_.output_dir << std::endl;
            } else {
                std::cerr << "Не удалось создать выходную директорию: " 
                          << config_.output_dir << std::endl;
                return false;
            }
        }
        
        return true;
        
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Ошибка файловой системы: " << e.what() << std::endl;
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