#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>

class csv_writer {
private:
    std::string filename_;
    std::ofstream file_;
    bool file_is_open_ = false; //флаг факта открытия файла
    double last_median_;

public:
    explicit csv_writer(const std::string& filename_); //конструктор
    ~csv_writer();

    bool file_open();
    void file_close();

    void write_median_to_csv(uint64_t timestamp, double median);
    
    bool file_is_open() const { return file_is_open_; }
    double get_last_median() const { return last_median_; }

};