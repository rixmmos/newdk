#!/usr/bin/env python3
# normalize-packet-style.py — Phase 12 packet style-normalization prototype.
#
# Companion to audit-packet-divergence.sh (parked line, tag
# archive/modernization-phases-1-17) and to the 2026-08-07 re-run
# documented in docs/packet-divergence-2026-08-07.md. Where the audit
# CLASSIFIES pairs, this script NORMALIZES both sides of one packet
# class pair to a canonical style and reports the residual diff plus a
# wire-shape verdict, answering the Phase 12 sizing question: which of
# the 163 duplicated packet classes are style-only twins that one
# scripted pass can reconcile, and which carry real protocol
# divergence needing manual review.
#
# It builds on, and supersedes for this purpose, both earlier
# normalizers:
#   - the parked audit script's sed rules (throw(...) specs, the
#     client-only Client_PCH.h include, the __DEBUG_OUTPUT__ /
#     __GAME_CLIENT__ guard LINES, whitespace) — kept, but the guard
#     handling is now preprocessor-stack-aware (guard lines fold away,
#     guarded BODIES are kept and compared), and the PCH match is
#     whitespace-insensitive, immune to the CRLF working-tree issue
#     the re-run documented;
#   - the re-run's normalize.py (strip \r, // and /* */ comments,
#     trailing whitespace, blank-run collapse) — reproduced VERBATIM
#     as normalize_audit() so this script can regenerate the numbers
#     in docs/packet-divergence-2026-08-07.md (median 157, max
#     CLRegisterPlayer 747) for continuity checks: --audit-check.
#
# Canonical style (what both sides are normalized TOWARD — the target
# the eventual rewrite pass converges on; rationale in
# docs/packet-normalization-sample-2026-08-07.md):
#   layout   server's .clang-format (LLVM base, 4-space, no tabs,
#            attached braces, pointer-left) — the only machine-
#            enforced style in the repo (make fmt-check, CI);
#   specs    no dynamic-exception (throw) specifications — server
#            already dropped them; deprecated in C++11;
#   includes flat "X.h" form (server), include block sorted;
#   guards   the client-only Client_PCH.h include and the
#            __DEBUG_OUTPUT__ / __GAME_CLIENT__ guards are treated as
#            fold-into-unified-file lines: the unified file keeps
#            them, so for comparison the guard LINES are removed and
#            the guarded CONTENT is kept and compared. Same treatment
#            for the Handler-dispatch call in execute() (guarded vs
#            deleted client-side) and for the Assert.h/Assert1.h
#            include rename (see _INCLUDE_EQUIV / the dispatch fold
#            below for the measurements behind each).
# Comparison-only equivalences (applied to both sides symmetrically;
# they express "the rewrite pass may pick either spelling without
# touching the wire", not a rewrite direction):
#   string == std::string (generally: Foo::x == x, qualifier strip);
#   virtual keyword presence on overrides;
#   empty ctor/dtor (body at most __BEGIN_TRY/__END_CATCH) == absent;
#   statement layout (join/explode to one statement per line);
#   inter-token whitespace.
#
# The VERDICT is deliberately not driven by the text residual alone.
# For each side the script extracts a wire signature from the
# normalized .cpp/.h:
#   read_seq   ordered (op, args) of every read()/readEncrypt() call
#              in the read() body;
#   write_seq  same for write()/writeEncrypt() in write();
#   size_expr  getPacketSize() return expression, canonicalized
#              commutatively (sorted +-terms, sorted *-factors) —
#              wire-relevant: the client writes getPacketSize() into
#              the outgoing packet header (dkrix Packet.h,
#              writeHeaderNBody: oStream.write(getPacketSize())).
#   maxsize_expr getPacketMaxSize() likewise (factory) — reported as
#              a warning only: the client factory is compiled out in
#              real client builds (__DEBUG_OUTPUT__), so a mismatch
#              is live-vs-dead code, not live wire disagreement.
# verdict:
#   style-only      residual 0 on both files (rules fully reconcile);
#   style-residual  residual > 0 but read_seq, write_seq and
#                   size_expr all match — textual leftovers the rules
#                   don't cover yet, wire-equivalent as measured;
#   real-divergence read_seq, write_seq or size_expr differ — needs
#                   protocol review, echoing the CLLogin finding.
# A real-divergence flag is a claim about MEASURED wire signatures,
# not a proof of behavioral difference; a style-residual flag is NOT
# a proof of wire equality outside the three measured signatures.
# Anything unusual (guard #else, unbalanced guards, missing method
# bodies) lands in notes[] and the pair is never silently upgraded.
#
# Deliberately NOT wired into CI (like the parked audit script): it is
# an investigation and batch-planning tool. The long-term scoreboard
# for the actual migration is check-packet-duplicates.sh (ratchet,
# baseline 326).
#
# Usage:
#   python3 dkrixserver/scripts/normalize-packet-style.py --list
#   python3 dkrixserver/scripts/normalize-packet-style.py --pair CGReady [--emit DIR]
#   python3 dkrixserver/scripts/normalize-packet-style.py --all [--tsv]
#   python3 dkrixserver/scripts/normalize-packet-style.py --audit-check
#
# Output: one JSON object per pair (or TSV with --tsv). --emit writes
# <pair>.{server,client}.{h,cpp}.norm plus <pair>.residual.diff into
# DIR for eyeball review. Trees are read-only; nothing under
# dkrixserver/src/Core/ or dkrix/Client/Packet/ is ever written.
#
# Exit status: 0 ok; 2 usage error / tree not found.

import argparse
import difflib
import json
import os
import re
import statistics
import sys

NAME_RE = re.compile(r"^(CG|GC|CL|LC|GS|SG|GT|TG)[A-Za-z0-9_]*\.(cpp|h)$")

# ---------------------------------------------------------------------------
# normalize_audit — VERBATIM from docs/packet-divergence-2026-08-07.md
# (appendix). Do not "improve": its only job is to reproduce the doc's
# numbers. Known, documented naivety: the // regex is not
# string-literal-aware.
# ---------------------------------------------------------------------------


def _audit_strip_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
    return re.sub(r"//[^\n]*", "", text)


def normalize_audit(text):
    text = _audit_strip_comments(text.replace("\r", ""))
    text = re.sub(r"throw\s*\([^)]*\)", "", text)
    lines = [l.rstrip() for l in text.split("\n")]
    out, prev_blank = [], False
    for l in lines:
        blank = l == ""
        if blank and prev_blank:
            continue
        out.append(l)
        prev_blank = blank
    while out and out[0] == "":
        out.pop(0)
    while out and out[-1] == "":
        out.pop()
    return "\n".join(out) + "\n"


# ---------------------------------------------------------------------------
# String-literal-aware scanning helpers (shared by the style normalizer).
# ---------------------------------------------------------------------------


def strip_comments_aware(text):
    """Remove // and /* */ comments, respecting string/char literals."""
    out = []
    i, n = 0, len(text)
    state = "code"  # code | str | chr | line | block
    while i < n:
        c = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if state == "code":
            if c == "/" and nxt == "/":
                state = "line"
                i += 2
                continue
            if c == "/" and nxt == "*":
                state = "block"
                i += 2
                continue
            if c == '"':
                state = "str"
            elif c == "'":
                state = "chr"
            out.append(c)
            i += 1
        elif state in ("str", "chr"):
            out.append(c)
            if c == "\\" and nxt:
                out.append(nxt)
                i += 2
                continue
            if (state == "str" and c == '"' and text[i] == '"' and out[-1] == '"') or (
                state == "chr" and c == "'"
            ):
                # closing quote just emitted (opening handled in code state)
                pass
            if (state == "str" and c == '"') or (state == "chr" and c == "'"):
                state = "code"
            i += 1
        elif state == "line":
            if c == "\n":
                out.append(c)
                state = "code"
            i += 1
        elif state == "block":
            if c == "*" and nxt == "/":
                out.append(" ")
                state = "code"
                i += 2
                continue
            if c == "\n":
                out.append(c)  # keep line structure
            i += 1
    return "".join(out)


def _scan_spans(line):
    """Yield (start, end, in_string) spans for a single line."""
    spans = []
    i, n = 0, len(line)
    start = 0
    state = "code"
    quote = ""
    while i < n:
        c = line[i]
        if state == "code" and c in "\"'":
            if i > start:
                spans.append((start, i, False))
            start = i
            state = "lit"
            quote = c
            i += 1
            continue
        if state == "lit":
            if c == "\\":
                i += 2
                continue
            if c == quote:
                spans.append((start, i + 1, True))
                start = i + 1
                state = "code"
        i += 1
    if start < n:
        spans.append((start, n, state == "lit"))
    return spans


def apply_outside_literals(line, fn):
    """Apply fn to the non-string-literal spans of a line."""
    return "".join(
        seg if lit else fn(seg) for a, b, lit in _scan_spans(line) for seg in [line[a:b]]
    )


# ---------------------------------------------------------------------------
# Style normalizer.
# ---------------------------------------------------------------------------

_FOLD_GUARDS = {"#ifdef __DEBUG_OUTPUT__", "#ifndef __GAME_CLIENT__"}
_PCH_INCLUDE = '#include "Client_PCH.h"'


def _collapse_directive(line):
    return re.sub(r"\s+", " ", line.strip())


def fold_guards(lines, notes):
    """Drop the two client-only guard directives (and their matching
    #endif), keeping the guarded content. Preprocessor-stack-aware, so
    nested guards and the include-guard #endif survive intact."""
    out = []
    stack = []  # True = folded guard, False = other #if*
    for line in lines:
        d = _collapse_directive(line)
        if d.startswith("#if"):
            folded = d in _FOLD_GUARDS
            stack.append(folded)
            if folded:
                continue
        elif d.startswith("#elif") or d == "#else":
            if stack and stack[-1]:
                notes.append("guard-else: #else/#elif inside a folded guard "
                             "(semantics differ per build; kept in residual)")
                out.append(line)
                continue
        elif d == "#endif" or d.startswith("#endif"):
            if stack:
                if stack.pop():
                    continue
            else:
                notes.append("unbalanced #endif")
        elif d == _PCH_INCLUDE:
            continue
        out.append(line)
    if stack:
        notes.append("unbalanced #if at EOF")
    return out


# Include-name equivalences: same role, same content, different name
# per tree. Canonical = the server name (unified files land server-side).
# Assert.h == Assert1.h: both define the identical Assert(expr) macro
# (no-op in release, __assert__ in debug); the server copy was renamed.
_INCLUDE_EQUIV = {
    '#include "Assert.h"': '#include "Assert1.h"',
}


def canonical_includes(lines):
    """Strip ../ from local includes; apply include-name equivalences;
    sort+dedupe the include lines in place of the first include."""
    inc, other, first_idx = [], [], None
    for idx, line in enumerate(lines):
        d = _collapse_directive(line)
        if d.startswith("#include"):
            d = re.sub(r'#include\s+"(?:\.\./)+', '#include "', d)
            d = _INCLUDE_EQUIV.get(d, d)
            inc.append(d)
            if first_idx is None:
                first_idx = len(other)
        else:
            other.append(line)
    if first_idx is None:
        return lines
    inc = sorted(set(inc))
    return other[:first_idx] + inc + other[first_idx:]


def join_explode(lines):
    """Canonical statement layout: within each run of non-preprocessor
    lines, join everything, then break after ';' (at paren depth 0),
    after '{', after '}', and after access-specifier labels. String-
    literal-aware."""
    out = []
    buf = []

    def flush():
        if not buf:
            return
        blob = " ".join(buf)
        buf.clear()
        cur = []
        depth = 0
        state = "code"
        quote = ""
        i, n = 0, len(blob)
        while i < n:
            c = blob[i]
            cur.append(c)
            if state == "lit":
                if c == "\\" and i + 1 < n:
                    cur.append(blob[i + 1])
                    i += 2
                    continue
                if c == quote:
                    state = "code"
                i += 1
                continue
            if c in "\"'":
                state = "lit"
                quote = c
            elif c in "([":
                depth += 1
            elif c in ")]":
                depth = max(0, depth - 1)
            elif c == ";" and depth == 0:
                out.append("".join(cur))
                cur = []
            elif c == "{":
                out.append("".join(cur))
                cur = []
            elif c == "}":
                # '}' gets its own line so structural passes can track
                # brace depth on bare lines.
                cur.pop()
                if "".join(cur).strip():
                    out.append("".join(cur))
                out.append("}")
                cur = []
            elif c == ":" and depth == 0:
                prev = "".join(cur).rstrip()
                nxt = blob[i + 1] if i + 1 < n else ""
                if nxt != ":" and not prev.endswith("::"):
                    m = re.search(r"(\w+)\s*:$", prev)
                    if m and m.group(1) in ("public", "private", "protected"):
                        out.append("".join(cur))
                        cur = []
            i += 1
        if cur and "".join(cur).strip():
            out.append("".join(cur))

    for line in lines:
        if line.lstrip().startswith("#"):
            flush()
            out.append(line)
        else:
            buf.append(line)
    flush()
    return [l.strip() for l in out if l.strip()]


def tighten(line):
    """Collapse whitespace runs and remove space adjacent to
    punctuation, outside string literals."""
    if line.lstrip().startswith("#"):
        return _collapse_directive(line)

    def fn(seg):
        seg = re.sub(r"\s+", " ", seg)
        seg = re.sub(r"\s*([^\w\s]+)\s*", r"\1", seg)
        return seg

    return apply_outside_literals(line, fn).strip()


def comparison_rewrites(line):
    """Symmetric comparison-only equivalences: drop 'virtual', strip
    qualifier prefixes (Foo:: — subsumes std::), outside literals."""

    def fn(seg):
        seg = re.sub(r"\bvirtual\b ?", "", seg)
        seg = re.sub(r"\b[A-Za-z_]\w*::", "", seg)
        return seg

    if line.startswith("#"):
        return line
    return apply_outside_literals(line, fn)


_TRIVIAL_BODY = {"__BEGIN_TRY", "__END_CATCH"}


def drop_empty_ctor_dtor(lines, pair):
    """Remove destructor declarations, and ctor/dtor definitions whose
    body is empty or only __BEGIN_TRY/__END_CATCH. Ctor declarations
    and any ctor with an init list or real body are kept."""
    out = []
    i, n = 0, len(lines)
    ctor_def = re.compile(r"^%s\(\)\{$" % re.escape(pair))
    dtor_def = re.compile(r"^~\w+\(\)\{$")
    dtor_decl = re.compile(r"^~\w+\(\);$")
    while i < n:
        line = lines[i]
        if dtor_decl.match(line):
            i += 1
            continue
        if ctor_def.match(line) or dtor_def.match(line):
            j = i + 1
            depth = 1
            body = []
            while j < n and depth > 0:
                if lines[j] == "{":
                    depth += 1
                elif lines[j] == "}":
                    depth -= 1
                    if depth == 0:
                        break
                else:
                    body.append(lines[j])
                j += 1
            body_toks = [t for b in body for t in b.split()]
            if j < n and all(t in _TRIVIAL_BODY for t in body_toks):
                i = j + 1
                if i < n and lines[i] == ";":
                    i += 1
                continue
        out.append(line)
        i += 1
    return out


def normalize_style(text, pair, notes, fold=True):
    """Full pipeline with fold=True. fold=False disables every rule
    that REMOVES content (guard folds, include canonicalization, empty
    ctor/dtor, dispatch line, empty access labels) and keeps only the
    reversible layout/spelling rules — used by --selfcheck to prove the
    fold rules never touch wire-describing text."""
    text = text.replace("\r", "")
    text = strip_comments_aware(text)
    lines = text.split("\n")
    if fold:
        lines = fold_guards(lines, notes)
        lines = canonical_includes(lines)
    text = "\n".join(lines)
    text = re.sub(r"\bthrow\s*\([^)]*\)", "", text)
    lines = join_explode(text.split("\n"))
    lines = [tighten(l) for l in lines]
    lines = [comparison_rewrites(l) for l in lines]
    # __BEGIN_TRY / __END_CATCH* are statement-shaped macros without
    # semicolons; give each its own line so gluing differences don't
    # leak into the residual. __END_CATCH and __END_CATCH_NO_RETHROW
    # stay distinct tokens on purpose (different error semantics).
    relined = []
    for l in lines:
        if l.startswith("#"):
            relined.append(l)
            continue
        for part in re.sub(
            r"\b(__BEGIN_TRY|__END_CATCH_NO_RETHROW|__END_CATCH)\b", r"\n\1\n", l
        ).split("\n"):
            relined.append(part)
    lines = relined
    lines = [l for l in (x.strip() for x in lines) if l]
    if not fold:
        return lines
    lines = drop_empty_ctor_dtor(lines, pair)
    # Handler-dispatch fold: the server's execute() dispatches to the
    # Handler (execute(this,pPlayer) after qualifier strip); the client
    # copy either guards that call under #ifndef __GAME_CLIENT__
    # (folded above) or deleted it outright. Dispatch is not wire
    # traffic, and the unified file keeps the guarded call, preserving
    # both sides' behavior — so the bare dispatch line is dropped for
    # comparison. Only the exact canonical form; anything else in an
    # execute() body stays visible.
    lines = [l for l in lines if l != "execute(this,pPlayer);"]
    # Empty access sections: a public:/private:/protected: label whose
    # section ends immediately (next line closes the class) is
    # layout-only noise.
    out = []
    for i, l in enumerate(lines):
        if l in ("public:", "private:", "protected:"):
            nxt = lines[i + 1] if i + 1 < len(lines) else ""
            if nxt in ("}", "public:", "private:", "protected:"):
                continue
        out.append(l)
    return out


# ---------------------------------------------------------------------------
# Wire signatures.
# ---------------------------------------------------------------------------


def _find_body(lines, header_pred):
    for i, line in enumerate(lines):
        if header_pred(line) and line.endswith("{"):
            depth = 1
            body = []
            j = i + 1
            while j < len(lines) and depth > 0:
                if lines[j].endswith("{"):
                    depth += 1
                if lines[j] == "}":
                    depth -= 1
                    if depth == 0:
                        break
                body.append(lines[j])
                j += 1
            return body
    return None


_IO_CALL = re.compile(r"\b(read|write)(Encrypt)?\(((?:[^()]|\([^()]*\))*)\)")


def _io_seq(body, kind):
    seq = []
    for line in body:
        for m in _IO_CALL.finditer(line):
            if m.group(1) != kind:
                continue
            op = m.group(1) + (m.group(2) or "")
            seq.append("%s(%s)" % (op, m.group(3)))
    return seq


# Size-constant equivalences, verified against both trees'
# types/SystemTypes.h: szuint = sizeof(unsigned int) and
# szDWORD = sizeof(DWORD) are both 4 on the two supported ABIs
# (MSVC x64, Linux x86-64). szlong/szulong are deliberately NOT
# equivalent: the client pins sizeof(int32_t)=4, the server uses
# sizeof(long)=8 on LP64 — a real cross-tree difference.
_SIZE_TOKEN_EQUIV = {"szuint": "szDWORD"}


def canonical_expr(expr):
    expr = expr.strip().rstrip(";")
    expr = re.sub(
        r"\b\w+\b", lambda m: _SIZE_TOKEN_EQUIV.get(m.group(0), m.group(0)), expr
    )
    terms, cur, depth = [], "", 0
    for c in expr:
        if c in "([":
            depth += 1
        elif c in ")]":
            depth -= 1
        if c == "+" and depth == 0:
            terms.append(cur)
            cur = ""
        else:
            cur += c
    terms.append(cur)

    def canon_term(t):
        fac, cur, depth = [], "", 0
        for c in t:
            if c in "([":
                depth += 1
            elif c in ")]":
                depth -= 1
            if c == "*" and depth == 0:
                fac.append(cur)
                cur = ""
            else:
                cur += c
        fac.append(cur)
        fac = sorted(f.strip() for f in fac)
        # Expand a single small integer multiplier: 4*szCoordInven ==
        # szCoordInven+szCoordInven+szCoordInven+szCoordInven, so the
        # two spellings compare equal (commutativity + small-N
        # distributivity; symbolic multipliers are left alone).
        nums = [f for f in fac if f.isdigit()]
        syms = [f for f in fac if not f.isdigit()]
        if len(nums) == 1 and syms and 1 < int(nums[0]) <= 32:
            return [("*".join(syms))] * int(nums[0])
        return ["*".join(fac)]

    out = []
    for t in terms:
        out.extend(canon_term(t))
    return "+".join(sorted(out))


def _size_expr(lines, method):
    body = _find_body(lines, lambda l: ("%s()const{" % method) in l or ("%s(){" % method) in l)
    if body is None:
        return None
    for line in body:
        m = re.match(r"^return(.*);$", line)
        if m:
            return canonical_expr(m.group(1))
    return "<no-return>"


def wire_signature(h_lines, cpp_lines, notes):
    both = cpp_lines + h_lines  # bodies may be inline in .h or in .cpp
    read_body = _find_body(both, lambda l: re.search(r"\bread\(SocketInputStream", l))
    write_body = _find_body(both, lambda l: re.search(r"\bwrite\(SocketOutputStream", l))
    if read_body is None:
        notes.append("no read() body found")
    if write_body is None:
        notes.append("no write() body found")
    return {
        "read_seq": _io_seq(read_body or [], "read"),
        "write_seq": _io_seq(write_body or [], "write"),
        "size_expr": _size_expr(both, "getPacketSize"),
        "maxsize_expr": _size_expr(both, "getPacketMaxSize"),
    }


# ---------------------------------------------------------------------------
# Pair discovery / diffing / verdicts.
# ---------------------------------------------------------------------------


def list_complete_pairs(server_tree, client_tree):
    names = set()
    for tree in (server_tree, client_tree):
        for f in os.listdir(tree):
            if NAME_RE.match(f) and not re.search(r"Handler\.(cpp|h)$", f):
                names.add(re.sub(r"\.(cpp|h)$", "", f))
    out = []
    for cls in sorted(names):
        if all(
            os.path.isfile(os.path.join(t, cls + ext))
            for t in (server_tree, client_tree)
            for ext in (".cpp", ".h")
        ):
            out.append(cls)
    return out


def changed_lines(a_lines, b_lines, fa="a", fb="b"):
    n = 0
    for l in difflib.unified_diff(a_lines, b_lines, fromfile=fa, tofile=fb, lineterm=""):
        if l.startswith(("+++", "---")):
            continue
        if l.startswith(("+", "-")):
            n += 1
    return n


def read_file(path):
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        return fh.read()


def analyze_pair(cls, server_tree, client_tree, emit_dir=None):
    notes = []
    sides = {}
    for side, tree in (("server", server_tree), ("client", client_tree)):
        files = {}
        for ext in ("h", "cpp"):
            raw = read_file(os.path.join(tree, "%s.%s" % (cls, ext)))
            files[ext] = {
                "raw": raw.replace("\r", "").split("\n"),
                "audit": normalize_audit(raw).split("\n"),
                "style": normalize_style(raw, cls, notes),
            }
        sides[side] = files
        sides[side]["wire"] = wire_signature(
            files["h"]["style"], files["cpp"]["style"], notes
        )

    raw_diff = audit_diff = residual = 0
    residual_text = []
    for ext in ("h", "cpp"):
        s, c = sides["server"][ext], sides["client"][ext]
        raw_diff += changed_lines(s["raw"], c["raw"])
        audit_diff += changed_lines(s["audit"], c["audit"])
        residual += changed_lines(s["style"], c["style"])
        residual_text.extend(
            difflib.unified_diff(
                s["style"], c["style"],
                fromfile="server/%s.%s" % (cls, ext),
                tofile="client/%s.%s" % (cls, ext), lineterm="",
            )
        )

    ws, wc = sides["server"]["wire"], sides["client"]["wire"]
    wire = {}
    wire_reasons = []
    for key in ("read_seq", "write_seq", "size_expr"):
        wire[key] = "match" if ws[key] == wc[key] else "DIFF"
        if ws[key] != wc[key]:
            wire_reasons.append(key)
    wire["maxsize_expr"] = "match" if ws["maxsize_expr"] == wc["maxsize_expr"] else "WARN-diff"
    if wire["maxsize_expr"] != "match":
        notes.append("getPacketMaxSize differs (factory; client copy is compiled "
                     "out in real builds — review, does not drive verdict)")

    if wire_reasons:
        verdict = "real-divergence"
    elif residual == 0:
        verdict = "style-only"
    else:
        verdict = "style-residual"

    result = {
        "pair": cls,
        "raw_diff": raw_diff,
        "audit_diff": audit_diff,
        "residual": residual,
        "verdict": verdict,
        "wire": wire,
        "wire_reasons": wire_reasons,
        "server_wire": ws,
        "client_wire": wc,
        "notes": sorted(set(notes)),
    }

    if emit_dir:
        os.makedirs(emit_dir, exist_ok=True)
        for side in ("server", "client"):
            for ext in ("h", "cpp"):
                p = os.path.join(emit_dir, "%s.%s.%s.norm" % (cls, side, ext))
                with open(p, "w", encoding="utf-8") as fh:
                    fh.write("\n".join(sides[side][ext]["style"]) + "\n")
        with open(os.path.join(emit_dir, "%s.residual.diff" % cls), "w", encoding="utf-8") as fh:
            fh.write("\n".join(residual_text) + "\n")
    return result


# ---------------------------------------------------------------------------
# CLI.
# ---------------------------------------------------------------------------


def main(argv):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    default_root = os.path.normpath(os.path.join(script_dir, "..", ".."))
    ap = argparse.ArgumentParser(description="Phase 12 packet style-normalization prototype")
    ap.add_argument("--repo-root", default=default_root)
    ap.add_argument("--server-tree")
    ap.add_argument("--client-tree")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--pair")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--audit-check", action="store_true")
    ap.add_argument("--selfcheck", action="store_true")
    ap.add_argument("--tsv", action="store_true")
    ap.add_argument("--emit")
    args = ap.parse_args(argv)

    server_tree = args.server_tree or os.path.join(args.repo_root, "dkrixserver", "src", "Core")
    client_tree = args.client_tree or os.path.join(
        args.repo_root, "dkrix", "Client", "Packet", "Cpackets"
    )
    for t in (server_tree, client_tree):
        if not os.path.isdir(t):
            print("tree not found: %s" % t, file=sys.stderr)
            return 2

    pairs = list_complete_pairs(server_tree, client_tree)

    if args.list:
        print("\n".join(pairs))
        return 0

    if args.audit_check:
        vals = []
        for cls in pairs:
            d = 0
            for ext in ("h", "cpp"):
                s = normalize_audit(read_file(os.path.join(server_tree, "%s.%s" % (cls, ext))))
                c = normalize_audit(read_file(os.path.join(client_tree, "%s.%s" % (cls, ext))))
                d += changed_lines(s.split("\n"), c.split("\n"))
            vals.append((d, cls))
        vals.sort()
        nums = [v[0] for v in vals]
        print("pairs: %d" % len(nums))
        print("min:    %d (%s)" % (vals[0][0], vals[0][1]))
        print("median: %g" % statistics.median(nums))
        print("p90:    %g" % nums[int(0.9 * (len(nums) - 1))])
        print("max:    %d (%s)" % (vals[-1][0], vals[-1][1]))
        print("expected from docs/packet-divergence-2026-08-07.md: "
              "min 92, median 157, p90 230, max 747 (CLRegisterPlayer)")
        return 0

    if args.selfcheck:
        # Prove the fold rules are wire-neutral: per pair and side, the
        # wire signature extracted after the full pipeline must equal
        # the one extracted after the fold-free pipeline.
        bad = 0
        for cls in pairs:
            for tree, side in ((server_tree, "server"), (client_tree, "client")):
                texts = {
                    ext: read_file(os.path.join(tree, "%s.%s" % (cls, ext)))
                    for ext in ("h", "cpp")
                }
                n1, n2 = [], []
                full = {ext: normalize_style(texts[ext], cls, n1) for ext in texts}
                mini = {
                    ext: normalize_style(texts[ext], cls, n2, fold=False)
                    for ext in texts
                }
                sig_full = wire_signature(full["h"], full["cpp"], [])
                sig_mini = wire_signature(mini["h"], mini["cpp"], [])
                if sig_full != sig_mini:
                    bad += 1
                    print("MISMATCH %s (%s): %s vs %s" % (cls, side, sig_full, sig_mini))
        print("selfcheck: %d/%d pair-sides wire-neutral under fold rules"
              % (2 * len(pairs) - bad, 2 * len(pairs)))
        return 0 if bad == 0 else 1

    if args.pair:
        if args.pair not in pairs:
            print("not a complete pair: %s" % args.pair, file=sys.stderr)
            return 2
        r = analyze_pair(args.pair, server_tree, client_tree, args.emit)
        print(json.dumps(r, indent=2, sort_keys=True))
        return 0

    if args.all:
        results = [analyze_pair(c, server_tree, client_tree, args.emit) for c in pairs]
        if args.tsv:
            print("pair\traw_diff\taudit_diff\tresidual\tverdict\twire_reasons")
            for r in results:
                print("%s\t%d\t%d\t%d\t%s\t%s" % (
                    r["pair"], r["raw_diff"], r["audit_diff"], r["residual"],
                    r["verdict"], ",".join(r["wire_reasons"]) or "-"))
        else:
            for r in results:
                print(json.dumps(r, sort_keys=True))
        n = len(results)
        style = sum(1 for r in results if r["verdict"] == "style-only")
        near = sum(1 for r in results if r["verdict"] == "style-residual" and r["residual"] <= 5)
        srest = sum(1 for r in results if r["verdict"] == "style-residual" and r["residual"] > 5)
        real = sum(1 for r in results if r["verdict"] == "real-divergence")
        print(
            "# summary: pairs %d | style-only %d | style-residual<=5 %d | "
            "style-residual>5 %d | real-divergence %d" % (n, style, near, srest, real),
            file=sys.stderr,
        )
        return 0

    ap.print_help()
    return 2


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
