#include "data_manager.hpp"

data_manager:: data_manager() = default;

data_manager::~data_manager() = default;

void data_manager::add_record(uint64_t ts, double price) {
    records_.push_back({ts, price});
}
    
void data_manager::sort_by_timestamp() {
    std::sort(records_.begin(), records_.end(),
        [](const auto& a, const auto& b) {
            return a.timestamp < b.timestamp;
        });
}

void data_manager::print_n_records(size_t n) {
    sort_by_timestamp();
    for (int i = 0; i < std::min(n, records_.size()); i++){
        spdlog::trace("{}  {}", records_[i].timestamp, records_[i].price);
    }
}