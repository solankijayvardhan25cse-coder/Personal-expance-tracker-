#include "User.h"
#include <stdexcept>

User::User() : userId(1), name("Default User"), email("not-set@example.com") {}
User::User(int id, const std::string& userName, const std::string& userEmail)
    : userId(id), name(userName), email(userEmail) {}
User::~User() = default;
int User::getUserId() const { return userId; }
const std::string& User::getName() const { return name; }
const std::string& User::getEmail() const { return email; }
void User::setProfile(const std::string& userName, const std::string& userEmail) {
    if (userName.empty()) throw std::invalid_argument("Name cannot be empty.");
    if (userEmail.find('@') == std::string::npos) throw std::invalid_argument("Enter a valid email containing @.");
    name = userName; email = userEmail;
}
bool User::isValid() const { return !name.empty() && email.find('@') != std::string::npos; }
