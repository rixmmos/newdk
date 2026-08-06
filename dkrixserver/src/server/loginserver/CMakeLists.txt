# ============================================================================
# LoginServer 
# ============================================================================

set(LOGINSERVER_SOURCES
    ClientManager.cpp
    UserInfoManager.cpp
    GameServerGroupInfoManager.cpp
    GameServerManager.cpp
    LoginPlayer.cpp
    LoginPlayerManager.cpp
    LoginServer.cpp
    ZoneGroupInfoManager.cpp
    ZoneInfoManager.cpp
    main.cpp
    ItemDestroyer.cpp
    ReconnectLoginInfoManager.cpp
)

add_executable(loginserver ${LOGINSERVER_SOURCES})


target_compile_definitions(loginserver PRIVATE __LOGIN_SERVER__)


target_include_directories(loginserver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/server/database
    ${CMAKE_SOURCE_DIR}/src/server/gameserver/billing
)


if(APPLE)
    set(EXTRA_LIBS z)
elseif(UNIX)
    set(EXTRA_LIBS nsl z dl)
endif()

target_link_libraries(loginserver PRIVATE
    LoginServerDatabase
    LoginServerPackets
    ServerCore
    Core
    LoginServerBilling
    LoginServerCBilling
    ${MYSQL_LIBRARY}
    Threads::Threads
    ${EXTRA_LIBS}
)
