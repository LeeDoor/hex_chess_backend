#include "registration_data.hpp"
#include <algorithm>

namespace http_handler{
    bool ValidateRegData(const RegistrationData& rd) {
        return ValidateLogin(rd.login) && ValidatePassword(rd.password);
    }
    bool ValidateLogin(const um::Login& login) {
        return login.size() >= 3;
    }
    bool ValidatePassword(const um::Password& password) {
        return password.size() >= 6 && std::count_if(password.begin(), password.end(), [&](char s){return s >='0'&&s<='9';}) >= 1;
    }
}