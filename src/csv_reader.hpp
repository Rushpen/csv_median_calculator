#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <algorithm>

class csv_reader {
private:
    std::string filename_;
    std::ifstream file_;
    std::vector<std::string> headers_; //заголовки в csv файле
    int line_number_ = 0;
    bool is_open_ = false; //флаг факта открытия файла
    
    std::vector<std::string> split(const std::string& str, char delimiter);
    
public:
    explicit csv_reader(const std::string& filename); //конструктор
    ~csv_reader();

    bool file_open();
    void file_close();
    
    bool file_is_open() const { return is_open_; }
    
    const std::vector<std::string>& get_headers() const { return headers_; }

    std::optional<std::vector<std::string>> read_next_row();
    std::vector<std::vector<std::string>> read_all_rows();
    int get_line_number() const { return line_number_; }
};