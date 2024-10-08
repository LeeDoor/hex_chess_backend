#include "listener.hpp"
#include "http_server.hpp"
#include "spdlog/spdlog.h"

namespace http_server {
    std::shared_ptr<Session> Session::GetSharedThis() {
        return shared_from_this();
    }

    void Session::Run() {
        net::dispatch(stream_->get_executor(), beast::bind_front_handler(&Session::Read, GetSharedThis()));
    }

    void Session::Read() {
        request_ = {};
        //stream_->expires_after(expiry_time_);
        http::async_read(*stream_, buffer_, request_,
            beast::bind_front_handler(&Session::OnRead, GetSharedThis()));
    }

    void Session::OnRead(beast::error_code ec, std::size_t byets_read) {
        if(ec == http::error::end_of_stream) {
            return Close();
        }
        if(ec) {
            return ReportError(ec, "Read");
        }
        HandleRequest(std::move(request_));
    }

    void Session::HandleRequest(HttpRequest&& request) {
        spdlog::info("handling request {}", static_cast<std::string>(request.target()));
        auto fileHandler = [self = this->shared_from_this()](FileResponse &&response) {
            self->Write(std::move(response));
        };
        auto stringHandler = [self = this->shared_from_this()](StringResponse &&response) {
            self->Write(std::move(response));
        };
        request_handler_(std::move(request), {
            stringHandler,
            fileHandler,
        }); 
    }

    void Session::OnWrite(bool close, beast::error_code ec, std::size_t bytes_written) {
        if(ec) {
            return ReportError(ec, "write");
        }
        if(close) {
            return Close();
        }
        Read();
    }
    void Session::Close() {
        stream_->socket().close();
    }
}