#include <queue>
#include <string>

class median_calculator {
    private:
        // Куча хранения значений <= медианы
        std::priority_queue<double> lower_heap_;
        // Куча для хранения значений > медианы(используем шаблон priority_queue)
        std::priority_queue<double, std::vector<double>, std::greater<double>> upper_heap_;

    public:
        void add_to_heap(double value);
        double calculate_median();
};