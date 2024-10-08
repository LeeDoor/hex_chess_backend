#list with all libraries
set (DEPENDENCIES serializer)

add_executable(test_application 
    api_functions.cpp
    api_functions.hpp
    socket_functions.hpp 
    socket_functions.cpp 
    test_set_url_parameters.cpp

    test_jsonserializer.cpp
    test_api_register.cpp
    test_api_login.cpp
    test_api_profile.cpp

    test_api_game_enqueue.cpp
    test_api_game_wait_for_opponent.cpp
    test_api_game_session_state.cpp
    test_api_game_session_state_change.cpp
    test_api_game_walk.cpp
    test_api_game_resign.cpp
    test_api_game_place_bomb.cpp
    test_api_game_place_gun.cpp
    
    test_api_player_tokens.cpp
    test_api_user.cpp
    test_api_mm_queue.cpp
    test_api_set_state.cpp
)


#including and linking dependencies
target_link_libraries(test_application ${DEPENDENCIES}
    game_manager

    Catch2::Catch2WithMain
    nlohmann_json::nlohmann_json 
    spdlog::spdlog
)
target_include_directories(test_application PRIVATE ${DIRECTORIES})