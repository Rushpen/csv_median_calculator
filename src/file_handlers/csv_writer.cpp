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

void csv_writer::write_metrics(uint64_t timestamp, 
    const std::map<std::string, double>& metrics) {
    if (!file_.is_open()) return;

    // Заголовок (пишем один раз)
    static bool header_written = false;
    
    // Записываем заголовок только один раз
    if (!header_written) {
        file_ << "timestamp";

        // Перебираем все метрики из словаря
        for (auto pair = metrics.begin(); 
            pair != metrics.end(); ++pair) {
            std::string metric_name = pair->first;
            file_ << ";" << metric_name;
        }

        file_ << "\n";
        header_written = true;
    }
    
    // Записываем данные
    file_ << timestamp;

    // Перебираем все метрики снова, но теперь берём значения
    for (auto pair = metrics.begin(); 
        pair != metrics.end(); ++pair) {
        double metric_value = pair->second;  // значение метрики
        file_ << ";" << std::fixed << std::setprecision(8) << metric_value;
    }

    file_ << "\n";
    file_.flush();
}