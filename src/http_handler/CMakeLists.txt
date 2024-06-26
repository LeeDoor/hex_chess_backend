#initializer's dependencies
set(DEPENDENCIES 
    serializer 
    database_manager
    token_manager 
    matchmaking_system
    game_manager
)
set(LIBRARY_NAME http_handler)
#creating library
add_library(${LIBRARY_NAME}
    http_types.hpp
    http_handler.hpp
    http_handler.cpp
    api_handler.cpp
    api_handler.hpp
    static_handler.cpp
    static_handler.hpp
    api_function_executor.cpp
    api_function_executor.hpp
    api_function_builder.cpp
    api_function_builder.hpp
    api_function.cpp
    api_function.hpp
    response_builder.hpp
    handler_parameters.hpp
    send_manager.hpp
    send_manager.cpp
    public_user_data.hpp
    registration_data.hpp
    registration_data.cpp
    get_token_from_header.hpp
    get_token_from_header.cpp
    api_function_params.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} PUBLIC ${DEPENDENCIES})
foreach (dir IN ITEMS ${DEPENDENCIES})
    target_include_directories(${LIBRARY_NAME} PUBLIC "../${dir}")
endforeach ()