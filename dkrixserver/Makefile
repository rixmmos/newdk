# DarkEden Makefile

.PHONY: all fmt fmt-check fmt-check-all clean help \
        debug release debug-asan debug-tsan debug-ubsan

# Parallelism: honor MAKEFLAGS -j if the caller set it, otherwise
# auto-detect (sysctl on macOS, nproc on Linux, fallback to 4).
NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

# Default target
all: debug

release:
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build -j$(NPROC)

debug:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build -j$(NPROC)

# Sanitizer debug builds (Phase 10 bullet 3, 2026-08-07). Each uses a
# dedicated build directory so switching between configs doesn't force
# a CMakeCache invalidation, and `clean` below removes all of them.
# USE_<SAN>=ON options are wired in CMakeLists.txt. ASan and TSan are
# mutually exclusive (enforced there as a CMake FATAL_ERROR); there is
# no debug-asan+tsan combination target.
debug-asan:
	cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON
	cmake --build build-asan -j$(NPROC)

debug-tsan:
	cmake -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DUSE_TSAN=ON
	cmake --build build-tsan -j$(NPROC)

debug-ubsan:
	cmake -B build-ubsan -DCMAKE_BUILD_TYPE=Debug -DUSE_UBSAN=ON
	cmake --build build-ubsan -j$(NPROC)


# Format code with clang-format
fmt:
	@echo "Formatting C++ code with clang-format..."
	find src -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec clang-format -i {} +
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
	for file in $$(find src -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \)); do \
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

# Clean build artifacts (including per-sanitizer build dirs)
clean:
	rm -rf build build-asan build-tsan build-ubsan bin lib

# Show help message
help:
	@echo "DarkEden Makefile targets:"
	@echo "  all           - Build the project (default: debug)"
	@echo "  debug         - CMake debug build"
	@echo "  release       - CMake release build"
	@echo "  debug-asan    - Debug build with AddressSanitizer"
	@echo "  debug-tsan    - Debug build with ThreadSanitizer"
	@echo "  debug-ubsan   - Debug build with UndefinedBehaviorSanitizer"
	@echo "  fmt           - Format all C++ code"
	@echo "  fmt-check     - Check format for modified files only (fast)"
	@echo "  fmt-check-all - Check format for all files (slow)"
	@echo "  clean         - Clean build artifacts"
	@echo "  help          - Show this help message"
