#include <vector>
#include <map>
#include <string>
#include <cmath>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

class metrics_calculator {
    private:
        boost::accumulators::accumulator_set<
            double, boost::accumulators::stats<
            boost::accumulators::tag::mean,
            boost::accumulators::tag::median,
            boost::accumulators::tag::variance,
            boost::accumulators::tag::min,
            boost::accumulators::tag::max
        >> accumulator_;
    
        std::vector<double> percentiles_; 
        std::map<std::string, double> last_values_;
        bool is_first_ = true;

    public:
        explicit metrics_calculator();
        ~metrics_calculator() = default;

        void add_value(double value);

        /**
         * \brief Методы для подсчёта основных
         * \return словарь {имя_метрики: значение}
         */
        double get_median() const;
        double get_mean() const;
        double get_stddev() const;
        double get_variance() const;
        double get_min() const;
        double get_max() const;
        double get_quantile(double p) const;
        size_t get_count() const;

        std::map<std::string, double> get_all_metrics() const;

        /**
         * \brief Функция для проверки изменилась ли хоть 1 метрика
         * \return словарь {имя_метрики: значение}
         */
        bool has_changed(const std::map<std::string, double>& current);
};  