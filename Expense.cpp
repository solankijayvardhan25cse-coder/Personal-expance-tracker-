#include "Expense.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <cctype>

Expense::Expense() : expenseId(0), amount(0.0), categoryId(0), date("1970-01-01"), description(""), paymentMethod("Cash") {}
Expense::Expense(int id, double value, int catId, const std::string& expenseDate,
                 const std::string& details, const std::string& payment)
    : expenseId(id), amount(value), categoryId(catId), date(expenseDate), description(details), paymentMethod(payment) {
    if (id <= 0) throw std::invalid_argument("Expense ID must be a positive integer.");
    setDetails(value, catId, expenseDate, details, payment);
}
Expense::~Expense() = default;
int Expense::getId() const { return expenseId; }
double Expense::getAmount() const { return amount; }
int Expense::getCategoryId() const { return categoryId; }
const std::string& Expense::getDate() const { return date; }
const std::string& Expense::getDescription() const { return description; }
const std::string& Expense::getPaymentMethod() const { return paymentMethod; }
void Expense::setDetails(double value, int catId, const std::string& expenseDate,
                         const std::string& details, const std::string& payment) {
    if (value <= 0.0) throw std::invalid_argument("Amount must be greater than zero.");
    if (catId < 0) throw std::invalid_argument("Category ID cannot be negative.");
    if (!isValidDate(expenseDate)) throw std::invalid_argument("Date must be a real date in YYYY-MM-DD format.");
    if (details.empty()) throw std::invalid_argument("Description cannot be empty.");
    if (payment.empty()) throw std::invalid_argument("Payment method cannot be empty.");
    amount = value; categoryId = catId; date = expenseDate; description = details; paymentMethod = payment;
}
void Expense::display(std::ostream& out) const {
    out << std::left << std::setw(8) << expenseId << std::setw(12) << std::fixed << std::setprecision(2)
        << amount << std::setw(12) << date << std::setw(16) << categoryId
        << std::setw(18) << paymentMethod << description << '\n';
}
bool Expense::isValidDate(const std::string& value) {
    if (value.size() != 10 || value[4] != '-' || value[7] != '-') return false;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(value[i]))) return false;
    }
    int year = std::stoi(value.substr(0,4)), month = std::stoi(value.substr(5,2)), day = std::stoi(value.substr(8,2));
    if (year < 1900 || month < 1 || month > 12) return false;
    const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int maxDay = days[month-1];
    bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    if (month == 2 && leap) maxDay = 29;
    return day >= 1 && day <= maxDay;
}
