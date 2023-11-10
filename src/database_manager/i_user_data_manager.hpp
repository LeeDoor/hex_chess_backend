#pragma once
#include "user_data.hpp"
#include <optional>
#include <memory>

namespace database_manager{
    class IUserDataManager{
    public:
        using Ptr = std::shared_ptr<IUserDataManager>;

        virtual ids::uuid GenerateUuid() = 0;
        virtual bool AddLine(UserData&& user_data) = 0;
        virtual std::optional<UserData> GetByUuid(ids::uuid&& uuid) = 0;
        virtual std::optional<UserData> GetByLoginPassword(std::string&& login, std::string&& password) = 0;
    };
}