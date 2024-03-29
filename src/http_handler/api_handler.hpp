#pragma once
#include "api_function_builder.hpp"
#include "i_serializer.hpp"
#include "response_builder.hpp"
#include "send_manager.hpp"
#include "handler_parameters.hpp"
#include <map>

namespace http_handler {
    class ApiHandler :  public std::enable_shared_from_this<ApiHandler> {
    public:
        ApiHandler(HandlerParameters handler_parameters);
        void Init();
        void HandleApiFunction(HttpRequest&& request, ResponseSender&& sender);

    private:
        void ApiFunctionsParse();
        bool ApiFunctionParse(std::map<std::string, ExecutorFunction>& executors, std::ifstream& is, ApiFunctionBuilder& afb);
        void ApiRegister(RequestNSender rns);
        void ApiLogin(RequestNSender rns);
        void ApiGetProfileData(RequestNSender rns);
        void ApiEnqueue(RequestNSender rns);

        tokenm::Token SenderAuthentication(const HttpRequest& request);

        std::map<std::string, ApiFunctionExecutor> request_to_executor_;
        serializer::ISerializer::Ptr serializer_;
        SendManager responser_;
        database_manager::IUserDataManager::Ptr uds_;
        token_manager::TokenToUuid::Ptr ttu_;
        matchmaking_system::MMQueue::Ptr mm_queue_;
    };

} // http_handler

