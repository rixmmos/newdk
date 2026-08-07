cmake_minimum_required(VERSION 3.10)

# DXLib Library - DirectX abstraction with backend selection
# Supports both Windows DirectX and SDL2 backends

project(dxlib)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection and backend selection
if(WIN32)
	option(USE_SDL_BACKEND "Use SDL2 backend instead of DirectX" OFF)

	if(USE_SDL_BACKEND)
		add_definitions(-DDXLIB_USE_SDL_BACKEND)
		message(STATUS "DXLib: Using SDL2 backend")
		find_package(SDL2 REQUIRED)
		find_package(SDL2_mixer)
	else()
		message(STATUS "DXLib: Using native Windows DirectX backend")
		# DirectX SDK paths - user may need to set these
		# set(DXSDK_DIR "$ENV{DXSDK_DIR}" CACHE PATH "DirectX SDK Directory")
		# include_directories(${DXSDK_DIR}/Include)
	endif()
endif()

# Non-Windows platforms must use SDL
if(NOT WIN32)
	add_definitions(-DDXLIB_USE_SDL_BACKEND)
	# Define platform-specific macros
	if(APPLE)
		add_definitions(-DPLATFORM_MACOS)
	endif()
	find_package(SDL2 REQUIRED)
	find_package(SDL2_mixer)
	message(STATUS "DXLib: Using SDL2 backend (required on this platform)")
endif()

# Check if SDL2_mixer is available
if(TARGET SDL2::SDL2_mixer OR SDL2_MIXER_LIBRARIES)
	set(HAVE_SDL2_MIXER TRUE)
	message(STATUS "DXLib: SDL2_mixer found - audio support enabled")
	add_definitions(-DHAVE_SDL2_MIXER)
else()
	set(HAVE_SDL2_MIXER FALSE)
	message(STATUS "DXLib: SDL2_mixer NOT found - audio support disabled")
endif()

# DXLib source files
set(DXLIB_SOURCES
	# Platform backend
	DXLibBackendSDL.cpp

	# Core graphics/sound/music implementations
	CSDLGraphics.cpp
	CDirectDrawSurface.cpp
	AudioManager.cpp
	CDirectMusic.cpp
	CDirectSoundStream.cpp

	# Static member definitions for CSDLGraphics
	CSDLGraphics_StaticMembers.cpp

	# Input manager (SDL2-backed)
	InputManager.cpp
)

# Phase 3 item 1 (2026-08): the CDirect{Sound,Music,SoundStream}_Adapter.cpp
# files formerly appended here were duplicate implementations of the same
# classes as AudioManager.cpp / CDirectMusic.cpp / CDirectSoundStream.cpp.
# They landed in DXLIB_SOURCES whenever HAVE_SDL2_MIXER was on, which never
# actually built: CDirectSound_Adapter.cpp and CDirectSoundStream_Adapter.cpp
# had ctor/dtor names left over from before a rename (e.g.
# `AudioManager::CDirectSound()`, not a valid member of that class), and
# CDirectMusic_Adapter.cpp duplicated every symbol in CDirectMusic.cpp,
# which would have been a link error even after fixing the names. Deleted;
# see docs/MODERNIZATION.md Phase 3.

# Add Windows-specific sources if using Windows backend
if(WIN32 AND NOT USE_SDL_BACKEND)
	# Add original DXLib files here (DirectX backend)
	# For now, we only support SDL backend in the adapter
	message(WARNING "Windows DirectX backend not yet implemented. Please use USE_SDL_BACKEND=ON")
endif()

# Create library
add_library(dxlib STATIC ${DXLIB_SOURCES})

# Public include directories
target_include_directories(dxlib
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/basic  # For platform abstraction
)

# Link SDL2 if using SDL backend
if(USE_SDL_BACKEND OR NOT WIN32)
	if(TARGET SDL2::SDL2)
		target_link_libraries(dxlib PUBLIC SDL2::SDL2)
	else()
		target_link_libraries(dxlib PUBLIC ${SDL2_LIBRARIES})
		target_include_directories(dxlib PUBLIC ${SDL2_INCLUDE_DIRS})
	endif()

	# Link SDL2_mixer if available
	if(HAVE_SDL2_MIXER)
		if(TARGET SDL2::SDL2_mixer)
			target_link_libraries(dxlib PUBLIC SDL2::SDL2_mixer)
		else()
			target_link_libraries(dxlib PUBLIC ${SDL2_MIXER_LIBRARIES})
			target_include_directories(dxlib PUBLIC ${SDL2_MIXER_INCLUDE_DIRS})
		endif()
	endif()
endif()

# Link basic library for platform abstraction
target_link_libraries(dxlib PUBLIC basic)

# Windows-specific libraries
if(WIN32 AND NOT USE_SDL_BACKEND)
	target_link_libraries(dxlib PUBLIC
		dinput8
		dsound
		dxguid
		winmm
	)
endif()

# Installation
install(TARGETS dxlib
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
)

# Install headers
install(FILES
	DXLibBackend.h
	DXLib.h
	InputManager.h
	AudioManager.h
	CDirectMusic.h
	CDirectSoundStream.h
	DESTINATION include/dxlib
)

# Build options message
message(STATUS "")
message(STATUS "=== DXLib Configuration ===")
message(STATUS "  Use SDL Backend:     ${USE_SDL_BACKEND}")
message(STATUS "  SDL2_mixer Support:  ${HAVE_SDL2_MIXER}")
message(STATUS "  C++ Standard:        ${CMAKE_CXX_STANDARD}")
message(STATUS "================================")
message(STATUS "")
