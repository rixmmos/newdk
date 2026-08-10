cmake_minimum_required(VERSION 3.16)
project(DarkEden VERSION 1.0 LANGUAGES CXX)

# ============================================================================

# ============================================================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ============================================================================

# ============================================================================
#  - 
if(APPLE)
    add_compile_definitions(__APPLE__ _REENTRANT)
elseif(UNIX)
    add_compile_definitions(__LINUX__ _REENTRANT)
endif()


set(WARNING_FLAGS 
    -Wall
    -Wno-format 
    -Wno-format-security
)
add_compile_options(${WARNING_FLAGS})

# Debug
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# ============================================================================
# Sanitizer configuration (Phase 10 bullet 3, 2026-08-07)
# ============================================================================
# Mirrors the client block at dkrix/CMakeLists.txt:37-65 so both trees
# expose the same -DUSE_<SAN>=ON switches. Wired here (rather than in
# per-target properties) because the server build is effectively
# monolithic — a single CMAKE_CXX/C_FLAGS fold-in covers all three
# executables (gameserver/loginserver/sharedserver) and Core.
option(USE_ASAN "Enable AddressSanitizer for memory error detection" OFF)
option(USE_TSAN "Enable ThreadSanitizer for race condition detection" OFF)
option(USE_UBSAN "Enable UndefinedBehaviorSanitizer for undefined behavior detection" OFF)

# ASan and TSan instrument memory access differently (shadow memory vs.
# interceptor-based happens-before tracking) and cannot be linked into
# the same binary. UBSan is a separate, compatible check and may be
# combined with either.
if(USE_ASAN AND USE_TSAN)
    message(FATAL_ERROR "USE_ASAN and USE_TSAN are mutually exclusive — enable only one at a time (UBSAN may combine with either).")
endif()

if(USE_ASAN OR USE_TSAN OR USE_UBSAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(SANITIZER_FLAGS "")

        if(USE_ASAN)
            set(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
        endif()

        if(USE_TSAN)
            set(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=thread")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
        endif()

        # `vptr` is deliberately excluded from the `undefined` group — see
        # docs/ci-server-ubsan.md for the full diagnosis. Short version:
        # -fsanitize=vptr emits a reference to `typeinfo for T` at every
        # member access through a polymorphic reference. Three such sites in
        # sharedserver (Guild.cpp, GuildManager.cpp x2) name GC* packet
        # classes whose key function — and therefore whose vtable and
        # typeinfo — lives only in the GameServerPackets library, which
        # sharedserver deliberately does not link. The link fails on exactly
        # those three symbols and nothing else.
        #
        # Disabling the check is preferred over linking GameServerPackets
        # into sharedserver: that would drag the whole gameserver packet set
        # across a real layering boundary purely to satisfy a sanitizer, for
        # no runtime benefit. vptr is also the least valuable member of the
        # group in this tree (bad-downcast detection, in code that uses
        # hand-rolled packet factories rather than dynamic_cast), while the
        # checks that actually pay off on 2000s-era C++ — signed overflow,
        # shift, null deref, alignment, bounds — all stay on.
        #
        # ORDER IS LOAD-BEARING: GCC applies these left to right, so
        # -fno-sanitize=vptr must come AFTER -fsanitize=undefined or it is
        # silently re-enabled. For the same reason this cannot be injected
        # from the Makefile via -DCMAKE_CXX_FLAGS (CMake places the cache
        # value before the appended SANITIZER_FLAGS) nor via
        # -DCMAKE_CXX_FLAGS_DEBUG (line 35 above is a plain `set`, which
        # overrides the cache). It has to be here.
        if(USE_UBSAN)
            set(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined -fno-sanitize=vptr")
        endif()

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")

        message(STATUS "Sanitizer flags: ${SANITIZER_FLAGS}")
    else()
        message(WARNING "Sanitizers requested but compiler is not GCC/Clang — flags not applied")
    endif()
endif()

# ============================================================================

# ============================================================================
find_package(Threads REQUIRED)

# MySQL
if(APPLE)
    find_path(MYSQL_INCLUDE_DIR mysql/mysql.h
        PATHS
        /opt/homebrew/include
        /opt/homebrew/Cellar/mysql-client/*/include
        /opt/homebrew/Cellar/mariadb-connector-c/*/include
        /usr/local/include
        NO_DEFAULT_PATH
    )
    find_library(MYSQL_LIBRARY mysqlclient
        PATHS
        /opt/homebrew/lib
        /opt/homebrew/Cellar/mysql-client/*/lib
        /opt/homebrew/Cellar/mariadb-connector-c/*/lib
        /opt/homebrew/Cellar/mariadb-connector-c/*/lib/mariadb
        /usr/local/lib
    )
elseif(UNIX)
    find_path(MYSQL_INCLUDE_DIR mysql/mysql.h
        PATHS /usr/include /usr/local/include
        NO_DEFAULT_PATH
    )
    find_library(MYSQL_LIBRARY mysqlclient
        PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu
    )
endif()

# Lua 5.1 / LuaJIT
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(LUA51 lua5.1)
endif()
if(NOT LUA51_FOUND)
    if(APPLE)
        find_path(LUA51_INCLUDE_DIRS lua.h
            PATHS
            /opt/homebrew/include/luajit-2.1
            /opt/homebrew/include/lua5.1
            /usr/local/include/lua5.1
        )
        find_library(LUA51_LIBRARIES luajit
            PATHS /opt/homebrew/lib /usr/local/lib
        )
        if(NOT LUA51_LIBRARIES)
            find_library(LUA51_LIBRARIES lua5.1
                PATHS /opt/homebrew/lib /usr/local/lib
            )
        endif()
    else()
        find_path(LUA51_INCLUDE_DIRS lua.h PATHS /usr/include/lua5.1 /usr/local/include/lua5.1)
        find_library(LUA51_LIBRARIES lua5.1 PATHS /usr/lib /usr/local/lib)
    endif()
endif()

# Xerces-C
if(APPLE)
    find_path(XERCES_INCLUDE_DIR xercesc/util/XercesVersion.hpp
        PATHS
        /opt/homebrew/include
        /opt/homebrew/Cellar/xerces-c/*/include
        /usr/local/include
    )
    find_library(XERCES_LIBRARY xerces-c
        PATHS /opt/homebrew/lib /usr/local/lib
    )
else()
    find_library(XERCES_LIBRARY xerces-c)
endif()

# ============================================================================

# ============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/src/Core
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/server/database
    ${MYSQL_INCLUDE_DIR}
)

if(XERCES_INCLUDE_DIR)
    include_directories(${XERCES_INCLUDE_DIR})
endif()

# ============================================================================

# ============================================================================
# Phase 12: canonical packet-class tree lives in ../shared/Packets (sibling
# to dkrix/ and dkrixserver/). Must exist before src/Core's packet library
# targets can reference the shared_packets INTERFACE target. See
# shared/Packets/README.md and docs/MODERNIZATION.md's Phase 12 section.
add_subdirectory(${CMAKE_SOURCE_DIR}/../shared/Packets ${CMAKE_BINARY_DIR}/shared_packets_build)
add_subdirectory(src/Core)
add_subdirectory(src/server)

# ============================================================================

# ============================================================================
install(TARGETS gameserver loginserver sharedserver
    RUNTIME DESTINATION bin
)
