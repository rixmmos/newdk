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
    LoginThrottle.cpp
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
    # Phase 12: header-visibility for migrated packet headers moved out of
    # src/Core into shared/Packets/ -- see the matching comment in
    # server/gameserver/CMakeLists.txt for the full reasoning. Deliberately
    # not linking shared_packets_cl here (LoginServerPackets already does,
    # and PRIVATE linkage there means loginserver itself never saw the
    # include path even though it can have direct sources of its own).
    ${CMAKE_SOURCE_DIR}/../shared/Packets
)


if(APPLE)
    set(EXTRA_LIBS z)
elseif(UNIX)
    set(EXTRA_LIBS nsl z dl)
endif()

target_link_libraries(loginserver PRIVATE
    LoginServerPackets
    ServerCore
    Core
    LoginServerBilling
    LoginServerCBilling
    LoginServerDatabase
    ${MYSQL_LIBRARY}
    Threads::Threads
    ${EXTRA_LIBS}
)
