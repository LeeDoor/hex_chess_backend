#include "static_handler.hpp"
#include "response_builder.hpp"
#include <iostream>

namespace http_handler {
    StaticHandler::StaticHandler(HandlerParameters handler_parameters){
        serializer_ = handler_parameters.serializer;
    }

    void StaticHandler::HandleFile(HttpRequest&& request, fs::path&& root, ResponseSender&& sender) {
        std::string path_str = static_cast<std::string>(request.target().substr(0, request.target().find('?')));
        fs::path path = root.concat(path_str);
        RequestNSender rns{request,sender};
        std::cout << path << std::endl;
        if(!IsSubdirectory(std::move(path), std::move(root))) {
            std::cout << "should be called when user writes root like 0.0.0.0:port/../../../forbidden_folder/passwords.txt"
           " but boost beast not allowing sockets send this kind of targets\n";
            return SendNoAccessError(rns);
        }
        if(!fs::exists(path) || fs::is_directory(path)) {
            return SendWrongPathError(rns);
        }
        return SendFile(std::move(path), rns);
    }

    void StaticHandler::SendFile(fs::path&& path, RequestNSender rns){
        ResponseBuilder<http::file_body> builder;
        rns.sender.file(std::move(builder.File(path, rns.request.method()).Status(status::ok).GetProduct()));
    }
    void StaticHandler::SendWrongPathError(RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("wrong_path", "file does not exists");
        rns.sender.string(std::move(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct()));
    }
    void StaticHandler::SendNoAccessError(RequestNSender rns){
        ResponseBuilder<http::string_body> builder;
        std::string body = serializer_->SerializeError("no_acess", "path is out of root");
        rns.sender.string(std::move(builder.BodyText(std::move(body), rns.request.method()).Status(status::bad_request).GetProduct()));
    }


    bool StaticHandler::IsSubdirectory(fs::path&& path, fs::path&& base) {
        path = fs::weakly_canonical(path);
        base = fs::weakly_canonical(base);
        for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
            if (p == path.end() || *p != *b) {
                return false;
            }
        }
        return true;
    }
} // http_handler