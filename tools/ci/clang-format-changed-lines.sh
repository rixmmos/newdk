#!/usr/bin/env bash
#
# Changed-lines clang-format check.
#
# Reads NUL-free file paths (one per line) on stdin, relative to the current
# working directory, and checks ONLY the lines each file changed in
# "$BASE..HEAD" -- not the whole file.
#
# Why not whole-file [2026-08-09]: the client tree carries a measured backlog
# of 2,199 unformatted legacy sources, several with four-figure drift
# (basic/Platform.h 1,683 lines; Client/UIMessageManager.cpp 18,350, whose
# .clang-format SortIncludes + IncludeBlocks: Regroup would reorder ~300
# includes in a file demonstrably sensitive to include order). Under a
# whole-file ratchet every one-line bug fix in a legacy file trips the gate,
# and the only ways out are a risky bulk reformat or a carve-out list. Both
# are worse than the thing they enforce. Ratcheting on changed lines keeps
# "new code is clean" without charging a reformat as the price of a bug fix.
#
# This is what upstream's git-clang-format does; it is reimplemented here
# rather than vendored so CI does not depend on fetching a script out of
# llvm-project at run time.
#
# Usage:  git diff --name-only ... | BASE=<sha> tools/ci/clang-format-changed-lines.sh <label>
# Exit:   0 all clean (or nothing to check), 1 a file needs formatting.

set -u -o pipefail

label="${1:-files}"

if [ -z "${BASE:-}" ]; then
	echo "::error::BASE is unset; cannot determine the changed-line range."
	exit 1
fi

checked=0
listed=0
failed=0

while IFS= read -r f; do
	[ -z "$f" ] && continue
	listed=$((listed + 1))

	if [ ! -f "$f" ]; then
		# Deleted, or renamed away, between BASE and HEAD. --diff-filter=ACM
		# should already exclude these; treat it as a warning, not a failure.
		echo "::warning file=$f::listed as changed but not present in the worktree; skipped"
		continue
	fi

	# Collect the added/modified line ranges on the NEW side, from the
	# hunk headers of a zero-context diff: "@@ -a,b +c,d @@".
	# d==0 means a pure deletion -- no new lines to check, so skip it.
	ranges=$(git diff -U0 "$BASE" -- "$f" |
		sed -n 's/^@@ -[0-9,]* +\([0-9]*\)\(,\([0-9]*\)\)\? @@.*/\1 \3/p' |
		while read -r start count; do
			count="${count:-1}"
			[ "$count" -eq 0 ] && continue
			echo "--lines=$start:$((start + count - 1))"
		done)

	if [ -z "$ranges" ]; then
		# Whitespace-only or deletion-only change: nothing added to police.
		continue
	fi

	checked=$((checked + 1))

	# Compare ONLY within the changed ranges [fixed 2026-08-09].
	#
	# clang-format does not honour --lines for preprocessor-directive
	# indentation: with IndentPPDirectives: None it renormalises every "#"
	# directive in the file no matter how narrow the range. Measured on
	# basic/Platform.h -- `clang-format --lines=425:427` also rewrites lines
	# 726, 728 and 730. A plain whole-file diff therefore blamed each author
	# for directives they never touched, and since fixing those shifted the
	# next range, the check never converged: three separate sessions hit it
	# (one watched the range grow for eight passes), and it made Platform.h
	# uneditable under the gate.
	#
	# So report a hunk only when a line the author actually changed is the
	# one clang-format disagrees about. This is what the header comment above
	# has always promised; the whole-file diff simply did not implement it.
	# shellcheck disable=SC2086 -- $ranges is a deliberately word-split flag list
	offending=$(diff -u "$f" <(clang-format $ranges "$f") |
		awk -v ranges="$ranges" '
			function inrange(L,   i) {
				for (i = 1; i <= m; i++)
					if (L >= lo[i] && L <= hi[i]) return 1
				return 0
			}
			function flush() {
				if (inhunk && hit) printf "%s\n%s", hdr, body
			}
			BEGIN {
				n = split(ranges, r, /[[:space:]]+/)
				for (i = 1; i <= n; i++)
					if (match(r[i], /^--lines=[0-9]+:[0-9]+$/)) {
						split(substr(r[i], 9), p, ":")
						lo[++m] = p[1]; hi[m] = p[2]
					}
			}
			/^(---|\+\+\+)/ { next }
			/^@@ / {
				# "@@ -a,b +c,d @@" -- a,b is the range in the ORIGINAL file.
				# Walk the original-side line number through the hunk body so
				# the test is per line, not per hunk: one hunk routinely spans
				# both a line the author changed and unrelated directives
				# clang-format wants reindented, and blaming the author for
				# the latter is the bug being fixed here.
				flush()
				hdr = $0; body = ""; hit = 0; inhunk = 1
				split($2, o, ",")                  # o[1] = "-a", o[2] = b
				cur = substr(o[1], 2) + 0
				next
			}
			inhunk {
				body = body $0 "\n"
				if (substr($0, 1, 1) == "-")      { if (inrange(cur)) hit = 1; cur++ }
				else if (substr($0, 1, 1) == "+") { if (inrange(cur) || inrange(cur - 1)) hit = 1 }
				else                              { cur++ }
			}
			END { flush() }
		')

	if [ -n "$offending" ]; then
		echo "::error file=$f::Lines changed in this push are not clang-format clean."
		# Show only the offending hunks; a whole-file diff on a legacy file
		# is thousands of lines and buries the actual problem.
		printf '%s\n' "$offending" | head -40
		failed=1
	fi
done

echo "Checked changed lines in $checked of $listed $label."

# Same guard the whole-file gates grew in 60f4c35: a non-empty change list
# that inspects nothing is a bug in this script's path resolution, not a pass.
if [ "$listed" -gt 0 ] && [ "$checked" -eq 0 ] && [ "$failed" -eq 0 ]; then
	echo "::notice::$listed file(s) changed but none had added lines to check (deletions/whitespace only)."
fi

exit $failed
