#include "type_serializer.hpp"
#include "spdlog/spdlog.h"
#include "bomb.hpp"
#include "gun.hpp"
#include "bullet.hpp"

namespace game_manager{
    void to_json(json& j, const State& v) {
        j = json{
            {"state", "playing"}, 
            {"players", v.players}, 
            {"objects", v.objects}, 
            {"terrain", v.terrain}, 
            {"now_turn", v.now_turn},
            {"map_size", v.map_size},
            {"move_number", v.move_number},
        };
    }
    void from_json(const json& j, State& v) {
        j.at("players").get_to(v.players);
        j.at("objects").get_to(v.objects);
        j.at("terrain").get_to(v.terrain);
        j.at("now_turn").get_to(v.now_turn);
        j.at("map_size").get_to(v.map_size);
        j.at("move_number").get_to(v.move_number);
    }
    
    void to_json(json& j, const Object::Ptr& v) {
        v->tojson(j);
    }
    void from_json(const json& it, Object::Ptr& v) {
        Object::OwnerType owner;
        ActorId actor_id;
        owner = it.at("owner").get<Object::OwnerType>();
        actor_id = it.at("actor_id").get<ActorId>();
        if(it.at("type") == "bomb"){
            Bomb::Ptr bomb = std::make_shared<Bomb>(owner, actor_id);
            it.at("ticks_left").get_to(bomb->ticks_left);
            bomb->Place(it.at("posX").get<Dimention>(), it.at("posY").get<Dimention>());
            v = bomb;
            return;
        }
        if(it.at("type" == "gun")){
            Gun::Ptr gun = std::make_shared<Gun>(owner, actor_id);
            it.at("ticks_to_shot").get_to(gun->ticks_to_shot);
            it.at("shots_left").get_to(gun->shots_left);
            gun->Place(it.at("posX").get<Dimention>(), it.at("posY").get<Dimention>(), it.at("dir").get<Direction>());
            v = gun;
            return;
        }
        if(it.at("type" == "bullet")){
            Bullet::Ptr bullet = std::make_shared<Bullet>(owner, actor_id);
            bullet->Place(it.at("posX").get<Dimention>(), it.at("posY").get<Dimention>(), it.at("dir").get<Direction>());
            v = bullet;
            return;
        }
        else{
            throw std::runtime_error("object type not implemented: " + it.at("type").get<std::string>());
        } 
    }
    void to_json(json& j, const Event& v) {
        j["move_number"] = v.move_number;
        j["actor_id"] = v.actor_id;
        j["event_type"] = v.event_type;
        j["data"] = v.data;
    }
    void to_json(json& j, const VariantEventData& v) {
        if(std::holds_alternative<WalkData>(v)){
            j = std::get<WalkData>(v);
        }
        if(std::holds_alternative<BombData>(v)){
            j = std::get<BombData>(v);
        }
    }
}

namespace session_manager{
    void to_json(json& j, const PublicSessionData& v) {
        j = json{
            {"state", "finished"}, 
            {"id", v.id}, 
            {"winner", v.winner?*v.winner:"null"}, 
            {"loser", v.loser?*v.loser:"null"}
        };
    }
    void from_json(const json& j, PublicSessionData& v) {
        j.at("id").get_to(v.id);
        um::Login login;
        j.at("winner").get_to(login);
        if(login == "null")
            v.winner = std::nullopt;
        else
            v.winner = login;
        j.at("loser").get_to(login);
        if(login == "null")
            v.loser = std::nullopt;
        else
            v.loser = login;
    }
}