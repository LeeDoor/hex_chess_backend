#initializer's dependencies
set(DEPENDENCIES user_manager session_manager notification_system)
set(LIBRARY_NAME game_manager)
#creating library
add_library(${LIBRARY_NAME}
    game_manager.cpp
    game_manager.hpp
    session.hpp
    session.cpp
    session_id_generator.hpp
    matchmaking_balancer.hpp
    matchmaking_balancer.cpp
    state.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
    PUBLIC user_manager
    PUBLIC session_manager
    PUBLIC http_handler

    PRIVATE notification_system
)
target_include_directories(${LIBRARY_NAME} 
    PUBLIC "../user_manager"
    PUBLIC "../session_manager"
    PUBLIC "../http_handler"

    PRIVATE "../notification_system"
)