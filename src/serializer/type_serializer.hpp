#include <nlohmann/json.hpp>
#include "registration_data.hpp"
#include "public_user.hpp"
#include "session.hpp"
#include "session_data.hpp"
#include "object.hpp"
using json = nlohmann::json;

namespace http_handler {
    void to_json(json& j, const RegistrationData& v);
    void from_json(const json& j, RegistrationData& v);
    
    void to_json(json& j, const PublicUser& v);
    void from_json(const json& j, PublicUser& v);
} 

namespace user_manager {
    void to_json(json& j, const User& v);
    void from_json(const json& j, User& v);
}

namespace game_manager{
    void to_json(json& j, const State& v);
    void from_json(const json& j, State& v);

    void to_json(json& j, const Player& v);
    void from_json(const json& j, Player& v);

    void to_json(json& j, const Obstacle& v);
    void from_json(const json& j, Obstacle& v);

    void to_json(json& j, const MapSize& v);
    void from_json(const json& j, MapSize& v);

    void to_json(json& j, const Object::Ptr& v);
    void from_json(const json& j, Object::Ptr& v);

    void to_json(json& j, const Event& v);
    void from_json(const json& j, Event& v);

    void to_json(json& j, const Session::PlaceData& v);
    void from_json(const json& j, Session::PlaceData& v);
    
    NLOHMANN_JSON_SERIALIZE_ENUM( Obstacle::Type, {
        {Obstacle::Type::Wall, "wall"},
    })

    NLOHMANN_JSON_SERIALIZE_ENUM( Session::MoveType, {
        {Session::MoveType::Walk, "walk"},
        {Session::MoveType::Resign, "resign"},
        {Session::MoveType::PlaceBomb, "place_bomb"},
    })
}

namespace session_manager{
    void to_json(json& j, const PublicSessionData& v);
    void from_json(const json& j, PublicSessionData& v);
}