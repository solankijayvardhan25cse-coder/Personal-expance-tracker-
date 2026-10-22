#include "ExpenseSystem.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <stdexcept>
#include <cctype>

namespace {
void clearInput() { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }
std::string readLine(const std::string& prompt) { std::cout << prompt; std::string value; std::getline(std::cin, value); return value; }
int readInt(const std::string& prompt) {
    while (true) { std::cout << prompt; int value; if (std::cin >> value) { clearInput(); return value; }
        std::cout << "Invalid input. Enter a whole number.\n"; clearInput(); }
}
double readDouble(const std::string& prompt) {
    while (true) { std::cout << prompt; double value; if (std::cin >> value && value > 0.0) { clearInput(); return value; }
        std::cout << "Invalid input. Enter a number greater than zero.\n"; clearInput(); }
}
std::string currentMonthFromDate(const std::string& date) { return date.size() >= 7 ? date.substr(0,7) : ""; }
}

ExpenseSystem::ExpenseSystem() : user(), expenses(new Expense[5]), count(0), capacity(5), categories(), budgets(), fileManager("Data") {
    loadAll();
}
ExpenseSystem::~ExpenseSystem() { delete[] expenses; }
void ExpenseSystem::growIfNeeded() {
    if (count < capacity) return;
    int newCapacity = capacity * 2;
    Expense* replacement = new Expense[newCapacity];
    for (int i=0; i<count; ++i) replacement[i] = expenses[i];
    delete[] expenses; expenses = replacement; capacity = newCapacity;
}
int ExpenseSystem::findExpenseIndex(int id) const { for (int i=0; i<count; ++i) if (expenses[i].getId() == id) return i; return -1; }
int ExpenseSystem::findCategoryIndex(int id) const { for (std::size_t i=0; i<categories.size(); ++i) if (categories[i].getId() == id) return static_cast<int>(i); return -1; }
int ExpenseSystem::nextExpenseId() const { int highest=0; for (int i=0;i<count;++i) highest=std::max(highest,expenses[i].getId()); return highest+1; }
int ExpenseSystem::nextBudgetId() const { int highest=0; for (const auto& b:budgets) highest=std::max(highest,b.getId()); return highest+1; }
double ExpenseSystem::totalForMonth(const std::string& month, int categoryId) const {
    double total=0; for (int i=0;i<count;++i) if (currentMonthFromDate(expenses[i].getDate())==month && (categoryId==-1 || expenses[i].getCategoryId()==categoryId)) total+=expenses[i].getAmount(); return total;
}
void ExpenseSystem::loadAll() {
    user = fileManager.loadUser(); categories = fileManager.loadCategories(); budgets = fileManager.loadBudgets();
    std::vector<Expense> loaded = fileManager.loadExpenses();
    delete[] expenses; capacity = std::max(5, static_cast<int>(loaded.size()) * 2); expenses = new Expense[capacity]; count=0;
    for (const auto& expense : loaded) {
        if (findExpenseIndex(expense.getId()) == -1) expenses[count++] = expense;
    }
    if (categories.empty()) categories.emplace_back(7,"Other");
}
void ExpenseSystem::saveAll() const {
    fileManager.saveUser(user); fileManager.saveCategories(categories); fileManager.saveExpenses(expenses,count); fileManager.saveBudgets(budgets);
}
void ExpenseSystem::printExpenseTable() const {
    if (count==0) { std::cout << "No expenses recorded yet.\n"; return; }
    std::cout << std::left << std::setw(8) << "ID" << std::setw(12) << "Amount" << std::setw(12) << "Date" << std::setw(16) << "Category ID" << std::setw(18) << "Payment" << "Description\n";
    std::cout << std::string(78,'-') << '\n';
    // Runtime polymorphism: invoke the overridden Expense::display through a Record reference.
    for (int i=0;i<count;++i) { const Record& record = expenses[i]; record.display(std::cout); }
}
void ExpenseSystem::addExpense() {
    try {
        std::cout << "\nAvailable categories:\n";
        for (const auto& c:categories) std::cout << c.getId() << ". " << c.getName() << '\n';
        int categoryId = readInt("Category ID: ");
        if (findCategoryIndex(categoryId)==-1) throw std::invalid_argument("That category ID does not exist.");
        double amount = readDouble("Amount: ");
        std::string date = readLine("Date (YYYY-MM-DD): ");
        if (!Expense::isValidDate(date)) throw std::invalid_argument("Invalid date. Use a real date in YYYY-MM-DD format.");
        std::string description = readLine("Description: ");
        std::string payment = readLine("Payment method (Cash/UPI/Card/etc.): ");
        Expense item(nextExpenseId(), amount, categoryId, date, description, payment);
        growIfNeeded(); expenses[count++] = item;
        saveAll();
        std::cout << "Expense added successfully. Assigned ID: " << item.getId() << "\n";
        processTransaction();
    } catch (const std::exception& ex) { std::cout << "Could not add expense: " << ex.what() << '\n'; }
}
void ExpenseSystem::displayExpenses() const { std::cout << "\n========== ALL EXPENSES ==========\n"; printExpenseTable(); }
void ExpenseSystem::searchExpense() const {
    std::cout << "1. Search by ID\n2. Search by category ID\n3. Search by date\n";
    int choice=readInt("Choose search type: "); bool found=false;
    if (choice==1) { int id=readInt("Expense ID: "); int i=findExpenseIndex(id); if(i>=0){ expenses[i].display(std::cout); found=true; } }
    else if (choice==2) { int id=readInt("Category ID: "); for(int i=0;i<count;++i) if(expenses[i].getCategoryId()==id){expenses[i].display(std::cout);found=true;} }
    else if (choice==3) { std::string date=readLine("Date (YYYY-MM-DD): "); for(int i=0;i<count;++i) if(expenses[i].getDate()==date){expenses[i].display(std::cout);found=true;} }
    else { std::cout << "Invalid search option.\n"; return; }
    if(!found) std::cout << "No matching expense found.\n";
}
void ExpenseSystem::updateExpense() {
    int id=readInt("Enter expense ID to update: "); int index=findExpenseIndex(id);
    if(index<0){std::cout << "Expense not found.\n";return;}
    try {
        std::cout << "Enter replacement details.\n";
        for(const auto& c:categories) std::cout << c.getId() << ". " << c.getName() << '\n';
        int cat=readInt("Category ID: "); if(findCategoryIndex(cat)<0) throw std::invalid_argument("Unknown category ID.");
        double amount=readDouble("Amount: "); std::string date=readLine("Date (YYYY-MM-DD): ");
        std::string desc=readLine("Description: "); std::string payment=readLine("Payment method: ");
        Expense replacement(id,amount,cat,date,desc,payment); expenses[index]=replacement; saveAll();
        std::cout << "Expense updated successfully.\n";
    } catch(const std::exception& ex){std::cout << "Update failed: " << ex.what() << '\n';}
}
void ExpenseSystem::deleteExpense() {
    int id=readInt("Enter expense ID to delete: "); int index=findExpenseIndex(id);
    if(index<0){std::cout << "Expense not found.\n";return;}
    std::string confirm=readLine("Delete this expense? Type YES to confirm: ");
    if(confirm!="YES"){std::cout << "Deletion cancelled.\n";return;}
    for (int i = index; i < count - 1; ++i) {
        expenses[i] = expenses[i + 1];
    }
    --count;
    try { saveAll(); std::cout << "Expense deleted successfully.\n"; }
    catch(const std::exception& ex){std::cout << "Deleted in memory, but saving failed: " << ex.what() << '\n';}
}
void ExpenseSystem::processTransaction() const {
    if(count==0){std::cout << "No expenses to process. Add an expense first.\n";return;}
    std::string month=readLine("Month to process (YYYY-MM): ");
    if(!Budget::isValidMonth(month)){std::cout << "Invalid month format.\n";return;}
    double total=totalForMonth(month);
    std::cout << std::fixed << std::setprecision(2) << "Total spending for " << month << ": " << total << '\n';
    bool hasBudget=false;
    for(const auto& b:budgets) if(b.getMonth()==month && b.getCategoryId()==0){
        hasBudget=true; double remaining=b.remaining(total);
        std::cout << "Overall budget: " << b.getLimit() << ", remaining: " << remaining << '\n';
        if(b.isExceeded(total)) std::cout << "WARNING: Overall budget exceeded.\n";
        else if(total >= b.getLimit()*0.8) std::cout << "Warning: You have used at least 80% of the budget.\n";
    }
    for(const auto& b:budgets) if(b.getMonth()==month && b.getCategoryId()!=0){
        double spent=totalForMonth(month,b.getCategoryId());
        std::cout << "Category ID " << b.getCategoryId() << " spent " << spent << " / " << b.getLimit() << '\n';
        if(b.isExceeded(spent)) std::cout << "WARNING: Category budget exceeded.\n";
        else if(spent >= b.getLimit()*0.8) std::cout << "Warning: Category budget is at least 80% used.\n";
        hasBudget=true;
    }
    if(!hasBudget) std::cout << "No budget is configured for this month. Use Report > Set budget to add one.\n";
}
void ExpenseSystem::showReport() const {
    std::string month=readLine("Enter report month (YYYY-MM): ");
    if(!Budget::isValidMonth(month)){std::cout << "Invalid month format.\n";return;}
    std::cout << "\n========== MONTHLY REPORT: " << month << " ==========\n";
    double total=totalForMonth(month); std::cout << std::fixed << std::setprecision(2) << "Total expenses: " << total << '\n';
    std::cout << "Category-wise spending:\n";
    for(const auto& c:categories) { double subtotal=totalForMonth(month,c.getId()); if(subtotal>0) std::cout << "  " << c.getName() << " (ID " << c.getId() << "): " << subtotal << '\n'; }
    bool budgetFound=false;
    for(const auto& b:budgets) if(b.getMonth()==month){ double spent=b.getCategoryId()==0?total:totalForMonth(month,b.getCategoryId());
        std::cout << "Budget " << b.getId() << " (" << (b.getCategoryId()==0?"all categories":"category ID "+std::to_string(b.getCategoryId()))
                  << "): limit " << b.getLimit() << ", remaining " << b.remaining(spent) << '\n';
        if(b.isExceeded(spent)) std::cout << "  Status: EXCEEDED\n"; else if(spent>=b.getLimit()*0.8) std::cout << "  Status: WARNING (80% or more used)\n"; else std::cout << "  Status: Within budget\n";
        budgetFound=true;
    }
    if(!budgetFound) std::cout << "No budget configured for this month.\n";
    std::cout << "=============================================\n";
}
void ExpenseSystem::manageBudget() {
    std::cout << "1. Set budget\n2. List budgets\n"; int choice=readInt("Choose: ");
    if(choice==2){ if(budgets.empty()) std::cout << "No budgets configured.\n"; for(const auto& b:budgets) std::cout << "ID "<<b.getId()<<", month "<<b.getMonth()<<", category "<<(b.getCategoryId()==0?"All":std::to_string(b.getCategoryId()))<<", limit "<<b.getLimit()<<'\n'; return; }
    if(choice!=1){std::cout << "Invalid option.\n";return;}
    try {
        std::cout << "Use category ID 0 for an overall budget.\n"; int cat=readInt("Category ID: ");
        if(cat!=0 && findCategoryIndex(cat)<0) throw std::invalid_argument("Unknown category ID.");
        double limit=readDouble("Budget limit: "); std::string month=readLine("Month (YYYY-MM): ");
        if(!Budget::isValidMonth(month)) throw std::invalid_argument("Invalid month.");
        for(auto& b:budgets) if(b.getCategoryId()==cat && b.getMonth()==month){std::cout << "A budget already exists for that category and month. Updating it.\n";
            // Budget is intentionally immutable after construction; replace the matching record while retaining its ID.
            b=Budget(b.getId(),cat,limit,month); saveAll(); std::cout << "Budget updated.\n"; return; }
        budgets.emplace_back(nextBudgetId(),cat,limit,month); saveAll(); std::cout << "Budget saved.\n";
    } catch(const std::exception& ex){std::cout << "Could not set budget: "<<ex.what()<<'\n';}
}
void ExpenseSystem::manageProfile() {
    std::cout << "Current profile: " << user.getName() << " <" << user.getEmail() << ">\n";
    std::string name=readLine("Name: "), email=readLine("Email: ");
    try { user.setProfile(name,email); saveAll(); std::cout << "Profile saved.\n"; }
    catch(const std::exception& ex){std::cout << "Profile not changed: " << ex.what() << '\n';}
}
void ExpenseSystem::run() {
    bool running=true;
    while(running){
        std::cout << "\n========================================\n PERSONAL EXPENSE TRACKER\n========================================\n"
                  << "1. Add Record\n2. Display Records\n3. Search Record\n4. Update Record\n5. Delete Record\n"
                  << "6. Main Transaction/Process\n7. Report\n8. Save/Load\n9. Exit\n10. Manage Budget (extra)\n11. Manage User Profile (extra)\n";
        int choice=readInt("Enter your choice: ");
        try {
            switch(choice){case 1:addExpense();break;case 2:displayExpenses();break;case 3:searchExpense();break;
                case 4:updateExpense();break;case 5:deleteExpense();break;case 6:processTransaction();break;
                case 7:showReport();break;case 8:{std::cout<<"1. Save all data\n2. Reload from files\n";int sub=readInt("Choose: ");
                    if(sub==1){saveAll();std::cout<<"All records saved.\n";}else if(sub==2){loadAll();std::cout<<"Data reloaded from files.\n";}else std::cout<<"Invalid option.\n";break;}
                case 9:saveAll();std::cout<<"Data saved. Goodbye.\n";running=false;break;
                case 10:manageBudget();break;case 11:manageProfile();break;
                default:std::cout<<"Invalid menu choice. Enter a number from 1 to 11.\n";
            }
        } catch(const std::exception& ex){std::cout<<"Operation failed: "<<ex.what()<<'\n';}
    }
}
