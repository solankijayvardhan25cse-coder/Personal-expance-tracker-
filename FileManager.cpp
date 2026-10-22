#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

FileManager::FileManager(const std::filesystem::path& directory) : dataDirectory(directory) { ensureDataDirectory(); }
FileManager::~FileManager() = default;
void FileManager::ensureDataDirectory() const {
    std::error_code ec;
    std::filesystem::create_directories(dataDirectory, ec);
    if (ec) throw std::runtime_error("Could not create Data directory: " + ec.message());
}
std::string FileManager::cleanField(std::string value) {
    for (char& ch : value) if (ch == '|' || ch == '\n' || ch == '\r') ch = ' ';
    return value;
}
User FileManager::loadUser() const {
    std::ifstream in(dataDirectory / "users.txt");
    if (!in) return User();
    std::string line, name, email; int id;
    if (!std::getline(in, line)) return User();
    std::stringstream ss(line); std::string sid;
    if (!std::getline(ss, sid, '|') || !std::getline(ss, name, '|') || !std::getline(ss, email)) return User();
    try { id = std::stoi(sid); return User(id, name, email); } catch (...) { return User(); }
}
std::vector<Category> FileManager::loadCategories() const {
    std::vector<Category> result; std::ifstream in(dataDirectory / "categories.txt");
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line); std::string sid, name;
        if (!std::getline(ss, sid, '|') || !std::getline(ss, name)) continue;
        try { result.emplace_back(std::stoi(sid), name); } catch (...) { /* skip malformed row */ }
    }
    if (result.empty()) {
        result.emplace_back(1, "Food"); result.emplace_back(2, "Transportation");
        result.emplace_back(3, "Education"); result.emplace_back(4, "Shopping"); result.emplace_back(5, "Entertainment");
        result.emplace_back(6, "Healthcare"); result.emplace_back(7, "Other");
    }
    return result;
}
std::vector<Expense> FileManager::loadExpenses() const {
    std::vector<Expense> result; std::ifstream in(dataDirectory / "expenses.txt");
    std::string line;
    while (std::getline(in, line)) {
        std::stringstream ss(line); std::string sid, amount, cat, date, description, payment;
        if (!std::getline(ss,sid,'|') || !std::getline(ss,amount,'|') || !std::getline(ss,cat,'|') ||
            !std::getline(ss,date,'|') || !std::getline(ss,description,'|') || !std::getline(ss,payment)) continue;
        try { result.emplace_back(std::stoi(sid), std::stod(amount), std::stoi(cat), date, description, payment); }
        catch (...) { /* skip malformed row rather than stopping all loading */ }
    }
    return result;
}
std::vector<Budget> FileManager::loadBudgets() const {
    std::vector<Budget> result; std::ifstream in(dataDirectory / "budgets.txt"); std::string line;
    while (std::getline(in,line)) {
        std::stringstream ss(line); std::string id, cat, limit, month;
        if (!std::getline(ss,id,'|') || !std::getline(ss,cat,'|') || !std::getline(ss,limit,'|') || !std::getline(ss,month)) continue;
        try { result.emplace_back(std::stoi(id), std::stoi(cat), std::stod(limit), month); } catch (...) { /* skip malformed row */ }
    }
    return result;
}
void FileManager::saveUser(const User& user) const {
    std::ofstream out(dataDirectory / "users.txt", std::ios::trunc);
    if (!out) throw std::runtime_error("Unable to write users.txt");
    out << user.getUserId() << '|' << cleanField(user.getName()) << '|' << cleanField(user.getEmail()) << '\n';
}
void FileManager::saveCategories(const std::vector<Category>& categories) const {
    std::ofstream out(dataDirectory / "categories.txt", std::ios::trunc);
    if (!out) throw std::runtime_error("Unable to write categories.txt");
    for (const auto& c : categories) out << c.getId() << '|' << cleanField(c.getName()) << '\n';
}
void FileManager::saveExpenses(const Expense* expenses, int count) const {
    std::ofstream out(dataDirectory / "expenses.txt", std::ios::trunc);
    if (!out) throw std::runtime_error("Unable to write expenses.txt");
    out.precision(2); out.setf(std::ios::fixed);
    for (int i=0; i<count; ++i) out << expenses[i].getId() << '|' << expenses[i].getAmount() << '|'
        << expenses[i].getCategoryId() << '|' << expenses[i].getDate() << '|'
        << cleanField(expenses[i].getDescription()) << '|' << cleanField(expenses[i].getPaymentMethod()) << '\n';
}
void FileManager::saveBudgets(const std::vector<Budget>& budgets) const {
    std::ofstream out(dataDirectory / "budgets.txt", std::ios::trunc);
    if (!out) throw std::runtime_error("Unable to write budgets.txt");
    out.precision(2); out.setf(std::ios::fixed);
    for (const auto& b : budgets) out << b.getId() << '|' << b.getCategoryId() << '|' << b.getLimit() << '|' << b.getMonth() << '\n';
}
