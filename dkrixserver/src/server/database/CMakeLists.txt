# ============================================================================
# Database  - 
# ============================================================================

set(DATABASE_SOURCES
    Connection.cpp
    Statement.cpp
    PreparedStatement.cpp
    Result.cpp
    DatabaseManager.cpp
)

# ============================================================================
#  Database 
# ============================================================================
add_library(Database STATIC ${DATABASE_SOURCES})
target_include_directories(Database PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Database PUBLIC ServerCore)

# ============================================================================
# GameServer  Database
#  OBJECT
# ============================================================================
add_library(GameServerDatabase STATIC ${DATABASE_SOURCES})
target_compile_definitions(GameServerDatabase PRIVATE __GAME_SERVER__)
target_include_directories(GameServerDatabase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(GameServerDatabase PUBLIC ServerCore)

# ============================================================================
# LoginServer  Database
# ============================================================================
add_library(LoginServerDatabase STATIC ${DATABASE_SOURCES})
target_compile_definitions(LoginServerDatabase PRIVATE __LOGIN_SERVER__)
target_include_directories(LoginServerDatabase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(LoginServerDatabase PUBLIC ServerCore)

# ============================================================================
# SharedServer  Database
# ============================================================================
add_library(SharedServerDatabase STATIC ${DATABASE_SOURCES})
target_compile_definitions(SharedServerDatabase PRIVATE __SHARED_SERVER__)
target_include_directories(SharedServerDatabase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SharedServerDatabase PUBLIC ServerCore)

# ============================================================================
# UpdateServer  Database
# ============================================================================
add_library(UpdateServerDatabase STATIC ${DATABASE_SOURCES})
target_compile_definitions(UpdateServerDatabase PRIVATE __UPDATE_SERVER__)
target_include_directories(UpdateServerDatabase PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(UpdateServerDatabase PUBLIC ServerCore)
