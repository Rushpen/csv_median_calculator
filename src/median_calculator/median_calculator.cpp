#include "median_calculator.hpp"

median_calculator::median_calculator() :
    last_median_(0.0), is_first_(true) {}

median_calculator::~median_calculator = default;

void median_calculator::add_price(double price) {
    median_accumulator_(price);
}
    
double median_calculator::get_median() {
    return boost::accumulators::median(median_accumulator_);
}
    
bool median_calculator::has_median_changed(double current_median) {
    if (is_first_ || current_median != last_median_) {
        last_median_ = current_median;
        is_first_ = false;
        return true;
    }
    return false;
}