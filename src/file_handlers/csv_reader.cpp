#include "csv_reader.hpp"

csv_reader::csv_reader(const std::string& filename) :
    filename_(filename) {}

csv_reader::~csv_reader() {
    file_close();
}

bool csv_reader::file_open() {
    if (file_.is_open()) {
        file_.close();
    }
    
    file_.open(filename_);
    if (!file_.is_open()) {
        spdlog::error("Ошибка: проблема при попытке открытия файла {}", filename_);
        is_open_ = false;
        return false;
    }
    
    std::string header_line;
    if (std::getline(file_, header_line)) {
        headers_ = split(header_line, ';');
        line_number_ = 1;  // заголовок — строка 1
    } else {
        spdlog::error("Ошибка: файл пустой или не имеет заголовков");
        file_.close();
        is_open_ = false;
        return false;
    }
    
    is_open_ = true;
    return true;
}

void csv_reader::file_close() {
    if (file_.is_open()) {
        file_.close();
    }
    is_open_ = false;
}

std::vector<std::string> csv_reader::split(const std::string& str,
        char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::optional<std::vector<std::string>> csv_reader::read_next_row() {
    if (!file_.is_open() || file_.eof()) {
        return std::nullopt;
    }
    
    std::string line;
    while (std::getline(file_, line)) {
        line_number_++;

        //Продолжаем считывать если пустая строка в середине файла
        if (line.empty()) { 
            continue;
        }
        
        return split(line, ';');
    }
    
    return std::nullopt;
}

std::vector<std::vector<std::string>> csv_reader::read_all_rows() {
    std::vector<std::vector<std::string>> rows;
    
    while (auto row = read_next_row()) {
        rows.push_back(*row);
    }
    
    return rows;
}