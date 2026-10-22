#include "Budget.h"
#include <stdexcept>
#include <cctype>
Budget::Budget() : budgetId(0), categoryId(0), limitAmount(0), month("1970-01") {}
Budget::Budget(int id, int catId, double limit, const std::string& monthValue)
    : budgetId(id), categoryId(catId), limitAmount(limit), month(monthValue) {
    if (id <= 0 || catId < 0 || limit <= 0 || !isValidMonth(monthValue))
        throw std::invalid_argument("Invalid budget details.");
}
Budget::~Budget() = default;
int Budget::getId() const { return budgetId; }
int Budget::getCategoryId() const { return categoryId; }
double Budget::getLimit() const { return limitAmount; }
const std::string& Budget::getMonth() const { return month; }
double Budget::remaining(double spent) const { return limitAmount - spent; }
bool Budget::isExceeded(double spent) const { return spent > limitAmount; }
bool Budget::isValidMonth(const std::string& value) {
    if (value.size() != 7 || value[4] != '-') return false;
    for (std::size_t i=0; i<value.size(); ++i) if (i != 4 && !std::isdigit(static_cast<unsigned char>(value[i]))) return false;
    int year = std::stoi(value.substr(0,4)), month = std::stoi(value.substr(5,2));
    return year >= 1900 && month >= 1 && month <= 12;
}