#include "metrics_calculator.hpp"

metrics_calculator::metrics_calculator() 
    : is_first_(true) {}

// Всё подсчитываем через Boost.Accumulators
void metrics_calculator::add_value(double value) {
    accumulator_(value);
}

double metrics_calculator::get_mean() const {
    return boost::accumulators::mean(accumulator_);
}

double metrics_calculator::get_median() const {
    return boost::accumulators::median(accumulator_);
}

double metrics_calculator::get_variance() const {
    return boost::accumulators::variance(accumulator_);
}

double metrics_calculator::get_stddev() const {
    return std::sqrt(get_variance());
}

double metrics_calculator::get_min() const {
    return boost::accumulators::min(accumulator_);
}

double metrics_calculator::get_max() const {
    return boost::accumulators::max(accumulator_);
}

size_t metrics_calculator::get_count() const {
    return boost::accumulators::count(accumulator_);
}

// Функция для возврата всей 'карты' метрик
std::map<std::string, double> metrics_calculator::get_all_metrics() const {
    std::map<std::string, double> result;
    
    result["mean"] = get_mean();
    result["median"] = get_median();
    result["stddev"] = get_stddev();
    result["min"] = get_min();
    result["max"] = get_max();
    result["count"] = static_cast<double>(get_count());

    return result;
}

// Функция проверки изменился ли хотя бы один параметр
bool metrics_calculator::has_changed(const std::map<std::string, 
    double>& current) {
    if (is_first_) {
        last_values_ = current;
        is_first_ = false;
        return true;
    }
    
    bool changed = false;
    for (const auto& [name, value] : current) {
        // Проверяем только числовые метрики (не count)
        if (name != "count" && std::abs(value - last_values_[name]) > 1e-10) {
            changed = true;
            break;
        }
    }
    
    if (changed) {
        last_values_ = current;
    }
    return changed;
}