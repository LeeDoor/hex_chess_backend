#pragma once
#include "api_handler.hpp"

namespace http_handler{
    class DebugHandler : public ApiHandler, public std::enable_shared_from_this<DebugHandler> {
    public:
        DebugHandler(HandlerParameters handler_parameters);
        void Init() override;
    private:
        // generate api function set from file
        void ApiFunctionsParse();

        void ApiGetPlayerTokens(SessionData&& rns, const RequestData& rd);
        void ApiGetUser(SessionData&& rns, const RequestData& rd);
        void ApiGetMMQueue(SessionData&& rns, const RequestData& rd);
        void ApiSetState(SessionData&& rns, const RequestData& rd);

        user_manager::IUserManager::Ptr dm_;
        token_manager::ITokenManager::Ptr tm_;
        game_manager::IQueueManager::Ptr qm_;
        game_manager::GameManager::Ptr gm_;
    };
}


