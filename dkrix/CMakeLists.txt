# CMake 3.12+ required for file(GLOB ... CONFIGURE_DEPENDS) support —
# see the CLIENT_MAIN_SOURCES glob below. 3.12 shipped 2018-07, so every
# mainstream distro (Ubuntu 20.04+, Debian 11+, macOS Homebrew, etc.)
# has it; only Ubuntu 18.04 LTS with system cmake (3.10) needs an
# upgrade. Phase 14 (2026-04-19).
cmake_minimum_required(VERSION 3.12)

# Set SDL2 include path before project() for it to take effect
if(EXISTS "/opt/homebrew/include/SDL2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/opt/homebrew/include/SDL2")
elseif(EXISTS "/usr/local/include/SDL2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include/SDL2")
endif()

project(DarkEdenClient VERSION 1.0.0)

# Custom Find modules live in dkrix/cmake/ (shims for Ubuntu 22.04
# packages that ship no *Config.cmake — SDL2_image, SDL2_ttf).
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Note: We do NOT define _DEBUG here because it conflicts with DebugInfo.h
# Instead, rely on the new DebugLog.h system which is always available
# Debug builds will have console output enabled via log_init() in GameInit.cpp

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer configuration
option(USE_ASAN "Enable AddressSanitizer for memory error detection" OFF)
option(USE_TSAN "Enable ThreadSanitizer for race condition detection" OFF)
option(USE_UBSAN "Enable UndefinedBehaviorSanitizer for undefined behavior detection" OFF)

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

        if(USE_UBSAN)
            set(SANITIZER_FLAGS "${SANITIZER_FLAGS} -fsanitize=undefined")
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
        endif()

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")

        message(STATUS "Sanitizer flags: ${SANITIZER_FLAGS}")
    endif()
endif()

# SDL backend is now mandatory on all platforms (Windows native support removed)
set(USE_SDL_BACKEND ON CACHE BOOL "Use SDL backend instead of native Windows APIs" FORCE)

# Phase 4C: BUILD_ENGINE option retired. The tools/engine/sprite/ tree
# is being absorbed into Client/SpriteLib (C++ side wins). The only
# still-live C file was zone.c — moved into tools/viewers/map_viewer/.

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add SDL2 include directory BEFORE add_subdirectory
if(EXISTS "/opt/homebrew/include/SDL2")
	include_directories(/opt/homebrew/include/SDL2)
elseif(EXISTS "/usr/local/include/SDL2")
	include_directories(/usr/local/include/SDL2)
endif()

# Find SDL2 (needed for viewers and Client/Platform)
# Skip for Emscripten - it uses -s USE_SDL=2 linker flags instead
if(USE_SDL_BACKEND OR NOT WIN32)
	if(EMSCRIPTEN)
		# Emscripten provides SDL2 through its port system
		message(STATUS "Emscripten build: SDL2 will be provided through linker flags")
		# Add linker flags globally for all executables
		add_link_options("-sUSE_SDL=2" "-sUSE_SDL_IMAGE=2" "-sUSE_SDL_TTF=2")
		# Create interface targets for compatibility with empty link libraries
		add_library(SDL2_interface INTERFACE IMPORTED)
		add_library(SDL2_image_interface INTERFACE IMPORTED)
		add_library(SDL2_ttf_interface INTERFACE IMPORTED)
		add_library(SDL2main_interface INTERFACE IMPORTED)

		# Create aliases for compatibility
		add_library(SDL2::SDL2 ALIAS SDL2_interface)
		add_library(SDL2::SDL2main ALIAS SDL2main_interface)
		add_library(SDL2_image::SDL2_image ALIAS SDL2_image_interface)
		add_library(SDL2_ttf::SDL2_ttf ALIAS SDL2_ttf_interface)
	else()
		find_package(SDL2 REQUIRED)
		find_package(SDL2_image REQUIRED)
		find_package(SDL2_ttf REQUIRED)
	endif()
endif()

# Find JPEG library (needed for UtilityFunction.cpp)
find_package(JPEG QUIET)
if(NOT JPEG_FOUND)
	# Try pkg-config as fallback
	find_package(PkgConfig QUIET)
	if(PKG_CONFIG_FOUND)
		pkg_check_modules(JPEG libjpeg)
	endif()
endif()
if(JPEG_FOUND OR JPEG_LIBRARIES)
	message(STATUS "JPEG library found: ${JPEG_LIBRARIES}")
else()
	message(WARNING "JPEG library not found - JPG loading/saving may not work")
endif()

# Subdirectories
add_subdirectory(basic)
add_subdirectory(Client/Platform)
add_subdirectory(Client/SpriteLib)

# Additional libraries from VC6 project
add_subdirectory(Client/framelib)

# Phase 12 migration: canonical packet-class tree lives in
# ../shared/Packets (sibling to dkrix/ and dkrixserver/). Defines
# the shared_packets INTERFACE target that DarkEden links below so
# its sources (e.g. CGStoreOpen.cpp) are compiled into the client
# without the __GAME_SERVER__/__LOGIN_SERVER__ defines the server
# packet libraries apply.
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../shared/Packets ${CMAKE_BINARY_DIR}/shared_packets_build)

# ============================================================
# TextSystem - New SDL-based Text System (Abstracted)
# ============================================================
if(USE_SDL_BACKEND OR NOT WIN32)
	set(TEXTSYSTEM_SOURCES
		Client/TextSystem/TextService.cpp
		Client/TextSystem/TextBackendSDL.cpp
		Client/TextSystem/RenderTargetSpriteSurface.cpp
	)

	add_library(TextSystem STATIC ${TEXTSYSTEM_SOURCES})

	target_include_directories(TextSystem PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/Client/TextSystem
		${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
		${CMAKE_CURRENT_SOURCE_DIR}/Client
		${CMAKE_CURRENT_SOURCE_DIR}/basic
		${SDL2_INCLUDE_DIRS}
	)

	target_link_libraries(TextSystem PUBLIC
		SpriteLib
		SDL2::SDL2
		SDL2_ttf::SDL2_ttf
	)

	target_compile_definitions(TextSystem PRIVATE SPRITELIB_BACKEND_SDL USE_SDL_BACKEND)
	if(NOT WIN32)
		target_compile_definitions(TextSystem PRIVATE PLATFORM_MACOS)
	endif()

	message(STATUS "TextSystem library will be built from ${TEXTSYSTEM_SOURCES}")
endif()

# Phase 4C: tools/engine/sprite/ no longer wired (retired in 4D).

# ============================================================
# VS_UI Library - UI System (按需编译必要的 Client 文件)
# ============================================================
if(USE_SDL_BACKEND OR NOT WIN32)
	# VS_UI source files — explicit list (Phase 14B, 2026-04-19).
	# Replaces the previous file(GLOB_RECURSE VS_UI/src/*.cpp VS_UI/*.cpp)
	# because CMake globs are not re-evaluated on incremental builds.
	# See docs/MODERNIZATION.md Phase 14 block. Adding or removing a
	# .cpp under VS_UI/ now requires editing this list.
	#
	# Regenerate with:
	#   cd dkrix && find VS_UI -name "*.cpp" | sort
	#
	# Generated 2026-04-19: 56 files (6 root + 50 under src/).
	set(VS_UI_SRC_SOURCES
		# VS_UI root (6 files)
		VS_UI/Client_PCH.cpp
		VS_UI/DebugInfo.cpp
		VS_UI/MitemTableInit.cpp
		VS_UI/RarFile.cpp
		VS_UI/SXml.cpp
		VS_UI/WinMain.cpp
		# VS_UI/src (44 flat files)
		VS_UI/src/AcceleratorDef.cpp
		VS_UI/src/AcceleratorManager.cpp
		VS_UI/src/Imm/CImm.cpp
		VS_UI/src/InputFocusManager.cpp
		VS_UI/src/KeyAccelerator.cpp
		VS_UI/src/SkinManager.cpp
		VS_UI/src/VS_UI_BBS.cpp
		VS_UI/src/VS_UI_Base.cpp
		VS_UI/src/VS_UI_DESC.cpp
		VS_UI/src/VS_UI_Description.cpp
		VS_UI/src/VS_UI_Descriptor.cpp
		VS_UI/src/VS_UI_Dialog.cpp
		VS_UI/src/VS_UI_ELEVATOR.cpp
		VS_UI/src/VS_UI_Exception.cpp
		VS_UI/src/VS_UI_Exchange.cpp
		VS_UI/src/VS_UI_ExtraDialog.cpp
		VS_UI/src/VS_UI_Game.cpp
		VS_UI/src/VS_UI_GameCommon.cpp
		VS_UI/src/VS_UI_GameOusters.cpp
		VS_UI/src/VS_UI_GameSlayer.cpp
		VS_UI/src/VS_UI_GameVampire.cpp
		VS_UI/src/VS_UI_GlobalResource.cpp
		VS_UI/src/VS_UI_Helper.cpp
		VS_UI/src/VS_UI_Item.cpp
		VS_UI/src/VS_UI_Message.cpp
		VS_UI/src/VS_UI_PetStorage.cpp
		VS_UI/src/VS_UI_PointExchange.cpp
		VS_UI/src/VS_UI_SKILL_VIEW.cpp
		VS_UI/src/VS_UI_Shop.cpp
		VS_UI/src/VS_UI_Storage.cpp
		VS_UI/src/VS_UI_Title.cpp
		# Bug OO (2026-04-20): MSVC6-era uppercase .CPP not picked up
		# by Phase 14B's `find VS_UI -name "*.cpp"` generator. Provides
		# C_VS_UI_NEWCHAR::_ShowCharacter{Slayer,Vampire,Ousters,AC*}
		# referenced from VS_UI_Title.cpp.
		VS_UI/src/VS_UI_TITLE_SHOWCHAR.CPP
		VS_UI/src/VS_UI_Tutorial.cpp
		VS_UI/src/VS_UI_WebBrowser.cpp
		VS_UI/src/VS_UI_mouse_pointer.cpp
		VS_UI/src/VS_UI_progress.cpp
		VS_UI/src/VS_UI_skill_tree.cpp
		VS_UI/src/VS_UI_ui_result_receiver.cpp
		VS_UI/src/VS_UI_util.cpp
		VS_UI/src/VS_UI_widget.cpp
		VS_UI/src/Vs_ui.cpp
		VS_UI/src/vs_ui_gamecommon2.cpp
		# VS_UI/src/hangul (3 files)
		VS_UI/src/hangul/Ci.cpp
		VS_UI/src/hangul/Ci_macOS.cpp
		VS_UI/src/hangul/FL2.cpp
		# VS_UI/src/widget (6 files)
		VS_UI/src/widget/U_edit.cpp
		VS_UI/src/widget/mother.cpp
		VS_UI/src/widget/pi_core.cpp
		VS_UI/src/widget/u_button.cpp
		VS_UI/src/widget/u_scrollbar.cpp
		VS_UI/src/widget/u_window.cpp
	)

	# Exclude Windows-specific files
	if(NOT WIN32)
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*Imm.*")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*WinMain.*")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*WebBrowser.*")
		# Exclude Windows-specific Korean input method files (except macOS stub)
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/Ci\\.cpp")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/FL2\\.cpp")
		# U_edit is now cross-platform (SDL-based), so don't exclude it
		# Exclude problematic files in VS_UI root directory
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/DebugInfo\\.cpp")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/MitemTableInit\\.cpp")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/SXml\\.cpp")
		# Note: u_window.cpp is now cross-platform compatible
	endif()

	# Client files that VS_UI depends on (按需编译)
	set(VS_UI_CLIENT_SOURCES
		# Core
		Client/Client.cpp
		Client/ClientFunction.cpp

		# Item System
		Client/MItem.cpp
		Client/MItemManager.cpp
		Client/MItemOptionTable.cpp
		Client/MMoneyManager.cpp

		# Skill System
		Client/MSkillManager.cpp
		Client/SkillDef.cpp

		# Gear System
		Client/MSlayerGear.cpp
		Client/MVampireGear.cpp
		Client/MOustersGear.cpp
		Client/MQuickSlot.cpp

		# Inventory System
		Client/MInventory.cpp
		Client/MStorage.cpp
		Client/MShopShelf.cpp

		# Zone System
		Client/MZone.cpp
		Client/MZoneTable.cpp

		# Player System
		Client/MPlayer.cpp
		Client/MCreatureTable.cpp
		Client/MFakeCreature.cpp
		Client/MNpcTable.cpp
		Client/MNPCTable.cpp

		# Social System
		Client/MParty.cpp
		Client/MGuildMarkManager.cpp
		Client/MGuildInfoMapper.cpp

		# Trade System
		Client/MTradeManager.cpp
		Client/MPriceManager.cpp
		Client/MShop.cpp

		# Game Data
		Client/MGameStringTable.cpp
		Client/ExperienceTable.cpp
		Client/UserInformation.cpp
		Client/SystemAvailabilities.cpp
		Client/FameInfo.cpp
		Client/DebugInfo.cpp
		Client/ClientConfig.cpp
		Client/UtilityFunction.cpp
		Client/RenderingFunctions.cpp
		Client/UIUtilityFunctions.cpp

		# Events & Time
		Client/MEventManager.cpp
		Client/MGameTime.cpp
		Client/MTimeItemManager.cpp

		# UI Support
		Client/MHelpMessageManager.cpp
		Client/MSoundTable.cpp
		Client/MMonsterKillQuestInfo.cpp
	)

	# Combine all sources
	set(VS_UI_ALL_SOURCES ${VS_UI_SRC_SOURCES} ${VS_UI_CLIENT_SOURCES})

	# Create VS_UI static library
	add_library(VS_UI STATIC ${VS_UI_ALL_SOURCES})

	# Include directories
	target_include_directories(VS_UI PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/header
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/widget
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/hangul
		${CMAKE_CURRENT_SOURCE_DIR}/Client
		${CMAKE_CURRENT_SOURCE_DIR}/Client/TextSystem
		${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
		${CMAKE_CURRENT_SOURCE_DIR}/Client/framelib
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Packet
		${CMAKE_CURRENT_SOURCE_DIR}
	)

	# Link libraries
	target_link_libraries(VS_UI PUBLIC
		basic
		platform
		SpriteLib
		framelib
		TextSystem
		SDL2::SDL2
		SDL2::SDL2main
		SDL2_image::SDL2_image
		SDL2_ttf::SDL2_ttf
	)

	# Platform-specific definitions
	if(NOT WIN32)
		target_compile_definitions(VS_UI PRIVATE PLATFORM_MACOS)
		# Also pass PLATFORM_MACOS to Client files compiled into VS_UI
		target_compile_definitions(VS_UI PUBLIC PLATFORM_MACOS)
	endif()

	# Define _LIB to match original VC6 build configuration
	# This excludes test/debug code wrapped in #ifndef _LIB from library builds
	target_compile_definitions(VS_UI PRIVATE _LIB)

	message(STATUS "VS_UI library will be built from ${VS_UI_SRC_SOURCES} VS_UI files + ${VS_UI_CLIENT_SOURCES} Client files")
endif()

# ============================================================
# Resource Management Tools
# ============================================================

# Resource Validator - checks all resource files for integrity
add_executable(resource_validator
	Client/tools/resource_validator.cpp
)

target_include_directories(resource_validator PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(resource_validator PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ============================================================
# Resource Viewing Tools
# ============================================================

# Sprite Viewer - view .spk sprite pack files (C++ version using Client/SpriteLib)
add_executable(sprite_viewer
	tools/viewers/sprite_viewer/main.cpp
)

target_include_directories(sprite_viewer PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
	${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
)

target_link_libraries(sprite_viewer PRIVATE
	SpriteLib
	platform
	basic
	SDL2::SDL2
)

set_target_properties(sprite_viewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)


# Zone Parser - parse and dump .map zone files (standalone C++ version)
add_executable(zone_parser
	tools/viewers/zone_parser/main.cpp
)

target_link_libraries(zone_parser PRIVATE)

set_target_properties(zone_parser PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Item Viewer - view .ispk indexed sprite pack files (C++ version using Client/SpriteLib)
add_executable(item_viewer
	tools/viewers/item_viewer/main.cpp
)

target_include_directories(item_viewer PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
	${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
)

target_link_libraries(item_viewer PRIVATE
	SpriteLib
	platform
	basic
	SDL2::SDL2
)

set_target_properties(item_viewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Creature Viewer - view creature animations (C++ version using Client/SpriteLib + Client/framelib)
add_executable(creature_viewer
	tools/viewers/creature_viewer/main.cpp
)

target_include_directories(creature_viewer PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
	${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
	${CMAKE_CURRENT_SOURCE_DIR}/Client/framelib
)

target_link_libraries(creature_viewer PRIVATE
	SpriteLib
	framelib
	platform
	basic
	SDL2::SDL2
)

set_target_properties(creature_viewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Map Viewer - browse zone maps with pan/zoom (C++ version using Client/SpriteLib).
# Phase 4C: zone.c is now compiled in-tree rather than linked from the retired
# tools/engine/sprite `sprite` library.
add_executable(map_viewer
	tools/viewers/map_viewer/main.cpp
	tools/viewers/map_viewer/zoneloader.cpp
	tools/viewers/map_viewer/zone.c
	Client/TileRenderer.cpp
)

target_include_directories(map_viewer PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/Client
	${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
	${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
	${CMAKE_CURRENT_SOURCE_DIR}/tools/viewers/map_viewer
)

target_link_libraries(map_viewer PRIVATE
	SpriteLib
	platform
	basic
	SDL2::SDL2
)

set_target_properties(map_viewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Effect Viewer - test and debug effect system
add_executable(effect_viewer
	tools/viewers/effect_viewer/main.cpp
	tools/viewers/effect_viewer/EffectSpriteTypeTable.cpp
	tools/viewers/effect_viewer/stubs.cpp
	Client/MObject.cpp
	Client/EffectResourceContainer.cpp
	Client/MEffect.cpp
)

target_include_directories(effect_viewer PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/Client
	${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
	${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
	${CMAKE_CURRENT_SOURCE_DIR}/Client/framelib
	${CMAKE_CURRENT_SOURCE_DIR}/basic
)

target_link_libraries(effect_viewer PRIVATE
	SpriteLib
	framelib
	platform
	basic
	SDL2::SDL2
)

# Set output directory for effect_viewer
# Use CMAKE_BINARY_DIR to avoid nesting (build/debug-asan/bin/)
if(CMAKE_BUILD_TYPE STREQUAL "debug-asan")
	set_target_properties(effect_viewer PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	)
else()
	set_target_properties(effect_viewer PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	)
endif()


# Examples (always built if backend is enabled)
# Examples directory removed during cleanup
# if(USE_SDL_BACKEND OR NOT WIN32)
# 	add_subdirectory(examples)
# endif()

# Dark Eden Client Executable
if(USE_SDL_BACKEND OR NOT WIN32)
	# Collect remaining Client source files (excluding those already in VS_UI).
	#
	# Phase 14C (2026-04-19): CONFIGURE_DEPENDS added so CMake re-evaluates
	# the glob on every build instead of caching the file list at initial
	# configure time. Covers ~1045 .cpp files across Client/ root (242),
	# Client/Packet/ recursive (801), Client/SXml/ (1), Client/WinLib/ (1).
	#
	# Why a glob + CONFIGURE_DEPENDS instead of an explicit list:
	#   - The Packet tree alone is 801 files across 6 sub-packages; an
	#     explicit enumeration would add ~1060 lines of CMake and a
	#     CMake edit for every new packet.
	#   - CONFIGURE_DEPENDS (CMake 3.12+) solves the exact problem the
	#     explicit-list plan was meant to solve: "adding a new source
	#     file silently fails to compile until the next cmake reconfigure."
	#     With CONFIGURE_DEPENDS, cmake re-runs the glob automatically.
	#   - VS_UI (Phase 14B) kept its explicit list because it's small
	#     (56 files) and stable.
	#
	# The subdirectories NOT matched by this glob (Client/Platform,
	# Client/SpriteLib, Client/framelib, Client/TextSystem, Client/Updater,
	# Client/OtherClass, Client/JpegLib, Client/tools) are wired through
	# separate add_subdirectory() / add_library() targets above.
	file(GLOB CLIENT_MAIN_SOURCES CONFIGURE_DEPENDS
		Client/*.cpp
		Client/Packet/*.cpp
		Client/Packet/**/*.cpp
		Client/SXml/*.cpp
		Client/WinLib/*.cpp
	)

	# Add additional source files
	list(APPEND CLIENT_MAIN_SOURCES
		Client/COGGSTREAM.cpp
		# Time functions extracted from PacketFunction.cpp (which has
		# Windows-specific networking code).
		Client/ClientTimeFunctions.cpp
		# Global variables and small utility functions that used to live
		# in GamePacketFunctions.cpp.
		Client/Globals.cpp
		# Bug OO (2026-04-20): MSVC6-era files with uppercase .CPP
		# extension are silently dropped by the case-sensitive
		# file(GLOB Client/*.cpp) above on Linux. List them explicitly
		# to get them into the build graph. BIT_RES.CPP provides the
		# Bit_Reserve class used by mp3.cpp + subdecoder.cpp; the other
		# four define managers referenced from various GC*Handler.cpp.
		Client/BIT_RES.CPP
		Client/MAttachZoneAroundEffectGenerator.CPP
		Client/MBloodyBreakerEffectGenerator.CPP
		Client/MTimeItemManager.CPP
		Client/MWarManager.CPP
	)

	# Remove files that are already compiled in VS_UI library.
	# The nine duplicate .cpp files that used to be excluded here
	# (GlobalVariables, GCNotifyWin{,Handler}, MitemTableinit2, MissingGlobals,
	# GameHelpers, GameFunctions, GamePacketFunctions, ActionFunctions) have
	# been deleted from the tree outright — see docs/MODERNIZATION.md Phase 1.
	list(REMOVE_ITEM CLIENT_MAIN_SOURCES
		${VS_UI_CLIENT_SOURCES}
	)

	# Note: Cpackets contains CG (Client to Game) handlers. The server-side
	# *Handler.cpp sources that used to live here were deleted outright (see
	# docs/MODERNIZATION.md Phase 1); the 14 Handler methods that needed a
	# client-side link target are stubbed in Client/CGHandlersStub.cpp.
	# The FILTER EXCLUDE that used to remove these files is no longer needed.


	# Exclude Windows-specific files
	if(NOT WIN32)
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*Imm.*")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*D3D.*")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*Direct3.*")
		# list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*GameMain.*")  # Re-enabled for cross-platform support
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*EXECryptor.*")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*GetWinVer.*")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*\\.bak.*")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*_bak-.*")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*MInternetConnection.*")
		# Exclude Handler files in Client root only (not in Packet subdirectories)
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX "^Client/[^/]+Handler\\.cpp")
		# Exclude Client.cpp and ClientFunction.cpp - they're already in VS_UI library
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX "^Client/Client\\.cpp")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX "^Client/ClientFunction\\.cpp")
		# MWorkThread and MMusic are needed on all platforms
		# list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*MWorkThread.*")
		# list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*MMusic\\.cpp")
		# Note: PacketFunction.cpp now included with proper platform guards
		# Windows-specific NetBIOS/IPX/SPX code is wrapped in #ifdef PLATFORM_WINDOWS
		# list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*PacketFunction\\.cpp")
		# ProfileManager needed on all platforms
		# list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*ProfileManager\\.cpp")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*VolumeOut.*\\.cpp")
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX ".*WavePackFileManager\\.cpp")
		# Exclude Windows-specific WinMain files
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX "WinLib/CWinMain\\.cpp")
		# Exclude Client.cpp on non-Windows (uses WinMain)
		list(FILTER CLIENT_MAIN_SOURCES EXCLUDE REGEX "Client/Client\\.cpp")
	endif()

	# Add DarkEden executable
	add_executable(DarkEden ${CLIENT_MAIN_SOURCES})

	# Set output directory
	set_target_properties(DarkEden PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	)

	# Link libraries (now includes VS_UI and TextSystem)
	target_link_libraries(DarkEden PRIVATE
		SpriteLib
		platform
		basic
		framelib
		VS_UI
		TextSystem
		shared_packets
		SDL2::SDL2
		SDL2::SDL2main
	)

	# Link iconv library - not needed for Emscripten (stub impls) and
	# not needed on glibc Linux (iconv symbols are built into libc).
	# find_package(Iconv) sets Iconv_IS_BUILT_IN when the platform's
	# libc already provides iconv(); otherwise it resolves Iconv::Iconv
	# to the standalone libiconv (macOS/BSD/MinGW).
	if(NOT EMSCRIPTEN)
		find_package(Iconv)
		if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
			target_link_libraries(DarkEden PRIVATE Iconv::Iconv)
		endif()
	endif()

	# Link JPEG library if available
	if(JPEG_LIBRARIES)
		target_link_libraries(DarkEden PRIVATE ${JPEG_LIBRARIES})
		if(JPEG_INCLUDE_DIRS)
			target_include_directories(DarkEden PRIVATE ${JPEG_INCLUDE_DIRS})
		endif()
	endif()

	# Include directories
	target_include_directories(DarkEden PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/basic
		${CMAKE_CURRENT_SOURCE_DIR}/Client
		${CMAKE_CURRENT_SOURCE_DIR}/Client/TextSystem
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
		${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Packet
		${CMAKE_CURRENT_SOURCE_DIR}/Client/framelib
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/header
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/widget
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/hangul
	)

	# Add Immersion library include path only on Windows
	if(WIN32)
		target_include_directories(DarkEden PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/Imm
		)
	endif()

	# Compile definitions
	target_compile_definitions(DarkEden PRIVATE
		USE_SDL_BACKEND
		SPRITELIB_BACKEND_SDL
		__GAME_CLIENT__=1
	)

	# Platform-specific definitions
	if(NOT WIN32)
		target_compile_definitions(DarkEden PRIVATE PLATFORM_MACOS)
	endif()

	message(STATUS "DarkEden executable will be built from remaining Client files (VS_UI compiled separately)")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== Dark Eden Client Configuration ===")
message(STATUS "  Build Tests:      ${BUILD_TESTS}")
message(STATUS "  Use SDL Backend:  ${USE_SDL_BACKEND}")
message(STATUS "  C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "=======================================")
message(STATUS "")
