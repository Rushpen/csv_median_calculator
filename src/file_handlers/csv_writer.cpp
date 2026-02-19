#include "csv_writer.hpp"

csv_writer::csv_writer(const std::string& filename) :
    filename_(filename), file_is_open_(false), last_median_(0.0) {}

csv_writer::~csv_writer() {
    file_close();
}

bool csv_writer::file_open() {
    file_.open(filename_);
    if (!file_.is_open()) {
        spdlog::error("Ошибка: Не удалось открыть файл для записи: {}",
            filename_);
        return false;
    }
    
    // Записываем заголовок csv
    file_ << "receive_ts;price_median\n";
    file_is_open_ = true;
    return true;
}

void csv_writer::file_close() {
    if (file_.is_open()) {
        file_.close();
    }
    file_is_open_ = false;
}

void csv_writer::write_median_to_csv(uint64_t timestamp, double median) {
    if (!file_.is_open()) return;
    
    // Запись строки с результатом
    file_ << timestamp << ";"
          << std::fixed << std::setprecision(8) << median << "\n";
    file_.flush();  // Сразу записываем на диск
    
    last_median_ = median;
}