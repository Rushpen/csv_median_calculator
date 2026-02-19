#include <iostream>
#include <iomanip>
#include <filesystem>
#include <string>
#include <memory>

#include "spdlog/spdlog.h"

#include "../file_handlers/config_manager.hpp"
#include "../file_handlers/file_scanner.hpp"
#include "../file_handlers/csv_reader.hpp"
#include "../file_handlers/csv_writer.hpp"
#include "../data_manager/data_manager.hpp"
#include "../median_calculator/median_calculator.hpp"

/**
 * \brief Главный класс приложения
 * \details Объединяет все компоненты: конфиг, данные, калькулятор, запись
 */
class main_application {
private:
    std::unique_ptr<class config_manager> config_;
    std::unique_ptr<class data_manager> data_;
    std::unique_ptr<class median_calculator> calculator_;
    std::unique_ptr<class csv_writer> writer_;
    
    bool initialize(int argc, char* argv[]);
    bool scan_and_read_files();
    bool calculate_and_write_results();
    void print_statistics() const;
    
public:
    main_application();
    ~main_application();

    /**
     * \brief Запускает приложение
     * \param argc аргументы командной строки
     * \param argv аргументы командной строки
     * \return 0 при успехе, код ошибки иначе
     */
    int run(int argc, char* argv[]);
};