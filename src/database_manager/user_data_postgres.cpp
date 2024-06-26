#include "user_data_postgres.hpp"
#include "postgres_data_conversion.hpp"
#include <iostream>

namespace database_manager{    
    UserDataPostgres::UserDataPostgres(bool is_test) : pool_(CONNECTION_CAPACITY, is_test){
        if(is_test){
            try{
                ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
                pqxx::work trans(*cw);
                trans.exec("DELETE FROM users;");
                trans.commit();
            }
            catch(std::exception& ex){
                std::cout << ex.what() << std::endl;
            }
        }
    }

    bool UserDataPostgres::AddLine(hh::RegistrationData& rd) {
        try{
            ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
            pqxx::work trans(*cw);
            pqxx::row uuid_pqxx = trans.exec1("SELECT gen_random_uuid();");
            trans.exec_params0("INSERT INTO users VALUES ($1, $2, $3);", uuid_pqxx[0].as<Uuid>(), rd.login, rd.password);
            trans.commit();
            return true;
        }
        catch(std::exception& ex){
            std::cout << ex.what() << std::endl;
        }
        return false;
    }
    std::optional<UserData> UserDataPostgres::GetByUuid(const Uuid& uuid) {
        ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE id=$1;", uuid);
            UserData ud = from_result(res);
            return ud;
        } 
        catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
        
    }
    std::optional<UserData> UserDataPostgres::GetByLoginPassword(const Login& login, const Password& password) {
        ConnectionPool::ConnectionWrapper cw = pool_.GetConnection();
        pqxx::read_transaction trans(*cw);

        try {
            pqxx::row res = 
                trans.exec_params1("SELECT * FROM users WHERE login=$1 AND password=$2;", login, password);
            UserData ud = from_result(res);
            return ud;
        } 
        catch (const std::exception& ex) {  
            std::cout << ex.what() << std::endl;
            return std::nullopt;
        }
    }

}