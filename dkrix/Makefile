# DarkEden Client Makefile — formatting entry points (Phase 10).
#
# The client builds with CMake + Visual Studio on Windows (see
# ../CLAUDE.md); this Makefile deliberately carries no build targets.
# It exists so `make fmt` / `make fmt-check` work the same way here as
# in dkrixserver/. Run it from dkrix/ under any POSIX make (WSL, Git
# Bash, CI's Ubuntu). Match CI's formatter version locally with:
#   pip install clang-format==18.1.8
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
#   Client/Platform/{ogg,...}.h Xiph ogg/vorbis API headers (moved from DXLib/, Phase 3 item 3)

.PHONY: fmt fmt-check fmt-check-all help

.DEFAULT_GOAL := help

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
	':(exclude)Client/zutil.h' \
	':(exclude)Client/Platform/ogg.h' \
	':(exclude)Client/Platform/vorbisenc.h' \
	':(exclude)Client/Platform/vorbisfile.h'

# One pathspec set drives every target below.
FMT_PATHSPECS := '*.cpp' '*.h' '*.CPP' '*.H' $(FMT_EXCLUDES)

FMT_LS   := git ls-files -- $(FMT_PATHSPECS)
FMT_LS_Z := git ls-files -z -- $(FMT_PATHSPECS)

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
	@echo "DarkEden Client Makefile targets (formatting only — the build"
	@echo "is CMake + Visual Studio, see ../CLAUDE.md):"
	@echo "  fmt           - Format all tracked C++ code"
	@echo "  fmt-check     - Check format for modified files only (fast)"
	@echo "  fmt-check-all - Check format for all files (slow)"
	@echo "  help          - Show this help message"
