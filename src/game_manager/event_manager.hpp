#pragma once
#include <string>
#include <vector>
#include <list>
#include <optional>
#include <memory>
#include "event.hpp"
namespace game_manager{
    class EventListWrapper{
    public:
        using Ptr = std::shared_ptr<EventListWrapper>;
        using CPtr = std::shared_ptr<const EventListWrapper>;
        using OptCPtr = std::optional<CPtr>;
        using Vec = std::list<Event::Ptr>;
        const Vec& Value() const;
        
        void Clear();
        void AddEvent(Event::Ptr event);
        void AddEvents(Vec&& events);
        Vec FromMove(int move_number) const;
        void SetMoveNumber(int mn);
    private:
        int move_number_ = 0;

        std::vector<int> from_move_;
        Vec value_;
    };
}