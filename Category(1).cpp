#include "Category.h"
#include <stdexcept>
Category::Category() : categoryId(0), name("Other") {}
Category::Category(int id, const std::string& categoryName) : categoryId(id), name(categoryName) {
    if (id < 0) throw std::invalid_argument("Category ID cannot be negative.");
    if (categoryName.empty()) throw std::invalid_argument("Category name cannot be empty.");
}
Category::~Category() = default;
int Category::getId() const { return categoryId; }
const std::string& Category::getName() const { return name; }
void Category::setName(const std::string& categoryName) {
    if (categoryName.empty()) throw std::invalid_argument("Category name cannot be empty.");
    name = categoryName;
}
