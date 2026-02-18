#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>

struct list_trades {
    uint64_t timestamp;
    double price;
};

class data_manager {
private:
    std::vector<list_trades> records_;
    
public:
    void add_record(uint64_t ts, double price);
    
    void sort_by_timestamp();
    const std::vector<list_trades>& get_records() const { return records_; }
    
    void clear() { records_.clear(); }
    size_t size() const { return records_.size(); }
    bool empty() const { return records_.empty(); }

    void print_n_records(size_t n);
};