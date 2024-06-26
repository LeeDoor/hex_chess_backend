#pragma once
#include <string>
#include "user_data.hpp"
#include "http_types.hpp"
namespace http_handler{

    struct PublicUserData{
        PublicUserData() = default;
        PublicUserData(dm::UserData ud){
            login = ud.login;
            password = ud.password;
        }
        dm::Login login;
        dm::Password password;
    };

}