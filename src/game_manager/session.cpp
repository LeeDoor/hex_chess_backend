#include "session.hpp"
#include "bomb.hpp"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <cmath>

#define BIND(func, place, pls, ...) (Bomb::ExplodeFunc)std::bind( func, this->weak_from_this(), pls) 
namespace game_manager{

    Session::Session(um::Uuid player1, const um::Login& login1, um::Uuid player2, const um::Login& login2) 
        :player1_(std::move(player1)), 
        player2_(std::move(player2)), 
        uuid_to_login_{{player1_, login1},{player2_, login2}},
        state_(std::make_shared<State>()){
        InitSessionState(login1, login2);
    }

    bool Session::HasPlayer(const um::Uuid& uuid){
        return player1_ == uuid || player2_ == uuid;
    }

    State::CPtr Session::GetState(){
        return state_;
    }
    std::optional<Session::Results> Session::GetResults() {
        return results_;
    }

    Session::GameApiStatus Session::ApiResign(const um::Uuid& player_id) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        FinishSession(player_id != player1_);
        AddEvent(player_id == player1_ ? player1().id : player2().id, PLAYER_RESIGN);
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiWalk(const um::Uuid& player_id, const PlaceData& move_data){
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1().login == uuid_to_login_.at(player_id)?
            player1() : player2();

        if (player.login != nowTurn())
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.posX - move_data.posX))
          + std::abs(short(player.posY - move_data.posY)) != 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!ValidCell(move_data.posX, move_data.posY))
            return GameApiStatus::WrongMove;
        
        player.posX = move_data.posX;
        player.posY = move_data.posY;
        AddEvent(player.id, PLAYER_WALK);
        AfterMove();
        return GameApiStatus::Ok;
    }
    Session::GameApiStatus Session::ApiPlaceBomb(const um::Uuid& player_id, const PlaceData& move_data) {
        std::lock_guard<std::mutex> locker(move_mutex_);
        Player& player = player1().login == uuid_to_login_.at(player_id)?
            player1() : player2();

        if (player.login != nowTurn())
            return GameApiStatus::NotYourMove;

        if (std::abs(short(player.posX - move_data.posX)) > 1 ||
            std::abs(short(player.posY - move_data.posY)) > 1){
            return GameApiStatus::WrongMove;
        } // cell is near the player
        if (!ValidCell(move_data.posX, move_data.posY))
            return GameApiStatus::WrongMove;
        
        PlaceBombObject(move_data, player.login);
        AddEvent(player.id, PLAYER_PLACE_BOMB);
        AfterMove();
        return GameApiStatus::Ok;
    }
    
    void Session::FinishSession(bool firstWinner) {
        results_ = firstWinner ? 
              Results{.winner=player1_, .loser=player2_} 
            : Results{.winner=player2_, .loser=player1_};
    }

    void Session::InitSessionState(const um::Login& login1, const um::Login& login2){
        state_->players.resize(2);
        player1().login = login1;
        player2().login = login2;

        player1().posX = 4;
        player1().posY = 1;

        player2().posX = 3;
        player2().posY = 6;

        player1().id = GetId();
        player2().id = GetId();
        
        std::vector<std::pair<unsigned, unsigned>> walls = {
            {0,2}, {3,2}, 
            {4,3}, {6,3},
            {1,4}, {3,4},
            {4,5}, {7,5}
        };
        terrain().reserve(walls.size());
        for(auto& pair : walls){
            terrain().emplace_back(pair.first, pair.second, Obstacle::Type::Wall);
        }

        nowTurn() = login1;
        state_->map_size = {15,15};    
    }

    void Session::AfterMove(){
        for(auto it = objects().begin(); it != objects().end(); ++it){
            auto tick_res = (*it)->UpdateTick();
            AddEvent((*it)->id, std::move(tick_res.first));
            if(tick_res.second){
                Object::Ptr obj = *it;
                it--;
                RemoveObject(obj);
            }
        }
        nowTurn() = nowTurn() == player1().login? player2().login : player1().login;
    }

    void Session::AddEvent(ActorId id, std::string event_type) {
        state_->events.emplace_back(id, std::move(event_type));
    }

    bool Session::ValidCell(unsigned posX, unsigned posY){
        if (state_->map_size.height <= posY || state_->map_size.width <= posX){
            spdlog::warn("cell is invalid: out of map size [{},{}]. map is {}x{}", posX, posY, state_->map_size.width,state_->map_size.height);
            return false;
        }

        auto it = std::find_if(terrain().begin(), terrain().end(), 
            [&](const Obstacle& v){return v.posX == posX && v.posY == posY;});
        if(it != terrain().end()){
            spdlog::warn("cell is invalid: on the obstacle [{},{}]", posX, posY);
            return false;
        }
        
        if(player1().posX == posX && player1().posY == posY ||
           player2().posX == posX && player2().posY == posY){
            spdlog::warn("cell is invalid: on the other player [{},{}]", posX, posY);
            return false;
        }

        return true;
    }

    void Session::PlaceBombObject(PlaceData place, Player::Login login) {
        namespace pl = std::placeholders;
        auto sp = this->shared_from_this();
        Bomb::Ptr obj = std::make_shared<Bomb>(login, GetId(), (Bomb::ExplodeFunc)std::bind(&Session::Explode, sp, pl::_1, pl::_2));
        obj->Place(place.posX, place.posY);
        objects().emplace_back(obj);
    }
    void Session::RemoveObject(Object::Ptr obj) {
        objects().erase(std::find_if(objects().begin(), objects().end(), [&](Object::Ptr obj2){return *obj == obj2;}));
    }

    void Session::Explode(Dimention posX, Dimention posY) {
        state_->terrain.emplace_back(posX, posY, Obstacle::Type::Wall);
    }
}