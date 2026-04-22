//////////////////////////////////////////////////////////////////////
//
// Filename    : Endian.h
// Description : Shared little-endian wire helpers
//
//////////////////////////////////////////////////////////////////////

#ifndef __SHARED_CORE_ENDIAN_H__
#define __SHARED_CORE_ENDIAN_H__

#include <cstring>
#include <type_traits>

namespace endian {

template <typename T, bool IsEnum>
struct wire_base_impl;

template <typename T>
struct wire_base_impl<T, false> {
    typedef T type;
};

template <typename T>
struct wire_base_impl<T, true> {
    typedef typename std::underlying_type<T>::type type;
};

template <typename T>
struct wire_base {
    typedef typename wire_base_impl<T, std::is_enum<T>::value>::type type;
};

template <typename T>
struct wire_unsigned {
    typedef typename std::make_unsigned<typename wire_base<T>::type>::type type;
};

template <>
struct wire_unsigned<bool> {
    typedef unsigned char type;
};

template <typename T>
inline T decode_little(const char* raw) {
    typedef typename wire_base<T>::type Base;
    typedef typename wire_unsigned<T>::type Unsigned;

    Unsigned accum = 0;
    for (size_t i = 0; i < sizeof(Base); ++i) {
        accum |= static_cast<Unsigned>(static_cast<unsigned char>(raw[i])) << (i * 8);
    }

    Base base = 0;
    std::memcpy(&base, &accum, sizeof(Base));

    if (std::is_enum<T>::value) {
        return static_cast<T>(base);
    }

    return static_cast<T>(base);
}

template <typename T>
inline void encode_little(T value, char* raw) {
    typedef typename wire_base<T>::type Base;
    typedef typename wire_unsigned<T>::type Unsigned;

    Base base = static_cast<Base>(value);
    Unsigned accum = 0;
    std::memcpy(&accum, &base, sizeof(Base));

    for (size_t i = 0; i < sizeof(Base); ++i) {
        raw[i] = static_cast<char>((accum >> (i * 8)) & 0xFF);
    }
}

} // namespace endian

#endif
