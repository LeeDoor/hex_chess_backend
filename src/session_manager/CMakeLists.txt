#initializer's dependencies
set(LIBRARY_NAME session_manager)
#creating library
add_library(${LIBRARY_NAME}
    i_session_manager.hpp
    session_manager_postgres.hpp
    session_manager_postgres.cpp
    session_data.hpp
    postgres_session_conversion.hpp
)

#including and linking dependencies
target_link_libraries(${LIBRARY_NAME} 
    PUBLIC psql_connection_pool
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${DIRECTORIES})