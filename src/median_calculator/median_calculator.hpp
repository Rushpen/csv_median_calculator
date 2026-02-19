#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

/**
 * \brief Класс для инкрементального вычисления медианы
 * \details Использует Boost.Accumulators с алгоритмом P²
 */
class median_calculator {
    private:
    // Аккумулятор для потокового вычисления медианы
        boost::accumulators::accumulator_set<
        double, boost::accumulators::stats<boost::accumulators::
            tag::median>> median_accumulator_;

        double last_median_ = 0; ///< Последнее вычисленное значение медианы
        bool is_first_ = true;  ///< Флаг первого добавления

    public:
        median_calculator();
        ~median_calculator();

         /**
         * \brief Добавляет новую цену в расчёт
         * \param price цена для добавления
         */
        void add_price(double price);
        double get_median();

        /**
        * \brief Проверяет, изменилась ли медиана
        * \param current_median текущая медиана
        * \return true если изменилась
        */
        bool has_median_changed(double current_median);
};