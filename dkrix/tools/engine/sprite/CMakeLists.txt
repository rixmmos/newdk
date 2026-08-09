cmake_minimum_required(VERSION 3.10)

# Sprite engine static library
# Exposes SP.h as the public API header

find_package(SDL2 REQUIRED)

add_library(sprite STATIC
    src/animation.c
    src/color.c
    src/colorset.c
    src/frame.c
    src/framepack.c
    src/index_sprite.c
    src/index_spritepack.c
    src/sdl_framework.c
    src/shadow_sprite.c
    src/shadow_spritepack.c
    src/sprite.c
    src/spritepack.c
    src/ui_surface.c
    src/zone.c
)

# Public includes
target_include_directories(sprite
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against SDL2
if(TARGET SDL2::SDL2)
    target_link_libraries(sprite PUBLIC SDL2::SDL2)
else()
    target_link_libraries(sprite PUBLIC ${SDL2_LIBRARIES})
    target_include_directories(sprite PUBLIC ${SDL2_INCLUDE_DIRS})
endif()

set_property(TARGET sprite PROPERTY POSITION_INDEPENDENT_CODE ON)

# Install
install(TARGETS sprite
    ARCHIVE DESTINATION lib
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/SP.h
    DESTINATION include/sprite
)

# ============================================================
# Tests (Phase 4, 2026-08-08: wire the 11 test files under tests/ into CI)
# ============================================================
# This whole directory is only add_subdirectory()'d from ../../../CMakeLists.txt
# when BUILD_ENGINE is ON (always on non-Windows, always off on Windows/MSVC —
# see the CMakeLists.txt comment there), so no separate guard is needed here:
# reaching this point already means the engine and its tests should build.
# enable_testing() itself is called once in the root CMakeLists.txt, inside
# that same BUILD_ENGINE block, so `ctest` run from the top of the build tree
# discovers the add_test() calls below.

# test_main.c is a small shared PASS/FAIL test framework (tests_run /
# tests_passed / tests_failed counters, test_assert()/test_assert_eq()) whose
# main() calls one entry point per other test file (test_color_conversion,
# test_sprite_parsing, ...). Those 9 files are therefore compiled and linked
# together into a single combined binary, not run independently.
add_executable(sprite_tests
    tests/test_main.c
    tests/test_color.c
    tests/test_animation.c
    tests/test_frame.c
    tests/test_framepack.c
    tests/test_index_sprite.c
    tests/test_sdl_framework.c
    tests/test_shadow_sprite.c
    tests/test_sprite.c
    tests/test_spritepack.c
)
target_link_libraries(sprite_tests PRIVATE sprite)
add_test(NAME sprite_tests COMMAND sprite_tests)

# test_zone.c is the 11th file and deliberately NOT part of the combined
# binary above: it #include "zone.c" directly (the .c file, not just
# zone.h) to exercise it as a standalone translation unit, and defines its
# own main(). Linking it against the `sprite` library — which already
# compiles src/zone.c into its own object — would duplicate every zone_*
# symbol at link time. It needs nothing beyond zone.h/zone.c's own
# dependencies (stdlib, string, stdio, error.h — all header-only), so it
# gets its own executable and does not link the `sprite` target.
add_executable(sprite_test_zone tests/test_zone.c)
# src/ as well as include/: the #include "zone.c" above resolves against it.
# Without it the target fails at "zone.c: No such file or directory" -- which
# is exactly what happened the first time this leg ever ran (2026-08-09).
target_include_directories(sprite_test_zone PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME sprite_test_zone COMMAND sprite_test_zone)
