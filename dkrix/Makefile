# DarkEden Client Makefile — formatting entry points (Phase 10).
#
# The client builds with CMake + Visual Studio on Windows (see
# ../CLAUDE.md) — that stays the primary, authoritative build path and
# this Makefile still carries no generic all/debug/release/clean
# targets for it. It exists so `make fmt` / `make fmt-check` work the
# same way here as in dkrixserver/. Run it from dkrix/ under any POSIX
# make (WSL, Git Bash, CI's Ubuntu). Match CI's formatter version
# locally with:
#   pip install clang-format==18.1.8
#
# Phase 10 bullet 3 (2026-08-07) adds debug-asan / debug-tsan /
# debug-ubsan targets — a secondary, non-authoritative sanitizer path
# on Linux/macOS (per ../dkrix/CLAUDE.md: "macOS is a historical
# sanitizer path", "Linux is a secondary portability target"). They
# configure straight through CMake's USE_<SAN>=ON options
# (CMakeLists.txt) and are unrelated to the MSVC build above.
#
# File selection uses `git ls-files`, not `find`, so untracked build
# output and editor detritus can never reach a formatting pass. Legacy
# DOS-era sources use uppercase extensions, so .CPP/.H are matched
# alongside .cpp/.h. Tracked *.c files (vendored zlib, C tooling) and
# the lone *.inl are deliberately left out.
#
# Vendored third-party code is excluded — reformatting it would
# destroy diffability against upstream:
#   Client/JpegLib/            IJG libjpeg public headers
#   Client/{zlib,zconf,...}.h  zlib 1.1.x, vendored flat into Client/
#
# The Xiph ogg/vorbis API headers that lived here (ogg.h, vorbisenc.h,
# vorbisfile.h) were deleted as dead code — same evidence standard as
# a107d63 (no CMake source-list membership, zero external includers,
# self-referencing cluster only). See git history for the exclude
# entries this comment used to describe.

.PHONY: fmt fmt-check fmt-check-all help \
        debug-asan debug-tsan debug-ubsan clean-sanitizers

.DEFAULT_GOAL := help

# Parallelism: honor MAKEFLAGS -j if the caller set it, otherwise
# auto-detect (sysctl on macOS, nproc on Linux, fallback to 4).
NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

# Vendored third-party files, as git pathspec excludes. Keep this list
# in sync with the comment above when vendored code moves.
FMT_EXCLUDES := \
	':(exclude)Client/JpegLib' \
	':(exclude)Client/deflate.h' \
	':(exclude)Client/infblock.h' \
	':(exclude)Client/infcodes.h' \
	':(exclude)Client/inffast.h' \
	':(exclude)Client/inffixed.h' \
	':(exclude)Client/inftrees.h' \
	':(exclude)Client/infutil.h' \
	':(exclude)Client/trees.h' \
	':(exclude)Client/zconf.h' \
	':(exclude)Client/zlib.h' \
	':(exclude)Client/zutil.h'

# One pathspec set drives every target below.
FMT_PATHSPECS := '*.cpp' '*.h' '*.CPP' '*.H' $(FMT_EXCLUDES)

FMT_LS   := git ls-files -- $(FMT_PATHSPECS)
FMT_LS_Z := git ls-files -z -- $(FMT_PATHSPECS)

# Sanitizer debug builds (Phase 10 bullet 3, 2026-08-07) — Linux/macOS
# only, not the Windows/MSVC primary path. Each uses a dedicated build
# directory so switching between configs doesn't force a CMakeCache
# invalidation. USE_<SAN>=ON options are wired in CMakeLists.txt; ASan
# and TSan are mutually exclusive there (CMake FATAL_ERROR).
debug-asan:
	cmake -B build-asan -DCMAKE_BUILD_TYPE=Debug -DUSE_ASAN=ON
	cmake --build build-asan -j$(NPROC)

debug-tsan:
	cmake -B build-tsan -DCMAKE_BUILD_TYPE=Debug -DUSE_TSAN=ON
	cmake --build build-tsan -j$(NPROC)

debug-ubsan:
	cmake -B build-ubsan -DCMAKE_BUILD_TYPE=Debug -DUSE_UBSAN=ON
	cmake --build build-ubsan -j$(NPROC)

# Remove only the sanitizer build directories — leaves the Windows/MSVC
# `build/` tree (and anything else under here) untouched, since this
# Makefile does not own the primary build.
clean-sanitizers:
	rm -rf build-asan build-tsan build-ubsan

# Format all tracked C++ sources in place.
fmt:
	@echo "Formatting C++ code with clang-format..."
	@$(FMT_LS_Z) | xargs -0 clang-format -i
	@echo "Code formatted successfully!"

# Check format for modified files only (fast). `--relative` is
# required: without it git prints paths from the repo root
# (dkrix/Client/...), which do not resolve from this directory and
# every file would be silently skipped.
fmt-check:
	@echo "Checking format for modified files..."
	@failed=false; \
	files=$$(git diff --name-only --diff-filter=ACM --relative -- $(FMT_PATHSPECS) || true); \
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

# Check format for all files (slow).
fmt-check-all:
	@echo "Checking format for ALL files..."
	@failed=false; \
	for file in $$($(FMT_LS)); do \
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
	@echo "DarkEden Client Makefile targets (the authoritative build is"
	@echo "CMake + Visual Studio, see ../CLAUDE.md — everything below is"
	@echo "formatting plus a secondary Linux/macOS sanitizer path):"
	@echo "  fmt              - Format all tracked C++ code"
	@echo "  fmt-check        - Check format for modified files only (fast)"
	@echo "  fmt-check-all    - Check format for all files (slow)"
	@echo "  debug-asan       - Linux/macOS debug build with AddressSanitizer"
	@echo "  debug-tsan       - Linux/macOS debug build with ThreadSanitizer"
	@echo "  debug-ubsan      - Linux/macOS debug build with UndefinedBehaviorSanitizer"
	@echo "  clean-sanitizers - Remove build-asan/build-tsan/build-ubsan"
	@echo "  help             - Show this help message"
