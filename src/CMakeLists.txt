#list with all libraries
set (DEPENDENCIES 
    initializer 
    server 
    http_handler 
    user_manager 
    token_manager 
    queue_manager 
    game_manager 
    serializer
    notification_system
    session_manager
    psql_connection_pool
)
#add subdirectories
foreach (dir IN ITEMS ${DEPENDENCIES})
    add_subdirectory(${dir})
endforeach ()

#create exe file
add_executable(${PROJECT_NAME} main.cpp)

#include folders for headers
target_include_directories(${PROJECT_NAME} PRIVATE ${DEPENDENCIES})

#link conan libraries
target_link_libraries(${PROJECT_NAME} ${DEPENDENCIES} ${CONAN_LIBS}) 