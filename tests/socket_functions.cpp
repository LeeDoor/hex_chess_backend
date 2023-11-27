#include "socket_functions.hpp"
#include <boost/algorithm/string/classification.hpp> // for boost::is_any_of
#include <boost/algorithm/string/split.hpp> // boost::split

void ConnectSocket(net::io_context& ioc, tcp::socket& socket){
    tcp::resolver resolver(ioc); 
    auto const results = resolver.resolve("127.0.0.1", "9999");
    net::connect(socket, results.begin(), results.end());
    REQUIRE(socket.is_open());
}

void CheckStringResponse(const http::response<http::string_body>& response, 
                        bool is_head, 
                        http::status status, 
                        std::string&& body, 
                        std::string&& content_type, 
                        std::vector<std::string>&& allow_expected){
    CHECK(response.result() == status);
    if(!body.empty()){
        if(is_head) {
            CHECK(response.body() == "");
        }
        else {
            CHECK(response.body() == body);
        }
    }

    auto content_length_iter = response.find(http::field::content_length);
    if(content_length_iter == response.end()){
        FAIL_CHECK("no content_length header");
    }
    else if (!body.empty()){
        int content_length = std::stoi(content_length_iter->value().to_string());
        CHECK(content_length == body.size());
    }

    auto content_type_iter = response.find(http::field::content_type);
    if(content_type_iter == response.end()){
        FAIL_CHECK("no content_type header");
    }
    else {
        CHECK(content_type_iter->value().to_string() == content_type);
    }

    if(status == http::status::method_not_allowed){
        auto allow_iter = response.find(http::field::allow);
        if(allow_iter == response.end()) {
            FAIL_CHECK("no allow header");
        }
        else{
            std::string allow = allow_iter->value().to_string();
            std::transform(allow.begin(), allow.end(), allow.begin(),
                [](unsigned char c){ return std::tolower(c); });
            std::vector<std::string> methods;
            boost::split(methods, allow, boost::is_any_of(", "), boost::token_compress_on);
            std::sort(methods.begin(), methods.end());
            std::sort(allow_expected.begin(), allow_expected.end());
            INFO("|"<<allow<<"|");
            CHECK(allow_expected == methods);
        }
    }
}

http::response<http::string_body> GetResponseToRequest(bool is_head, http::request<http::string_body>& request, tcp::socket& socket) {
    REQUIRE_NOTHROW(http::write(socket, request));
    beast::flat_buffer buffer;
    http::response<http::string_body> response;
    if(!is_head){
        REQUIRE_NOTHROW(http::read(socket, buffer, response));
        return response;
    }
    http::response_parser<http::string_body> parser;
    REQUIRE_NOTHROW(http::read_header(socket, buffer, parser));
    response = parser.get();
    return response;
}