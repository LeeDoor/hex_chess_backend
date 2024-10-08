#include "serializer_basic.hpp"
#include "serializer_game.hpp"
#include "serializer_user.hpp"
#include "serializer_http.hpp"
#include "serializer_session.hpp"

#include "spdlog/spdlog.h"
#include "json_type_converter.hpp"
#include "type_serializer.hpp"
#include "const_variables.hpp"
#define EMPTY_JSON "{}"


namespace serializer{
    // BASIC //
    std::string SerializeEmpty() {
        return EMPTY_JSON;
    }
    std::string SerializeError(const std::string& error_name, const std::string& desc) {
        nlohmann::json obj = nlohmann::json::object();
        obj["error_name"] = std::move(error_name);
        obj["description"] = std::move(desc);
        return obj.dump();
    }
    std::string SerializeMap(std::map<std::string, std::string>&& map) {
        nlohmann::json obj;
        for (const auto& pair : map) {
            obj[pair.first] = pair.second;
        }
        return obj.dump();
    } 
    
    std::optional<std::map<std::string, std::string>> DeserializeMap(const std::string& json_str) {
        std::map<std::string, std::string> map;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            for(auto pair : j.items()){
                map[pair.key()] = pair.value();
            }
        }
        catch(std::exception ex){
            return std::nullopt;
        }
        return map;
    }

    // HTTP //
    std::string Serialize(const hh::RegistrationData& rd) {
        nlohmann::json json = rd;
        return json.dump();
    }
    std::string Serialize(const hh::PublicUser& pud) {
        nlohmann::json json = pud;
        return json.dump();
    }
    std::optional<hh::PublicUser>       DeserializePublicUser(const std::string& json_str) {
        hh::PublicUser pud;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            pud = j.template get<hh::PublicUser>();
        }
        catch(std::exception ex){
            return std::nullopt;
        }
        return pud;
    }
    std::optional<hh::RegistrationData> DeserializeRegData(const std::string& json_str) {
        hh::RegistrationData rd;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            rd = j.template get<hh::RegistrationData>();
        }
        catch(std::exception ex){
            return std::nullopt;
        }
        return rd;
    }
 
    // USER //
    std::string Serialize(const um::User& ud) {
        nlohmann::json json = ud;
        return json.dump();
    }
    std::string Serialize(const std::vector<um::Uuid>& v) {
        nlohmann::json obj(v);
        return obj.dump();
    }
    std::string Serialize(const std::map<tm::Token,um::Uuid>& ttu) {
        if(ttu.empty()){
            return SerializeEmpty();
        }
        nlohmann::json obj;
        for (const auto& pair : ttu) {
            obj[pair.first] = pair.second;
        }
        return obj.dump();
    }
    
    std::optional<um::User>              DeserializeUser(const std::string& json_str) {
        um::User ud;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            ud = j.template get<um::User>();
        }
        catch(std::exception ex){
            return std::nullopt;
        }
        return ud;
    }
    std::optional<std::vector<um::Uuid>> DeserializeUuids(const std::string& json_str) {
        std::vector<um::Uuid> res;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            j.get_to(res);
        }
        catch(std::exception& ex){
            return std::nullopt;
        }
        return res;
    }

    // GAME // 
    std::string Serialize(const gm::State& state) {
        nlohmann::json obj(state);
        return obj.dump();
    }
    std::string Serialize(const gm::MoveData& wd) {
        nlohmann::json obj(wd);
        return obj.dump();
    }
    std::string Serialize(const gm::EventListWrapper::Vec& elwv) {
        nlohmann::json obj(elwv);
        return obj.dump();
    }
    std::string SerializeGameConst() {
        nlohmann::json j;
        j["bomb_ticks_left"] = BOMB_TICKS_LEFT;
        j["bomb_explode_radius"] = BOMB_EXPLODE_RADIUS;
        j["bomb_place_radius"] = BOMB_PLACE_RADIUS;
        j["gun_cooldown"] = GUN_COOLDOWN;
        j["gun_shots"] = GUN_SHOTS;
        j["gun_place_radius"] = GUN_PLACE_RADIUS;
        return j.dump();
    }

    std::optional<gm::State>             DeserializeSessionState(const std::string& json_str) {
        gm::State res;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            res = j.get_to(res);
        }
        catch(std::exception& ex){
            return std::nullopt;
        }
        return res;
    }
    std::optional<gm::MoveData> DeserializeMoveData(const std::string& json_str) {
        gm::MoveData res;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            res = j.get_to(res);
        }
        catch(std::exception& ex){
            return std::nullopt;
        }
        return res;
    }
    std::optional<int> DeserializeFromMove(const std::string& json) {
        int res;
        try{
            nlohmann::json j = nlohmann::json::parse(json);
            j.at("from_move").get_to(res);
        }       
        catch(std::exception& ex){
            return std::nullopt;
        }
        return res;
    }

    // SESSION //
    std::string Serialize(const session_manager::PublicSessionData& data) {
        nlohmann::json obj(data);
        return obj.dump();
    }

    bool                                              DefineSessionState(const std::string& json_str) {
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            return j["state"] == "playing";
        }
        catch(std::exception& ex){
            return false;
        }
        return false;
    }
    std::optional<session_manager::PublicSessionData> DeserializePublicSessionData(const std::string& json_str) {
        session_manager::PublicSessionData res;
        try{
            nlohmann::json j = nlohmann::json::parse(json_str);
            j.get_to(res);
        }
        catch(std::exception& ex){
            return std::nullopt;
        }
        return res;
    }
}
