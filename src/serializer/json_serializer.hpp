#pragma once
#include "i_serializer.hpp"

namespace serializer{
    class JSONSerializer : public ISerializer{
    public:
        std::string SerializeEmpty() override;
        std::string SerializeError(std::string&& error_name, std::string&& desc) override;
        std::string SerializeMap(std::map<std::string, std::string>&& map) override;
    };
}