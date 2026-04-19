# DarkEden Client Makefile
#
# Minimal Makefile providing clang-format fmt / fmt-check targets
# to mirror the server tree. Build itself still goes through
# cmake directly; this file is format-only (for now).
#
# The source-tree directories covered below
# (Client, VS_UI, basic, tools) match what's actually compiled
# by CMakeLists.txt (and `list(FILTER EXCLUDE)` removes the
# Windows-only files from the build) — the fmt rules run over
# everything in those trees, including files currently excluded
# from the build, so the formatting stays consistent when they
# come back.

.PHONY: fmt fmt-check fmt-check-all help

# Directories clang-format should walk.
# Kept in sync with the `file(GLOB)` sites in CMakeLists.txt plus
# the other source roots that aren't globbed.
FMT_DIRS := Client VS_UI basic tools

# File extensions to format
FMT_EXTS := -name "*.cpp" -o -name "*.h" -o -name "*.hpp"

# Default target — show help rather than silently doing nothing
.DEFAULT_GOAL := help

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
	@echo "DarkEden Client Makefile targets (format-only for now):"
	@echo "  fmt           - Format all C++ code under $(FMT_DIRS)"
	@echo "  fmt-check     - Check format for modified files only (fast)"
	@echo "  fmt-check-all - Check format for all files (slow)"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Build targets still go through cmake directly:"
	@echo "  cmake -B build && cmake --build build -j"
	@echo "  cmake -B build -DUSE_ASAN=ON && cmake --build build -j"
