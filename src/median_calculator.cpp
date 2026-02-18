#include "median_calculator.hpp"

void median_calculator::add_to_heap(double value) {
    if(lower_heap_.empty() || value <= lower_heap_.top()) {
        lower_heap_.push(value);
    }
    else {
        upper_heap_.push(value);
    }

    //Необходимо сбалансировать кучи
    if(upper_heap_.size() > lower_heap_.size()) {
        lower_heap_.push(upper_heap_.top()); //Свапаем лишний элемент из большей кучи в меньшую
        upper_heap_.pop();
    }
    else if(lower_heap_.size() > upper_heap_.size() + 1) {
        upper_heap_.push(lower_heap_.top()); //Свапаем лишний элемент из меньшей кучи в большую
        lower_heap_.pop();
    }
}

double median_calculator::calculate_median(){
    double median = 0;
    //Если элементов поровну - берем среднее арифметическое
    if(lower_heap_.size() == upper_heap_.size()) {
        median = (lower_heap_.top() + upper_heap_.top()) / 2;
    }
    //Иначе - большее из меньших
    else {
        median = lower_heap_.top();
    }
    return median;
}