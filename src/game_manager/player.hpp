#pragma once
#include <memory>
#include "user.hpp"
#include "i_placeable.hpp"
#include "event_manager.hpp"
#include "custom_events.hpp"

namespace game_manager{
    class Player : public IPlaceable{
    public:
        using Ptr = std::shared_ptr<Player>;
        using Id = um::Login;

        Player();
        Player(Position position, ActorId actor_id, Id login);
        bool operator == (const Player& other) const;

        EventListWrapper::Vec Die();
        EventsType InteractWithBullet(std::shared_ptr<Bullet> bullet);

        Id login;
    private:
        const std::string PLAYER_DEAD = "player_dead";
    };
}