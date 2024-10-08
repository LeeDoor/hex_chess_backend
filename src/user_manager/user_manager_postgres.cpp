#include "user_manager_postgres.hpp"
#include "postgres_data_conversion.hpp"
#include "spdlog/spdlog.h"

namespace user_manager{    
    UserManagerPostgres::UserManagerPostgres(cp::ConnectionPool::Ptr pool) 
    : pool_(pool){}

    bool UserManagerPostgres::AddLine(const Login& login, const Password& password) {
        Uuid uuid;
        try{
            cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
            pqxx::work trans(*cw);
            uuid = trans.exec1("SELECT gen_random_uuid();")[0].as<Uuid>();
            trans.exec_params0("INSERT INTO users VALUES ($1, $2, $3);", uuid, login, password);
            trans.commit();
            return true;
        }
        catch(std::exception& ex){
            spdlog::error("database AddLine function terminated with {}", ex.what());
        }
        return false;
    }
    std::optional<User> UserManagerPostgres::GetByUuid(const Uuid& uuid) {
        cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE id=$1;", uuid);
            User ud = UserFromResult(res);
            return ud;
        } 
        catch (const std::exception& ex) {
            spdlog::error("PostgreSQL users GetByUuid({}) {}", uuid, ex.what());
            return std::nullopt;
        }
    }
    std::optional<User> UserManagerPostgres::GetByLogin(const Login& login) {
        cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE login=$1;", login);
            User ud = UserFromResult(res);
            return ud;
        } 
        catch (const std::exception& ex) {  
            spdlog::error("database GetByLoginPassword function terminated with {}", ex.what());
            return std::nullopt;
        }
    }
    std::optional<User> UserManagerPostgres::GetByLoginPassword(const Login& login, const Password& password) {
        cp::ConnectionPool::ConnectionWrapper cw = pool_->GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE login=$1 AND password=$2;", login, password);
            User ud = UserFromResult(res);
            return ud;
        } 
        catch (const std::exception& ex) {  
            spdlog::error("database GetByLoginPassword function terminated with {}", ex.what());
            return std::nullopt;
        }
    }
}