# DarkEden Client Makefile
#
# Phase 10B shipped this as format-only. Phase 15C (2026-04-19)
# adds build targets — debug / release / debug-asan / debug-tsan
# / debug-ubsan — so CI and local dev can invoke canonical
# cmake incantations through `make` instead of remembering the
# flag set.
#
# The source-tree directories covered below
# (Client, VS_UI, basic, tools) match what's actually compiled
# by CMakeLists.txt (and `list(FILTER EXCLUDE)` removes the
# Windows-only files from the build) — the fmt rules run over
# everything in those trees, including files currently excluded
# from the build, so the formatting stays consistent when they
# come back.

.PHONY: all fmt fmt-check fmt-check-all clean help \
        debug release debug-asan debug-tsan debug-ubsan

# Parallelism: honor MAKEFLAGS -j if the caller set it, otherwise
# auto-detect (sysctl on macOS, nproc on Linux, fallback to 4).
NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

# Directories clang-format should walk.
# Kept in sync with the `file(GLOB)` sites in CMakeLists.txt plus
# the other source roots that aren't globbed.
FMT_DIRS := Client VS_UI basic tools

# File extensions to format
FMT_EXTS := -name "*.cpp" -o -name "*.h" -o -name "*.hpp"

# Default target — `make` alone runs a debug build, matching the
# server tree convention.
.DEFAULT_GOAL := debug

# ----------------------------------------------------------------
# Build targets (Phase 15C, 2026-04-19)
# ----------------------------------------------------------------
# All builds land under their own build directory so switching
# between sanitizer configs doesn't force a full reconfigure
# every time.

all: debug

release:
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j$(NPROC)

debug:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build -j$(NPROC)

# Sanitizer debug builds — USE_<SAN>=ON options are wired in
# dkrix/CMakeLists.txt:28-56.
debug-asan:
	cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON
	cmake --build build-asan -j$(NPROC)

debug-tsan:
	cmake -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DUSE_TSAN=ON
	cmake --build build-tsan -j$(NPROC)

debug-ubsan:
	cmake -B build-ubsan -DCMAKE_BUILD_TYPE=Debug -DUSE_UBSAN=ON
	cmake --build build-ubsan -j$(NPROC)

# Clean build artifacts (including per-sanitizer build dirs)
clean:
	rm -rf build build-asan build-tsan build-ubsan bin lib

# ----------------------------------------------------------------
# Format targets (Phase 10B)
# ----------------------------------------------------------------

# Format code with clang-format
fmt:
	@echo "Formatting C++ code with clang-format..."
	find $(FMT_DIRS) -type f \( $(FMT_EXTS) \) -exec clang-format -i {} +
	@echo "Code formatted successfully!"

# Check format for modified files only (fast)
fmt-check:
	@echo "Checking format for modified files..."
	@failed=false; \
	files=$$(git diff --name-only --diff-filter=ACM | grep -E '\.(cpp|h|hpp)$$' || true); \
	if [ -z "$$files" ]; then \
		echo "No modified C++ files to check."; \
	else \
		for file in $$files; do \
			if [ -f "$$file" ]; then \
				if ! clang-format "$$file" | diff -q "$$file" - > /dev/null 2>&1; then \
					echo "[FAIL] $$file needs formatting"; \
					failed=true; \
				fi; \
			fi; \
		done; \
	fi; \
	if $$failed; then \
		echo ""; \
		echo "Run 'make fmt' to fix formatting issues"; \
		exit 1; \
	fi; \
	echo "[OK] All modified files are properly formatted!"

# Check format for all files (slow)
fmt-check-all:
	@echo "Checking format for ALL files..."
	@failed=false; \
	for file in $$(find $(FMT_DIRS) -type f \( $(FMT_EXTS) \)); do \
		if ! clang-format "$$file" | diff -q "$$file" - > /dev/null 2>&1; then \
			echo "[FAIL] $$file needs formatting"; \
			failed=true; \
		fi; \
	done; \
	if $$failed; then \
		echo ""; \
		echo "Run 'make fmt' to fix formatting issues"; \
		exit 1; \
	fi; \
	echo "[OK] All files are properly formatted!"

# Show help message
help:
	@echo "DarkEden Client Makefile targets:"
	@echo "  all           - Build the project (default: debug)"
	@echo "  debug         - CMake debug build"
	@echo "  release       - CMake release build"
	@echo "  debug-asan    - Debug build with AddressSanitizer"
	@echo "  debug-tsan    - Debug build with ThreadSanitizer"
	@echo "  debug-ubsan   - Debug build with UndefinedBehaviorSanitizer"
	@echo "  fmt           - Format all C++ code under $(FMT_DIRS)"
	@echo "  fmt-check     - Check format for modified files only (fast)"
	@echo "  fmt-check-all - Check format for all files (slow)"
	@echo "  clean         - Clean build artifacts"
	@echo "  help          - Show this help message"
