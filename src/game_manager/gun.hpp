#pragma once
#include "object_directed.hpp"
#include <functional>

namespace game_manager{
    class Gun : public ObjectDirected {
    public: 
        using Ptr = std::shared_ptr<Gun>;
        using DestroyFunc = std::function<void(ActorId)>;
        using ShootFunc = std::function<void(Direction)>;

        struct Methods{
            DestroyFunc destroy;
            ShootFunc shoot;
        };

        Gun(OwnerType owner, ActorId actor_id);
        Gun(OwnerType owner, ActorId actor_id, Methods&& methods);
        bool operator==(Object::Ptr obj) const override;
        virtual void tojson(nlohmann::json& j) const;

        std::string UpdateTick() override;
    
        unsigned ticks_to_shot = shot_cooldown_;
        unsigned shots_left = shot_amount_;
    private:
        Methods methods_; 

        const unsigned shot_cooldown_ = 3;
        const unsigned shot_amount_ = 3;

        const std::string GUN_WAITING = "gun_waiting";
        const std::string GUN_SHOT = "gun_shot";
        const std::string GUN_SHOT_DESTROY = "gun_shot_destroy";
    };
}