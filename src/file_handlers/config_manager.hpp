#include <iostream>
#include <filesystem>
#include <sstream>
#include <boost/program_options.hpp>
#include <toml++/toml.h>

struct app_config {
    std::string input_dir;                 // Путь к папке с входными CSV
    std::string output_dir;                 // Путь для выходных файлов
    std::vector<std::string> filename_masks;
    bool output_dir_created = false;        // Флаг создания выходной папки
};

struct cmd_arguments {
    std::string config_file;
    bool error = false;
};

class config_manager {
private:
    cmd_arguments cmd_;
    app_config config_;
    std::string usage_string_;
    
    bool parse_command_line(int argc, char* argv[]);
    bool parse_toml_file(const std::string& filename);
    bool validate_and_create_output_dir();

public:
    bool load_configuration(int argc, char* argv[]);
    
    std::string get_usage() const { return usage_string_; }
    
    bool has_error() const {
        return cmd_.error || config_.input_dir.empty(); 
    }
    
    // Геттеры для настроек
    std::string get_input_dir() const {
        return config_.input_dir; 
    }
    std::string get_output_dir() const { 
        return config_.output_dir; 
    }
    std::vector<std::string> get_filename_masks() const { 
        return config_.filename_masks; 
    }
    bool was_output_dir_created() const { 
        return config_.output_dir_created; 
    }
};