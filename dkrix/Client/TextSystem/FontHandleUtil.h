#ifndef TEXTSYSTEM_FONTHANDLEUTIL_H
#define TEXTSYSTEM_FONTHANDLEUTIL_H

#include <stdint.h>

namespace TextSystem {

enum FontFamilyId {
	FontFamilyDefault = 0,
	FontFamilyCormorantGaramond = 1,
	FontFamilyUnifrakturCook = 2
};

// Encode a font size into a pointer-sized handle for non-Windows builds.
// This avoids allocating font objects while still preserving size/family info.
inline void* EncodeFontHandle(int size, int familyId)
{
	if (size < 0)
		size = -size;
	if (size <= 0)
		size = 16;
	uintptr_t value = static_cast<uintptr_t>(size) & 0xFFFFu;
	value |= (static_cast<uintptr_t>(familyId) & 0xFFu) << 16;
	value |= 0xF0000000u; // magic tag to identify encoded handles
	return reinterpret_cast<void*>(value);
}

inline void* EncodeFontSizeHandle(int size)
{
	return EncodeFontHandle(size, FontFamilyDefault);
}

inline int DecodeFontSizeHandle(void* handle, int fallback = 16)
{
	if (!handle)
		return fallback;
	uintptr_t value = reinterpret_cast<uintptr_t>(handle);
	if ((value & 0xF0000000u) == 0xF0000000u) {
		int size = static_cast<int>(value & 0xFFFFu);
		return size > 0 ? size : fallback;
	}
	if ((value & 0xF0000u) == 0xF0000u) {
		int size = static_cast<int>(value & 0xFFFFu);
		return size > 0 ? size : fallback;
	}
	if (value > 0 && value < 256)
		return static_cast<int>(value);
	return fallback;
}

inline int DecodeFontFamilyHandle(void* handle, int fallback = FontFamilyDefault)
{
	if (!handle)
		return fallback;
	uintptr_t value = reinterpret_cast<uintptr_t>(handle);
	if ((value & 0xF0000000u) == 0xF0000000u)
		return static_cast<int>((value >> 16) & 0xFFu);
	return fallback;
}

} // namespace TextSystem

#endif // TEXTSYSTEM_FONTHANDLEUTIL_H
