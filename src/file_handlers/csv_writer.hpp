#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include "spdlog/spdlog.h"

/**
 * \brief Класс для записи результатов в CSV-файл
 */
class csv_writer {
private:
    std::string filename_;
    std::ofstream file_;        ///< Файловый поток
    bool file_is_open_ = false; ///< Флаг факта открытия файла
    double last_median_;        ///< Последняя записанная медиана

public:
    explicit csv_writer(const std::string& filename_); //конструктор
    ~csv_writer();

    bool file_open();
    void file_close();

     /**
     * \brief Записывает медиану с временной меткой
     * \param timestamp временная метка
     * \param median значение медианы
     */
    void write_median_to_csv(uint64_t timestamp, double median);
    
    bool file_is_open() const { 
        return file_is_open_; 
    }
    
    double get_last_median() const { 
        return last_median_; 
    }
};