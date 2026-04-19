//--------------------------------------------------------------------------------
//
// Filename   : Endian.h
// Written By : Phase 13B (2026-04-19)
//
// Little-endian wire-format primitives for the socket stream API.
//
// The on-the-wire byte order for DarkEden is little-endian; see
// docs/MODERNIZATION.md Phase 13.1 for the decision and rationale
// (short version: every deployed server and desktop client is
// already x86 LE, so making LE explicit is a no-op today while
// letting future BE hosts opt in via these same helpers).
//
// These helpers are intentionally header-only inline wrappers —
// no .cpp, no linker impact on LE hosts (where they compile to a
// no-op). On a theoretical BE host they compile to a single
// bswap instruction.
//
// Usage (planned for Phase 13.3, deferred until after Phase 12
// unifies the client + server packet trees):
//
//   // Writing a uint32_t to the socket output stream:
//   uint32_t wire = htole32(value);
//   memcpy(m_Buffer + m_Tail, &wire, sizeof(wire));
//
//   // Reading a uint32_t from the socket input stream:
//   uint32_t wire;
//   memcpy(&wire, m_Buffer + m_Head, sizeof(wire));
//   value = le32toh(wire);
//
// The `memcpy` also eliminates the unaligned-access UB that the
// current `*(T*)(m_Buffer + m_Head)` cast has on strict-alignment
// targets.
//
// This header is currently UNUSED — 13B (this commit) lands the
// primitives so they're available when Phase 12 + Phase 13.3
// eventually wire them into the unified Socket{Input,Output}
// Stream templates.
//
//--------------------------------------------------------------------------------

#ifndef __ENDIAN_H__
#define __ENDIAN_H__

#include <cstdint>

//--------------------------------------------------------------------------------
// Platform selection.
//
// Linux ships <endian.h> with htole* / le*toh as macros or inline
// functions — use them directly.
//
// macOS ships <libkern/OSByteOrder.h> with OSSwapHostToLittleInt*
// which has the same semantics. Wrap them to present the POSIXy
// htole*/le*toh names so callers don't branch on OS.
//
// mingw / other fall through to a portable __builtin_bswap* path
// keyed off a compile-time host-endian check. GCC and Clang both
// define __BYTE_ORDER__; MSVC does not, but the server doesn't
// target MSVC (see docs/MODERNIZATION.md non-goals).
//--------------------------------------------------------------------------------

#if defined(__linux__) || defined(__LINUX__)

#include <endian.h>
// <endian.h> on glibc already provides htole16/32/64 and
// le16/32/64toh — nothing to define.

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>

inline uint16_t htole16(uint16_t v) { return OSSwapHostToLittleInt16(v); }
inline uint32_t htole32(uint32_t v) { return OSSwapHostToLittleInt32(v); }
inline uint64_t htole64(uint64_t v) { return OSSwapHostToLittleInt64(v); }
inline uint16_t le16toh(uint16_t v) { return OSSwapLittleToHostInt16(v); }
inline uint32_t le32toh(uint32_t v) { return OSSwapLittleToHostInt32(v); }
inline uint64_t le64toh(uint64_t v) { return OSSwapLittleToHostInt64(v); }

#else
// Portable fallback for mingw and any other toolchain that
// doesn't ship <endian.h>. Relies on GCC/Clang __builtin_bswap*
// and __BYTE_ORDER__ / __ORDER_LITTLE_ENDIAN__ macros.

#if !defined(__BYTE_ORDER__) || !defined(__ORDER_LITTLE_ENDIAN__) || !defined(__ORDER_BIG_ENDIAN__)
#error "Endian.h: compiler does not define __BYTE_ORDER__; add a platform branch."
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

inline uint16_t htole16(uint16_t v) { return v; }
inline uint32_t htole32(uint32_t v) { return v; }
inline uint64_t htole64(uint64_t v) { return v; }
inline uint16_t le16toh(uint16_t v) { return v; }
inline uint32_t le32toh(uint32_t v) { return v; }
inline uint64_t le64toh(uint64_t v) { return v; }

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

inline uint16_t htole16(uint16_t v) { return __builtin_bswap16(v); }
inline uint32_t htole32(uint32_t v) { return __builtin_bswap32(v); }
inline uint64_t htole64(uint64_t v) { return __builtin_bswap64(v); }
inline uint16_t le16toh(uint16_t v) { return __builtin_bswap16(v); }
inline uint32_t le32toh(uint32_t v) { return __builtin_bswap32(v); }
inline uint64_t le64toh(uint64_t v) { return __builtin_bswap64(v); }

#else
#error "Endian.h: unsupported __BYTE_ORDER__ (neither LE nor BE)."
#endif

#endif // platform selection

//--------------------------------------------------------------------------------
// Signed-integer and float convenience wrappers.
//
// The unsigned-integer helpers above are the canonical ones; the
// signed/float variants are built on top so callers don't need
// to bit-cast manually at every call site. Implemented via
// memcpy — not reinterpret_cast — to stay inside the strict-
// aliasing rules.
//--------------------------------------------------------------------------------

#include <cstring>

inline int16_t htole_i16(int16_t v)
{
    uint16_t u;
    std::memcpy(&u, &v, sizeof(u));
    u = htole16(u);
    std::memcpy(&v, &u, sizeof(v));
    return v;
}

inline int32_t htole_i32(int32_t v)
{
    uint32_t u;
    std::memcpy(&u, &v, sizeof(u));
    u = htole32(u);
    std::memcpy(&v, &u, sizeof(v));
    return v;
}

inline int64_t htole_i64(int64_t v)
{
    uint64_t u;
    std::memcpy(&u, &v, sizeof(u));
    u = htole64(u);
    std::memcpy(&v, &u, sizeof(v));
    return v;
}

inline int16_t le_i16toh(int16_t v)
{
    uint16_t u;
    std::memcpy(&u, &v, sizeof(u));
    u = le16toh(u);
    std::memcpy(&v, &u, sizeof(v));
    return v;
}

inline int32_t le_i32toh(int32_t v)
{
    uint32_t u;
    std::memcpy(&u, &v, sizeof(u));
    u = le32toh(u);
    std::memcpy(&v, &u, sizeof(v));
    return v;
}

inline int64_t le_i64toh(int64_t v)
{
    uint64_t u;
    std::memcpy(&u, &v, sizeof(u));
    u = le64toh(u);
    std::memcpy(&v, &u, sizeof(v));
    return v;
}

#endif // __ENDIAN_H__
