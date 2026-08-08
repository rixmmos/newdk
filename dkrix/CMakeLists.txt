# CMake 3.12+ required for file(GLOB ... CONFIGURE_DEPENDS) support -
# see the CLIENT_MAIN_SOURCES glob below. 3.12 shipped 2018-07, so every
# mainstream distro (Ubuntu 20.04+, Debian 11+, macOS Homebrew, etc.)
# has it; only Ubuntu 18.04 LTS with system cmake (3.10) needs an
# upgrade. Phase 10 bullet 2 (2026-08-07).
cmake_minimum_required(VERSION 3.12)

# Set SDL2 include path before project() for it to take effect
if(EXISTS "/opt/homebrew/include/SDL2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/opt/homebrew/include/SDL2")
elseif(EXISTS "/usr/local/include/SDL2")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include/SDL2")
endif()

project(DarkEdenClient VERSION 1.0.0)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use a modern Windows SDK baseline so WinSDK declarations are available consistently.
if(WIN32)
	add_compile_definitions(
		PLATFORM_WINDOWS
		WINVER=0x0A00
		_WIN32_WINNT=0x0A00
		NTDDI_VERSION=0x0A000000
	)
endif()

if(MSVC)
	add_compile_options(/FS)
endif()

# Note: We do NOT define _DEBUG here because it conflicts with DebugInfo.h
# Instead, rely on the new DebugLog.h system which is always available
# Debug builds will have console output enabled via log_init() in GameInit.cpp

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sanitizer configuration
option(USE_ASAN "Enable AddressSanitizer for memory error detection" OFF)
option(USE_TSAN "Enable ThreadSanitizer for race condition detection" OFF)
option(USE_UBSAN "Enable UndefinedBehaviorSanitizer for undefined behavior detection" OFF)

# ASan and TSan instrument memory access differently (shadow memory vs.
# interceptor-based happens-before tracking) and cannot be linked into
# the same binary. UBSan is a separate, compatible check and may be
# combined with either. (Phase 10 bullet 3, 2026-08-07 — mirrors the
# same guard added to dkrixserver/CMakeLists.txt.)
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

option(BUILD_ENGINE "Build new SDL-based engine" ON)

if(WIN32)
	set(BUILD_ENGINE OFF CACHE BOOL "Build new SDL-based engine" FORCE)
endif()

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

# Find SDL2 (needed for engine and DXLib)
# Skip for Emscripten - it uses -s USE_SDL=2 linker flags instead
if(BUILD_ENGINE OR USE_SDL_BACKEND OR NOT WIN32)
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

# Windows SDL executables that include <SDL.h> directly need SDL2main.
set(SDL2_VIEWER_MAIN_LIB)
if(WIN32)
	set(SDL2_VIEWER_MAIN_LIB SDL2::SDL2main)
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

# Phase 12: canonical packet-class tree lives in ../shared/Packets (sibling
# to dkrix/ and dkrixserver/). Defines the shared_packets INTERFACE target
# that DarkEden links below. See shared/Packets/README.md and
# docs/MODERNIZATION.md's Phase 12 section.
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

if(BUILD_ENGINE)
	add_subdirectory(tools/engine/sprite)
endif()

# ============================================================
# VS_UI Library - UI System ( Client )
# ============================================================
if(USE_SDL_BACKEND OR NOT WIN32)
	# VS_UI source files - explicit list (Phase 10 bullet 2, 2026-08-07).
	# Replaces file(GLOB_RECURSE VS_UI_SRC_SOURCES VS_UI/src/*.cpp VS_UI/*.cpp)
	# because CMake globs are not re-evaluated on incremental builds:
	# adding a new .cpp under VS_UI/ silently failed to compile until the
	# next `cmake` reconfigure. See docs/MODERNIZATION.md Phase 10.
	#
	# Regenerate with (case-INsensitive -iname: VS_UI_TITLE_SHOWCHAR.CPP
	# is a real, live, uppercase-extension source file that a plain
	# `-name "*.cpp"` silently drops on a case-sensitive filesystem -
	# it built fine under the old glob only because that ran through
	# CMake on case-insensitive Windows/NTFS. Cost one red CI run to
	# find; verify against this exact list, not `-name`, next time):
	#   cd dkrix && find VS_UI -iname "*.cpp" | sort
	#
	# Generated 2026-08-07, corrected same day: 57 files (6 root + 51
	# under src/, incl. the one uppercase-extension file above).
	set(VS_UI_SRC_SOURCES
		# VS_UI root (6 files)
		VS_UI/Client_PCH.cpp
		VS_UI/DebugInfo.cpp
		VS_UI/MitemTableInit.cpp
		VS_UI/RarFile.cpp
		VS_UI/SXml.cpp
		VS_UI/WinMain.cpp
		# VS_UI/src (45 flat files)
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

	# VS_UI/WinMain.cpp is a standalone circa-2000 "VS UI Tester" dev harness
	# (see its own file header comment) with a genuine Win32 entry point -
	# `int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)` - meaningless as
	# an OS entry point outside Win32. It is NOT the non-Windows client entry
	# point: that is Client/SDLMain.cpp's `int main()`, whose own header says
	# it is "Based on WinMain from Client.cpp" and which is gated on
	# `#ifndef PLATFORM_WINDOWS`. This file used to be excluded only
	# `if(WIN32)`, so non-Windows was the only platform where it ever
	# actually compiled, and there it:
	#   (a) declared `CDirectDraw gC_DD;` at file scope - a type Phase 3
	#       item 3 (2026-08-07) renamed to CSDLGraphics, so it failed to
	#       compile at all (first bug hit by the sanitizers-linux CI leg);
	#   (b) would have collided at link time regardless, since it redefines
	#       globals Client/Client.cpp already defines unconditionally into
	#       the same VS_UI static library on every platform via
	#       VS_UI_CLIENT_SOURCES - e.g. both declare `HWND g_hWnd;`.
	# Fixing (a) alone (CDirectDraw -> CSDLGraphics) would only trade one
	# build failure for another, so exclude the file unconditionally rather
	# than patch a type it was never meant to use here. Nothing outside this
	# file references its other globals/functions (gC_DD, gC_DDSurface,
	# ProgramLoop, InitSound, MouseEventReceiver, ...) - verified by grep.
	list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*WinMain.*")

	# Exclude Windows-specific files
	if(WIN32)
		if(USE_SDL_BACKEND)
			list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/Ci\\.cpp")
			list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/Imm/CImm\\.cpp")
		else()
			list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/Ci_macOS\\.cpp")
		endif()
		if(USE_SDL_BACKEND)
			list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/FL2\\.cpp")
		endif()
	endif()

	if(NOT WIN32)
		# VS_UI_WebBrowser.cpp implements the real COM/ActiveX C_VS_UI_WEBBROWSER
		# (VS_UI_WebBrowser.h guards it behind PLATFORM_WINDOWS + <exdisp.h>/<atlbase.h>
		# and falls back to an inline stub class otherwise) - Windows-only, and the
		# only platform where VS_UI_Game.cpp's calls into it need a real definition.
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*WebBrowser.*")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX ".*Imm.*")
		# Exclude Windows-specific Korean input method files (except macOS stub)
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/Ci\\.cpp")
		list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/src/hangul/FL2\\.cpp")
	endif()

	# Exclude problematic VS_UI root files from the library baseline build
	list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/DebugInfo\\.cpp")
	list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/MitemTableInit\\.cpp")
	list(FILTER VS_UI_SRC_SOURCES EXCLUDE REGEX "VS_UI/SXml\\.cpp")

	# Client files that VS_UI depends on ()
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
		# GameFunctions.cpp - Moved to CLIENT_MAIN_SOURCES to avoid duplicates with GameMain.cpp
		# GamePacketFunctions.cpp - Removed, all functions now in PacketFunction.cpp
		Client/RenderingFunctions.cpp
		Client/UIUtilityFunctions.cpp

		# Events & Time
		Client/MEventManager.cpp
		Client/MGameTime.cpp
		Client/MTimeItemManager.CPP

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
		${CMAKE_CURRENT_SOURCE_DIR}/VS_UI/src/Imm
		${CMAKE_CURRENT_SOURCE_DIR}/Client
		${CMAKE_CURRENT_SOURCE_DIR}/Client/TextSystem
		${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
		${CMAKE_CURRENT_SOURCE_DIR}/Client/framelib
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
		SDL2_image::SDL2_image
		SDL2_ttf::SDL2_ttf
	)

	if(NOT WIN32)
		target_link_libraries(VS_UI PUBLIC SDL2::SDL2main)
	endif()

	# Platform-specific definitions
	if(NOT WIN32)
		target_compile_definitions(VS_UI PRIVATE PLATFORM_MACOS)
		# Also pass PLATFORM_MACOS to Client files compiled into VS_UI
		target_compile_definitions(VS_UI PUBLIC PLATFORM_MACOS)
	endif()

	# Define _LIB to match original VC6 build configuration
	# This excludes test/debug code wrapped in #ifndef _LIB from library builds
	target_compile_definitions(VS_UI PRIVATE _LIB USE_SDL_BACKEND SPRITELIB_BACKEND_SDL)

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
	${SDL2_VIEWER_MAIN_LIB}
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
	${SDL2_VIEWER_MAIN_LIB}
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
	${SDL2_VIEWER_MAIN_LIB}
)

set_target_properties(creature_viewer PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

if(NOT WIN32)
	# Map Viewer - browse zone maps with pan/zoom (C++ version using Client/SpriteLib)
	add_executable(map_viewer
		tools/viewers/map_viewer/main.cpp
		tools/viewers/map_viewer/zoneloader.cpp
		Client/TileRenderer.cpp
	)

	target_include_directories(map_viewer PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_CURRENT_SOURCE_DIR}/Client
		${CMAKE_CURRENT_SOURCE_DIR}/Client/SpriteLib
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Platform
		${CMAKE_CURRENT_SOURCE_DIR}/tools/engine/sprite/include
	)

	target_link_libraries(map_viewer PRIVATE
		sprite
		SpriteLib
		platform
		basic
		SDL2::SDL2
		${SDL2_VIEWER_MAIN_LIB}
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
		sprite
		SpriteLib
		framelib
		platform
		basic
		SDL2::SDL2
		${SDL2_VIEWER_MAIN_LIB}
	)

	if(CMAKE_BUILD_TYPE STREQUAL "debug-asan")
		set_target_properties(effect_viewer PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
		)
	else()
		set_target_properties(effect_viewer PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
		)
	endif()
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
	# Phase 10 bullet 2 (2026-08-07): CONFIGURE_DEPENDS added so CMake
	# re-evaluates the glob on every build instead of caching the file
	# list at initial configure time. Covers ~1045 .cpp files across
	# Client/ root (242), Client/Packet/ root (66) + one level of
	# sub-packages (735), Client/SXml/ (1), Client/WinLib/ (1).
	#
	# Why a glob + CONFIGURE_DEPENDS instead of an explicit list: the
	# Packet tree alone is 801 files across dozens of sub-packages; an
	# explicit enumeration would add ~1050 lines of CMake and require a
	# CMake edit for every new packet added to Client/Packet/.
	# CONFIGURE_DEPENDS (CMake 3.12+) solves the exact problem the
	# explicit-list approach would solve - "adding a new source file
	# silently fails to compile until the next cmake reconfigure" -
	# without the maintenance cost. VS_UI (above) keeps its explicit
	# list because it's small (56 files) and stable; the tradeoff only
	# works the other way for a tree this size. See docs/MODERNIZATION.md
	# Phase 10.
	file(GLOB CLIENT_MAIN_SOURCES CONFIGURE_DEPENDS
		Client/*.cpp
		Client/Packet/*.cpp
		Client/Packet/**/*.cpp
		Client/SXml/*.cpp
		Client/WinLib/*.cpp
	)

	# Add additional source files
	#
	# The four below (BIT_RES/MAttachZoneAroundEffectGenerator/
	# MBloodyBreakerEffectGenerator/MWarManager), plus COGGSTREAM, share the
	# same bug: real, live, uppercase-extension .CPP files under Client/ that
	# the case-sensitive `Client/*.cpp` glob above silently drops on Linux.
	# On Windows/NTFS the glob matched them anyway (case-insensitive
	# filesystem), so they built into every Windows binary without anyone
	# noticing they were never explicitly listed. Found via a full
	# case-sensitive sweep of Client/ for *.CPP (2026-08-08); all four are
	# referenced from live callers (MEffectGeneratorTable.cpp, MPlayer.cpp,
	# GameMain.cpp, huffman.cpp, etc.) - not dead code.
	list(APPEND CLIENT_MAIN_SOURCES
		Client/COGGSTREAM.CPP
		Client/BIT_RES.CPP
		Client/MAttachZoneAroundEffectGenerator.CPP
		Client/MBloodyBreakerEffectGenerator.CPP
		Client/MWarManager.CPP
		# Time functions extracted from PacketFunction.cpp (which has Windows-specific networking code)
		Client/ClientTimeFunctions.cpp
		# ActionFunctions.cpp excluded - functions now in PacketFunction.cpp
		# Client/ActionFunctions.cpp
		# Globals.cpp - Global variables and utility functions (from GamePacketFunctions.cpp)
		Client/Globals.cpp
	)

	# Remove files that are already compiled in VS_UI library
	list(REMOVE_ITEM CLIENT_MAIN_SOURCES
		${VS_UI_CLIENT_SOURCES}
		# Exclude legacy Client sprite implementation; SpriteLib already provides it
		${CMAKE_CURRENT_SOURCE_DIR}/Client/CSpritePal.cpp
	)

	# Note: Client/Packet/Cpackets/*Handler.cpp (CG server-side packet handlers)
	# and nine duplicate Client/*.cpp files (GlobalVariables, MissingGlobals,
	# GameHelpers, GameFunctions, GamePacketFunctions, ActionFunctions,
	# MitemTableinit2, GCNotifyWinHandler, GCNotifyWin) used to be excluded here.
	# They were deleted outright in Phase 1 of docs/MODERNIZATION.md
	# (2026-08-06) instead of just being excluded from the glob.

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
		basic
		TextSystem
		# Phase 12: absorbs shared/Packets/ sources (e.g. CLGetWorldList.cpp)
		# into this target's own compilation units, with __GAME_CLIENT__=1
		# applied below (no __GAME_SERVER__/__LOGIN_SERVER__ defines).
		shared_packets
		SDL2::SDL2
	)

	if(NOT WIN32)
		target_link_libraries(DarkEden PRIVATE SDL2::SDL2main)
	endif()

	if(WIN32)
		target_link_libraries(DarkEden PRIVATE ws2_32 netapi32 comctl32 wininet shlwapi)
	endif()

	# Phase 12: shared/Packets/*.h use quoted #include "Packet.h" /
	# "PacketFactory.h" (both live in Client/Packet/, not shared/Packets/).
	# Previously resolved only because Cpackets/*.h was #included from
	# Client/Packet/PacketFactoryManager.cpp, and MSVC's quoted-include
	# search walks the #include stack up to that includer's directory.
	# shared/Packets/*.cpp is now a standalone translation unit
	# (target_sources on shared_packets, not #included from anywhere) with
	# no such stack to walk.
	#
	# Two things already tried and reverted, kept here so the next person
	# doesn't repeat them:
	#  1. Adding Client/Packet to DarkEden's target-wide include dirs --
	#     broke the build worse. Client/Packet/ has generically named
	#     headers (FileAPI.h chief among them) that, once on every one of
	#     DarkEden's ~1000 translation units' /I path, shadow the real
	#     Windows SDK <fileapi.h> (case-insensitive match) ahead of the
	#     toolchain's own Windows Kits include path -- CreateFile,
	#     DeleteFile, FindFirstFile, ReadFile, GetFileSize etc. all vanish
	#     from the global namespace and winbase.h itself fails to parse
	#     (STREAM_INFO_LEVELS) wherever <windows.h> is later pulled in
	#     (ATL headers, VS_UI_WebBrowser.cpp's COM code, ...).
	#  2. Scoping it to just the shared_packets sources via the
	#     per-source-file INCLUDE_DIRECTORIES property -- didn't take
	#     effect at all (CI still hit the original Packet.h-not-found
	#     error), most likely a Visual Studio generator / CMP0118 quirk
	#     for sources absorbed into a target via a linked INTERFACE
	#     library's target_sources rather than added directly.
	#
	# Fix: a two-file forwarding shim (Client/Packet/SharedPacketsShim/)
	# containing only Packet.h/PacketFactory.h, each a one-line
	# #include "../Packet.h" / "../PacketFactory.h". Safe to add
	# target-wide -- it holds nothing else, so it cannot shadow anything,
	# and any other file that happened to quoted-include "Packet.h" would
	# just get correctly forwarded, not broken.
	target_include_directories(DarkEden PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/Client/Packet/SharedPacketsShim
	)

	# Link iconv only on platforms that actually use it.
	if(NOT EMSCRIPTEN AND NOT WIN32)
		target_link_libraries(DarkEden PRIVATE iconv)
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
		${CMAKE_CURRENT_SOURCE_DIR}/Client/framelib
#		${CMAKE_CURRENT_SOURCE_DIR}/tools/engine/sprite/include
#		${CMAKE_CURRENT_SOURCE_DIR}/tools/engine/ui/include
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

	if(WIN32)
		set_target_properties(DarkEden PROPERTIES WIN32_EXECUTABLE ON)
	endif()

	message(STATUS "DarkEden executable will be built from remaining Client files (VS_UI compiled separately)")
endif()

# Print configuration summary
message(STATUS "")
message(STATUS "=== Dark Eden Client Configuration ===")
message(STATUS "  Build Tests:      ${BUILD_TESTS}")
message(STATUS "  Use SDL Backend:  ${USE_SDL_BACKEND}")
message(STATUS "  Build Engine:     ${BUILD_ENGINE}")
message(STATUS "  C++ Standard:     ${CMAKE_CXX_STANDARD}")
message(STATUS "=======================================")
message(STATUS "")
