#pragma once
#include <string>
#include <optional>
#include "session.hpp"
#include "move_data.hpp"
#include "event_manager.hpp"

namespace serializer{
    std::string Serialize(const gm::State& state);
    std::string Serialize(const gm::MoveData& md);
    std::string Serialize(const gm::EventListWrapper::Vec& elwv);
    std::string SerializeGameConst();

    std::optional<gm::State> DeserializeSessionState(const std::string& json);
    std::optional<int> DeserializeFromMove(const std::string& json);

    std::optional<gm::MoveData> DeserializeMoveData(const std::string& json);
}