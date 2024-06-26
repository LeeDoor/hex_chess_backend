#include <catch2/catch_test_macros.hpp>
#include "json_serializer.hpp"
#include "registration_data.hpp"
#include "public_user_data.hpp"
#include "token.hpp"
#include <nlohmann/json.hpp>

using namespace serializer;
using json = nlohmann::json;
namespace hh = http_handler;

struct Error{
    std::string error_name;
    std::string description;
};
void to_json(json& j, const Error& p) {
    j = json{{"error_name", p.error_name}, {"description", p.description}};
}
void from_json(const json& j, Error& p) {
    j.at("error_name").get_to(p.error_name);
    j.at("description").get_to(p.description);
}
void CompareErrors(const Error& given, const Error& expected){
    if(given.error_name != expected.error_name){
        FAIL_CHECK("error_name is wrong. given: " << given.error_name
                         << " expected: " << expected.error_name);
    }
    if(given.description != expected.description){
        FAIL_CHECK("description is wrong. given: " << given.description
                         << " expected: " << expected.description);
    }
}

TEST_CASE("SerializeEmpty", "[jsonserializer]"){
    JSONSerializer serializer;
    REQUIRE(serializer.SerializeEmpty() == "{}");
}

TEST_CASE("SerializeError", "[jsonserializer]") {
    JSONSerializer serializer;
    json j;
    Error given;

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("error name 1", "description 1")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"error name 1", "description 1"});

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("", "description 1")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"", "description 1"});

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("error name 1", "")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"error name 1", ""});

    REQUIRE_NOTHROW(j = json::parse(serializer.SerializeError("", "")));
    REQUIRE_NOTHROW(given = j.template get<Error>());
    CompareErrors(given, {"", ""});
}

TEST_CASE("SerializeMap & DeserializeMap", "[jsonserializer]") {
    using StringMap = std::map<std::string, std::string>;
    
    JSONSerializer serializer;
    StringMap map;
    StringMap given;
    json j;

    SECTION("SerializeMap"){
        map = {{"first", "second"}, {"third", "fourth"}};
        REQUIRE_NOTHROW(j = json::parse(serializer.SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);

        map = {{"", ""}};
        REQUIRE_NOTHROW(j = json::parse(serializer.SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);
    }
    SECTION("DeserializeMap"){
        map = {{"biber", "dolik"}, {"dodster", "pizza"}};
        for(auto pair : map){
            j[pair.first] = pair.second;
        }
        auto is_given = serializer.DeserializeMap(j.dump());
        REQUIRE(is_given);
        given = *is_given;
        for(auto pair : given){
            REQUIRE(map.contains(pair.first));
            CHECK(pair.second == map.at(pair.first));
        }
    }
}

TEST_CASE("SerializeRegData & DeserializeRegData", "[jsonserializer]") {
    JSONSerializer serializer;
    SECTION ("SerializeRegData") {
        std::string rd_given = serializer.SerializeRegData({"login1", "password1"});
        CHECK(rd_given == "{\"login\":\"login1\",\"password\":\"password1\"}");
        rd_given = serializer.SerializeRegData({"login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"});
        CHECK(rd_given == "{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        rd_given = serializer.SerializeRegData({"", ""});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"\"}");
        rd_given = serializer.SerializeRegData({"123qwe123", ""});
        CHECK(rd_given == "{\"login\":\"123qwe123\",\"password\":\"\"}");
        rd_given = serializer.SerializeRegData({"", "123qwe123"});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"123qwe123\"}");
        rd_given = serializer.SerializeRegData({"login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"});
        CHECK(rd_given == "{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
    }
    SECTION ("DeserializeRegData") {
        hh::RegistrationData rd;
        rd = {"login1", "password1"};
        std::optional<hh::RegistrationData> rd_given;
        rd_given = serializer.DeserializeRegData("{\"login\":\"login1\",\"password\":\"password1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"};
        rd_given = serializer.DeserializeRegData("{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"", ""};
        rd_given = serializer.DeserializeRegData("{\"login\":\"\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"123123123", ""};
        rd_given = serializer.DeserializeRegData("{\"login\":\"123123123\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {"login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"};
        rd_given = serializer.DeserializeRegData("{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);
    }
}

TEST_CASE("SerializePublicUserData & DeserializePublicUserData", "[jsonserializer]") {
    JSONSerializer serializer;
    SECTION ("SerializePublicUserData") {
        std::string rd_given = serializer.SerializePublicUserData({{"uuid", "login1", "password1"}});
        CHECK(rd_given == "{\"login\":\"login1\",\"password\":\"password1\"}");
        rd_given = serializer.SerializePublicUserData({{"uuid", "login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"}});
        CHECK(rd_given == "{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        rd_given = serializer.SerializePublicUserData({{"uuid", "", ""}});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"\"}");
        rd_given = serializer.SerializePublicUserData({{"uuid", "123qwe123", ""}});
        CHECK(rd_given == "{\"login\":\"123qwe123\",\"password\":\"\"}");
        rd_given = serializer.SerializePublicUserData({{"uuid", "", "123qwe123"}});
        CHECK(rd_given == "{\"login\":\"\",\"password\":\"123qwe123\"}");
        rd_given = serializer.SerializePublicUserData({{"uuid", "login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"}});
        CHECK(rd_given == "{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
    }
    SECTION ("DeserializePublicUserData") {
        hh::PublicUserData rd;
        rd = {{"uuid", "login1", "password1"}};
        std::optional<hh::PublicUserData> rd_given;
        rd_given = serializer.DeserializePublicUserData("{\"login\":\"login1\",\"password\":\"password1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "login174387458578348735745687r576845785467956985957895678956789567", "passwor12344444444234234444444444444444444444444444444444444442342342d1"}};
        rd_given = serializer.DeserializePublicUserData("{\"login\":\"login174387458578348735745687r576845785467956985957895678956789567\",\"password\":\"passwor12344444444234234444444444444444444444444444444444444442342342d1\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "", ""}};
        rd_given = serializer.DeserializePublicUserData("{\"login\":\"\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "123123123", ""}};
        rd_given = serializer.DeserializePublicUserData("{\"login\":\"123123123\",\"password\":\"\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);

        rd = {{"uuid", "login@@@``~~~==/.<>@#$!%^", "password@@@``~~~==/.<>@#$!%^"}};
        rd_given = serializer.DeserializePublicUserData("{\"login\":\"login@@@``~~~==/.<>@#$!%^\",\"password\":\"password@@@``~~~==/.<>@#$!%^\"}");
        CHECK(rd_given.has_value());
        CHECK(rd_given->login == rd.login);
        CHECK(rd_given->password == rd.password);
    }
}

TEST_CASE("SerializeTokenToUuid", "[jsonserializer]"){
    using StringMap = std::map<token_manager::Token, dm::Uuid>;
    
    JSONSerializer serializer;
    StringMap map;
    StringMap given;
    json j;

    SECTION("SerializeTokenToUuid"){
        map = {{"first", "second"}, {"third", "fourth"}};
        REQUIRE_NOTHROW(j = json::parse(serializer.SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);

        map = {{"", ""}};
        REQUIRE_NOTHROW(j = json::parse(serializer.SerializeMap(std::move(map))));
        REQUIRE_NOTHROW(given = j.template get<StringMap>());
        CHECK(map == given);
    }
}