#pragma once
#include "http_types.hpp"
#include <functional>
namespace http_handler {
    typedef std::function<void(RequestNSender)> ExecutorFunction;
    typedef std::vector<http::verb> AllowedMethods;


    class ApiFunction {
    public:
        ApiFunction(ExecutorFunction&& executor_function, AllowedMethods&& allowed_methods);

        const AllowedMethods& GetAllowedMethods() const;
        void operator()(RequestNSender rns);

    private:
        ExecutorFunction executor_function_;
        AllowedMethods allowed_methods_;
    };

} // http_handler

