#include "TextBackend.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "SpriteLib/SpriteLibBackend.h"

#ifdef USE_SDL_BACKEND
#include <SDL.h>
#include <SDL_ttf.h>
#endif

namespace TextSystem {

static void TraceTextBackend(const char* fmt, ...)
{
	(void)fmt;
}

static uint32_t ClampRenderableCodepoint(uint32_t codepoint)
{
	if (codepoint == 0)
		return 0;
	if (codepoint > 0x10FFFF)
		return static_cast<uint32_t>('?');
	if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
		return static_cast<uint32_t>('?');
	return codepoint;
}

struct GlyphKey {
	int fontId;
	uint32_t codepoint;
	uint32_t color;

	bool operator==(const GlyphKey& other) const
	{
		return fontId == other.fontId && codepoint == other.codepoint && color == other.color;
	}
};

struct GlyphKeyHash {
	size_t operator()(const GlyphKey& key) const
	{
		return (static_cast<size_t>(key.fontId) * 1315423911u) ^
			(static_cast<size_t>(key.codepoint) * 2654435761u) ^
			(static_cast<size_t>(key.color) * 97531u);
	}
};

class TextBackendSDL : public TextBackend {
public:
	TextBackendSDL()
		: m_initialized(false)
	{}

	~TextBackendSDL() override
	{
		for (auto& it : m_glyphs) {
			Glyph& g = it.second;
			if (g.handle) {
				spritectl_destroy_sprite(reinterpret_cast<spritectl_sprite_t>(g.handle));
				g.handle = NULL;
			}
		}
		m_glyphs.clear();

		for (size_t i = 0; i < m_fonts.size(); ++i) {
			if (m_fonts[i]) {
				TTF_CloseFont(m_fonts[i]);
			}
		}
		m_fonts.clear();
	}

	bool Initialize() override
	{
#ifdef USE_SDL_BACKEND
		TraceTextBackend("Initialize begin TTF_WasInit=%d", TTF_WasInit());
		if (TTF_WasInit() == 0) {
			if (TTF_Init() != 0) {
				fprintf(stderr, "TextBackendSDL: TTF_Init failed: %s\n", TTF_GetError());
				TraceTextBackend("TTF_Init failed: %s", TTF_GetError());
				return false;
			}
			TraceTextBackend("TTF_Init OK");
		}
#endif
		m_initialized = true;
		TraceTextBackend("Initialize OK");
		return true;
	}

	FontHandle AcquireFont(const FontDesc& desc) override
	{
		if (!m_initialized)
			return FontHandle();

		int size = desc.size > 0 ? desc.size : 16;
		char sizeBuf[32];
		snprintf(sizeBuf, sizeof(sizeBuf), "%d", size);
		std::string cacheKey = desc.family;
		cacheKey += "#";
		cacheKey += sizeBuf;

		TraceTextBackend("AcquireFont request family='%s' size=%d", desc.family.c_str(), size);
		auto it = m_fontToFontId.find(cacheKey);
		if (it != m_fontToFontId.end()) {
			FontHandle handle;
			handle.id = it->second;
			TraceTextBackend("AcquireFont cache hit family='%s' size=%d id=%d", desc.family.c_str(), size, handle.id);
			return handle;
		}

		const bool useCormorant = desc.family == "Cormorant Garamond";
		const bool useUnifraktur = desc.family == "UnifrakturCook";
		const char* cormorantPaths[] = {
			"Data/Font/CormorantGaramond-SemiBold.otf",
			"Data/Font/CormorantGaramond-Regular.otf",
			NULL
		};
		const char* unifrakturPaths[] = {
			"Data/Font/UnifrakturCook-Bold.ttf",
			NULL
		};

		const char* fontPaths[] = {
			"Data/Font/CormorantGaramond-Regular.otf",
			"Data/Font/NotoSansCJK-Regular.ttc",
			"Data/Font/NotoSans-Regular.ttf",
			"Data/Font/DejaVuSans.ttf",
#ifdef PLATFORM_WINDOWS
			"C:/Windows/Fonts/segoeui.ttf",
			"C:/Windows/Fonts/arial.ttf",
			"C:/Windows/Fonts/calibri.ttf",
			"C:/Windows/Fonts/tahoma.ttf",
#endif
			"Data/Font/Hiragino Sans GB.ttc",
			"/System/Library/Fonts/Helvetica.ttc",
			"/System/Library/Fonts/Hiragino Sans GB.ttc",
			NULL
		};

		TTF_Font* font = NULL;
		if (useCormorant) {
			for (int i = 0; cormorantPaths[i] != NULL; ++i) {
				TraceTextBackend("AcquireFont try path='%s' size=%d", cormorantPaths[i], size);
				font = TTF_OpenFont(cormorantPaths[i], size);
				if (font) {
					fprintf(stderr, "TextBackendSDL: Loaded font '%s' size %d\n", cormorantPaths[i], size);
					TraceTextBackend("AcquireFont loaded path='%s' size=%d", cormorantPaths[i], size);
					break;
				}
			}
		}
		if (useUnifraktur) {
			for (int i = 0; unifrakturPaths[i] != NULL; ++i) {
				TraceTextBackend("AcquireFont try path='%s' size=%d", unifrakturPaths[i], size);
				font = TTF_OpenFont(unifrakturPaths[i], size);
				if (font) {
					fprintf(stderr, "TextBackendSDL: Loaded font '%s' size %d\n", unifrakturPaths[i], size);
					TraceTextBackend("AcquireFont loaded path='%s' size=%d", unifrakturPaths[i], size);
					break;
				}
			}
		}

		for (int i = 0; fontPaths[i] != NULL; ++i) {
			if (font)
				break;
			TraceTextBackend("AcquireFont try path='%s' size=%d", fontPaths[i], size);
			font = TTF_OpenFont(fontPaths[i], size);
			if (font) {
				fprintf(stderr, "TextBackendSDL: Loaded font '%s' size %d\n", fontPaths[i], size);
				TraceTextBackend("AcquireFont loaded path='%s' size=%d", fontPaths[i], size);
				break;
			}
		}

		if (!font) {
			fprintf(stderr, "TextBackendSDL: Failed to load font size %d\n", size);
			TraceTextBackend("AcquireFont failed size=%d err=%s", size, TTF_GetError());
			return FontHandle();
		}

		int id = static_cast<int>(m_fonts.size());
		m_fonts.push_back(font);
		m_fontToFontId[cacheKey] = id;

		FontHandle handle;
		handle.id = id;
		TraceTextBackend("AcquireFont success family='%s' size=%d id=%d", desc.family.c_str(), size, id);
		return handle;
	}

	int GetLineHeight(FontHandle font) const override
	{
		TTF_Font* ttf = GetFont(font);
		if (!ttf)
			return 16;
		return TTF_FontHeight(ttf);
	}

	int GetFontAscent(FontHandle font) const override
	{
		TTF_Font* ttf = GetFont(font);
		if (!ttf)
			return 12;
		return TTF_FontAscent(ttf);
	}

	bool GetGlyphMetrics(FontHandle font, uint32_t codepoint, GlyphMetrics& outMetrics) override
	{
		codepoint = ClampRenderableCodepoint(codepoint);
		TTF_Font* ttf = GetFont(font);
		if (!ttf)
			return false;
		if (codepoint < 128) {
			TraceTextBackend("GetGlyphMetrics font=%d codepoint=%u", font.id, codepoint);
		}

		int minx = 0, maxx = 0, miny = 0, maxy = 0, advance = 0;
		bool ok = false;

		if (codepoint <= 0xFFFF) {
			if (TTF_GlyphMetrics(ttf, static_cast<Uint16>(codepoint), &minx, &maxx, &miny, &maxy, &advance) == 0) {
				ok = true;
			}
		}

		if (!ok) {
			// Fallback: approximate metrics using rendered surface
			std::string utf8 = EncodeUtf8(codepoint);
			SDL_Color white = {255, 255, 255, 255};
			SDL_Surface* surf = TTF_RenderUTF8_Blended(ttf, utf8.c_str(), white);
			if (!surf)
			{
				TraceTextBackend("GetGlyphMetrics fallback render failed font=%d cp=%u err=%s", font.id, codepoint, TTF_GetError());
				return false;
			}
			minx = 0;
			maxx = surf->w;
			miny = -TTF_FontAscent(ttf);  // Assume top-aligned
			maxy = TTF_FontDescent(ttf);  // Assume baseline at bottom
			advance = surf->w;
			SDL_FreeSurface(surf);
		}

		outMetrics.width = maxx - minx;
		outMetrics.height = maxy - miny;
		outMetrics.advance = advance;
		outMetrics.bearingX = minx;
		// Keep legacy line positioning stable. SDL_ttf's rendered glyph
		// surface already includes the font line box, so callers should not
		// reposition it from raw glyph bounds alone.
		outMetrics.bearingY = TTF_FontAscent(ttf) + miny;
		return true;
	}

	const Glyph* GetGlyph(FontHandle font, uint32_t codepoint, const Color& color) override
	{
		codepoint = ClampRenderableCodepoint(codepoint);
		TTF_Font* ttf = GetFont(font);
		if (!ttf)
			return NULL;
		if (codepoint < 128) {
			TraceTextBackend("GetGlyph font=%d codepoint=%u color=%u,%u,%u,%u", font.id, codepoint, color.r, color.g, color.b, color.a);
		}

		uint32_t packedColor = (static_cast<uint32_t>(color.r) << 16) |
			(static_cast<uint32_t>(color.g) << 8) |
			static_cast<uint32_t>(color.b);

		GlyphKey key;
		key.fontId = font.id;
		key.codepoint = codepoint;
		key.color = packedColor;

		auto it = m_glyphs.find(key);
		if (it != m_glyphs.end()) {
			return &it->second;
		}

		// Get glyph metrics first
		int minx = 0, maxx = 0, miny = 0, maxy = 0, advance = 0;
		bool hasMetrics = false;

		if (codepoint <= 0xFFFF) {
			if (TTF_GlyphMetrics(ttf, static_cast<Uint16>(codepoint), &minx, &maxx, &miny, &maxy, &advance) == 0) {
				hasMetrics = true;
			}
		}

		// Render the glyph
		std::string utf8 = EncodeUtf8(codepoint);
		SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
		SDL_Surface* surface = TTF_RenderUTF8_Blended(ttf, utf8.c_str(), sdlColor);
		if (!surface)
		{
			TraceTextBackend("GetGlyph render failed font=%d cp=%u err=%s", font.id, codepoint, TTF_GetError());
			return NULL;
		}

		SDL_Surface* rgbaSurface = surface;
		if (surface->format->format != SDL_PIXELFORMAT_RGBA32) {
			rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
			SDL_FreeSurface(surface);
			if (!rgbaSurface)
			{
				TraceTextBackend("GetGlyph convert failed font=%d cp=%u err=%s", font.id, codepoint, SDL_GetError());
				return NULL;
			}
		}

		Glyph glyph;
		GlyphMetrics metrics;

		if (hasMetrics) {
			metrics.width = maxx - minx;
			metrics.height = maxy - miny;
			metrics.advance = advance;
			metrics.bearingX = 0;
			metrics.bearingY = 0;
		} else {
			// Fallback: approximate metrics from surface
			metrics.width = rgbaSurface->w;
			metrics.height = rgbaSurface->h;
			metrics.advance = rgbaSurface->w;
			metrics.bearingX = 0;
			metrics.bearingY = 0;
		}
		glyph.metrics = metrics;

		size_t dataSize = rgbaSurface->pitch * rgbaSurface->h;
		glyph.handle = reinterpret_cast<void*>(spritectl_create_sprite(
			rgbaSurface->w,
			rgbaSurface->h,
			SPRITECTL_FORMAT_RGBA32,
			rgbaSurface->pixels,
			dataSize));

		SDL_FreeSurface(rgbaSurface);

		if (!glyph.handle)
		{
			TraceTextBackend("GetGlyph sprite create failed font=%d cp=%u w=%d h=%d", font.id, codepoint, metrics.width, metrics.height);
			return NULL;
		}

		m_glyphs.emplace(key, glyph);
		if (codepoint < 128) {
			TraceTextBackend("GetGlyph success font=%d cp=%u advance=%d", font.id, codepoint, glyph.metrics.advance);
		}
		return &m_glyphs.find(key)->second;
	}

	void DrawGlyph(RenderTarget& target, const Glyph& glyph, int x, int y, uint8_t alpha) override
	{
		void* native = target.GetNative(NativeTargetType::SpriteCtlSurface);
		if (!native || !glyph.handle)
			return;

		spritectl_surface_t surface = reinterpret_cast<spritectl_surface_t>(native);
		spritectl_sprite_t sprite = reinterpret_cast<spritectl_sprite_t>(glyph.handle);

		(void)alpha;
		spritectl_blt_sprite(surface, x, y, sprite, SPRITECTL_BLT_NONE, 255);
	}

	bool DrawTextLine(RenderTarget& target, FontHandle font, const char* text, int x, int y, const Color& color, uint8_t alpha) override
	{
		TTF_Font* ttf = GetFont(font);
		if (!ttf || text == NULL || *text == '\0')
			return false;

		void* native = target.GetNative(NativeTargetType::SpriteCtlSurface);
		if (!native)
			return false;

		SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
		SDL_Surface* surface = TTF_RenderUTF8_Blended(ttf, text, sdlColor);
		if (!surface)
			return false;

		SDL_Surface* rgbaSurface = surface;
		if (surface->format->format != SDL_PIXELFORMAT_RGBA32) {
			rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
			SDL_FreeSurface(surface);
			if (!rgbaSurface)
				return false;
		}

		size_t dataSize = rgbaSurface->pitch * rgbaSurface->h;
		spritectl_sprite_t sprite = spritectl_create_sprite(
			rgbaSurface->w,
			rgbaSurface->h,
			SPRITECTL_FORMAT_RGBA32,
			rgbaSurface->pixels,
			dataSize);

		SDL_FreeSurface(rgbaSurface);
		if (!sprite)
			return false;

		spritectl_surface_t targetSurface = reinterpret_cast<spritectl_surface_t>(native);
		spritectl_blt_sprite(targetSurface, x, y, sprite, SPRITECTL_BLT_NONE, alpha);
		spritectl_destroy_sprite(sprite);
		return true;
	}

private:
	TTF_Font* GetFont(FontHandle handle) const
	{
		if (handle.id < 0 || handle.id >= static_cast<int>(m_fonts.size()))
			return NULL;
		return m_fonts[handle.id];
	}

	std::string EncodeUtf8(uint32_t codepoint) const
	{
		char buf[5] = {0};
		if (codepoint < 0x80) {
			buf[0] = static_cast<char>(codepoint);
			buf[1] = 0;
			return std::string(buf);
		}
		if (codepoint < 0x800) {
			buf[0] = static_cast<char>(0xC0 | (codepoint >> 6));
			buf[1] = static_cast<char>(0x80 | (codepoint & 0x3F));
			buf[2] = 0;
			return std::string(buf);
		}
		if (codepoint < 0x10000) {
			buf[0] = static_cast<char>(0xE0 | (codepoint >> 12));
			buf[1] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
			buf[2] = static_cast<char>(0x80 | (codepoint & 0x3F));
			buf[3] = 0;
			return std::string(buf);
		}
		buf[0] = static_cast<char>(0xF0 | (codepoint >> 18));
		buf[1] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
		buf[2] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
		buf[3] = static_cast<char>(0x80 | (codepoint & 0x3F));
		buf[4] = 0;
		return std::string(buf);
	}

private:
	bool m_initialized;
	std::vector<TTF_Font*> m_fonts;
	std::unordered_map<std::string, int> m_fontToFontId;
	std::unordered_map<GlyphKey, Glyph, GlyphKeyHash> m_glyphs;
};

TextBackend* CreateSDLTextBackend()
{
	return new TextBackendSDL();
}

} // namespace TextSystem
