#pragma once
#include "actor_id.hpp"

namespace game_manager{
    class IActor{
    public:
        bool operator ==(const IActor& other) const = default;

        ActorId actor_id;
    };
}