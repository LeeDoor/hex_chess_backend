#include "socket_functions.hpp"
#include "api_functions.hpp"
using namespace serializer;
#include "json_serializer.hpp"

TEST_CASE("ApiUserData", "[api][debug][user_data]") {
	net::io_context ioc;
    tcp::socket socket{ioc};
    ConnectSocket(ioc, socket);
    std::shared_ptr<JSONSerializer> serializer = std::make_shared<JSONSerializer>();
    std::string UNAUTHORIZED = serializer->SerializeError("invalid_admin", "the administrator password is missing or incorrect");
    std::string NO_SUCH_USER_UUID = serializer->SerializeError("user_not_found", "no user with provided uuid found");
    std::string NO_SUCH_USER_LP = serializer->SerializeError("user_not_found", "no user with provided login and password found");
    std::string URL_PARAMETERS_ERROR = serializer->SerializeError("url_parameters_error", "this api function requires url parameters");
    
    SECTION("server returns valid user data by uuid"){
    	hh::RegistrationData rd = RegisterSuccess(socket, serializer);
    	LoginData ld = LoginSuccess(socket, rd.login, serializer);
    	auto map = PlayerTokensSuccess(socket, serializer);
    	REQUIRE(map.contains(ld.token)); // player_tokens error
		dm::UserData ud = UserDataSuccess(socket, serializer, map[ld.token]);
		CHECK(ud.login == rd.login);    	
		CHECK(ud.password == rd.password);    	
    }

    SECTION("server returns valid user data by login and password"){
    	hh::RegistrationData rd = RegisterSuccess(socket, serializer); 
    	LoginData ld = LoginSuccess(socket, rd.login, serializer);
    	auto map = PlayerTokensSuccess(socket, serializer);
    	REQUIRE(map.contains(ld.token)); // player_tokens error
		dm::UserData ud = UserDataSuccess(socket, serializer, rd.login, rd.password);
		CHECK(ud.uuid == map[ld.token]);    	
    }

    SECTION("unexisting_uuid"){
    	StringResponse response = UserData(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, "aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
    	CheckStringResponse(response, {
	    	.body=NO_SUCH_USER_UUID,
	    	.res=http::status::not_found,
	    });
    }

    SECTION("empty_uuid"){
		StringResponse response = UserData(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, "");
    	CheckStringResponse(response, {
	    	.body=NO_SUCH_USER_UUID,
	    	.res=http::status::not_found,
	    });
    }

    SECTION("unexisting_login"){
		StringResponse response = UserData(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, INVALID_LOGIN, VALID_PASS);
    	CheckStringResponse(response, {
	    	.body=NO_SUCH_USER_LP,
	    	.res=http::status::not_found,
	    });
    }

    SECTION("login_empty"){
    	StringResponse response = UserData(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, "", INVALID_PASS);
    	CheckStringResponse(response, {
	    	.body=NO_SUCH_USER_LP,
	    	.res=http::status::not_found,
	    });
    }

    SECTION("wrong_password_correct_login"){
    	hh::RegistrationData rd = RegisterSuccess(socket, serializer);
    	StringResponse response = UserData(socket, serializer, ADMIN_LOGIN, ADMIN_PASSWORD, rd.login, INVALID_PASS);
    	CheckStringResponse(response, {
	    	.body=NO_SUCH_USER_LP,
	    	.res=http::status::not_found,
	    });
    }

    SECTION("url_parameters"){
    	std::string target;
    	// all sections below are setting wrong url to target 
    	SECTION("extra_ampersand"){
			target = "/api/debug/user_data?uuid=somekindofuuid&";
    	}
    	SECTION("extra_parameter_uuid"){
			target = "/api/debug/user_data?uuid=somekindofuuid&extraparam=extravalue";
    	}
    	SECTION("extra_parameter_login_password"){
			target = "/api/debug/user_data?login=somekindoflogin&password=somekindofpassword&extraparam=extravalue";
    	}
    	SECTION("uuid_with_login"){
			target = "/api/debug/user_data?uuid=somekindofuuid&login=somekindoflogin";
    	}
    	SECTION("uuid_with_password"){
			target = "/api/debug/user_data?uuid=somekindofuuid&password=somekindofpassword";
    	}
    	SECTION("uuid_with_login_and_password"){
			target = "/api/debug/user_data?uuid=somekindofuuid&login=somekindoflogin&password=somekindofpassword";
    	}
    	SECTION("login_without_password"){
			target = "/api/debug/user_data?login=somekindoflogin";
    	}
    	SECTION("password_without_login"){
			target = "/api/debug/user_data?password=somekindofpassword";
    	}
    	INFO(target);
	    http::request<http::string_body> req{http::verb::get, target, 11};
	    req.body() = serializer->SerializeRegData({ADMIN_LOGIN, ADMIN_PASSWORD});
	    req.prepare_payload();
	    StringResponse response = GetResponseToRequest(false, req, socket);
	    CheckStringResponse(response, {
	    	.body = URL_PARAMETERS_ERROR,
	    	.res = http::status::unprocessable_entity,
	    });
    }


    SECTION ("admin verification tests"){ 
		hh::RegistrationData rd = RegisterSuccess(socket, serializer);
	    SECTION ("server returns error when body is mess"){
	    	http::request<http::string_body> req{http::verb::get, SetUrlParameters(USER_DATA_API, {{"login", rd.login}, {"password", rd.password}}), 11};

		    req.body() = "wrong admin credentials";
		    req.prepare_payload();

		    StringResponse response = GetResponseToRequest(false, req, socket);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong headers"){
	    	http::request<http::string_body> req{http::verb::get, SetUrlParameters(USER_DATA_API, {{"login", rd.login}, {"password", rd.password}}), 11};

		    req.body() = "{\"header1\":\"value1\",\"header2\",\"value2\"}";
		    req.prepare_payload();

		    StringResponse response = GetResponseToRequest(false, req, socket);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong credentials"){
	    	StringResponse response = UserData(socket, serializer, "wrong login", "wrong password", rd.login, rd.password);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong password"){
	    	StringResponse response = UserData(socket, serializer, "leedoor", "wrong password", rd.login, rd.password);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is json, but wrong login"){
	    	StringResponse response = UserData(socket, serializer, "wrong login", "123qwe123", rd.login, rd.password);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	    SECTION ("server returns error when body is empty json"){
	    	http::request<http::string_body> req{http::verb::get, SetUrlParameters(USER_DATA_API, {{"login", rd.login}, {"password", rd.password}}), 11};;

		    req.body() = "{}";
		    req.prepare_payload();

		    StringResponse response = GetResponseToRequest(false, req, socket);
		    CheckStringResponse(response, {
		    	.body=UNAUTHORIZED,
		    	.res=http::status::unauthorized,
		    });
		}
	}
}