# ============================================================================
# SharedServer 
# ============================================================================

set(SHAREDSERVER_SOURCES
    Guild.cpp
    GuildManager.cpp
    ResurrectLocationManager.cpp
    GameServerPlayer.cpp
    HeartbeatManager.cpp
    GameServerInfoManager.cpp
    GameServerGroupInfoManager.cpp
    GameServerManager.cpp
    SharedServer.cpp
    main.cpp
    StringPool.cpp
)

add_executable(sharedserver ${SHAREDSERVER_SOURCES})


target_compile_definitions(sharedserver PRIVATE __SHARED_SERVER__)


target_include_directories(sharedserver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/server/database
)


if(APPLE)
    set(EXTRA_LIBS z)
elseif(UNIX)
    set(EXTRA_LIBS nsl z dl)
endif()

target_link_libraries(sharedserver PRIVATE
    SharedServerPackets
    ServerCore
    Core
    Database
    ${MYSQL_LIBRARY}
    Threads::Threads
    ${EXTRA_LIBS}
)
