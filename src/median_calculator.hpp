#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>


//sudo apt-get install libboost-all-dev

class median_calculator {
    private:
        boost::accumulators::accumulator_set<
        double, boost::accumulators::stats<boost::accumulators::tag::median>> median_accumulator_;

        double last_median_ = 0;
        bool is_first_ = true;

    public:
        median_calculator();
        void add_price(double price);
        double get_median();
        bool has_median_changed(double current_median);
};