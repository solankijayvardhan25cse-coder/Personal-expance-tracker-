#include "ExpenseSystem.h"
#include <iostream>

int main() {
    try {
        ExpenseSystem system;
        system.run();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal application error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
