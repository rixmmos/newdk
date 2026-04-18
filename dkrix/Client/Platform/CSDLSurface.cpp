//----------------------------------------------------------------------
// CSDLSurface.cpp
//
// SDL2 Implementation (Cross-platform)
// Windows DirectDraw implementation removed - using SDL2 on all platforms
// NOTE: This class is deprecated. Use SDL_Surface directly instead.
//----------------------------------------------------------------------

#include "CSDLSurface.h"

//-----------------------------------------------------------------------------
// Static member initialization for SDL2
//-----------------------------------------------------------------------------
void (*CSDLSurface::s_GammaFunction)(void *pDest, int len, int p) = nullptr;

//-----------------------------------------------------------------------------
// Constructor/Destructor
//-----------------------------------------------------------------------------

CSDLSurface::CSDLSurface()
{
	m_pDDSurface = nullptr;
	m_Width = 0;
	m_Height = 0;
	m_ClipLeft = 0;
	m_ClipTop = 0;
	m_ClipRight = 0;
	m_ClipBottom = 0;
	m_bBackSurface = false;
	m_bLock = false;
}

CSDLSurface::~CSDLSurface()
{
	// Stub - SDL surfaces are managed elsewhere
}

//-----------------------------------------------------------------------------
// Surface Initialization (Stubs - deprecated)
//-----------------------------------------------------------------------------

bool CSDLSurface::InitBacksurface()
{
	// Not implemented - use SDL surfaces directly
	return false;
}

bool CSDLSurface::InitOffsurface(int wWidth, int wHeight, DWORD dwCaps)
{
	m_Width = wWidth;
	m_Height = wHeight;
	m_ClipRight = wWidth;
	m_ClipBottom = wHeight;
	(void)dwCaps;
	return true;
}

bool CSDLSurface::InitTextureSurface(int wWidth, int wHeight, DWORD dwCaps, const LPDDPIXELFORMAT pDDPF)
{
	// Not implemented - use SDL textures directly
	(void)wWidth;
	(void)wHeight;
	(void)dwCaps;
	(void)pDDPF;
	return false;
}

bool CSDLSurface::InitTextureSurfaceFromBMP(LPCSTR szBitmap, DWORD dwCaps)
{
	// Not implemented - use SDL_image to load BMPs
	(void)szBitmap;
	(void)dwCaps;
	return false;
}

bool CSDLSurface::InitFromBMP(LPCSTR szBitmap, DWORD dwCaps)
{
	// Not implemented - use SDL_image to load BMPs
	(void)szBitmap;
	(void)dwCaps;
	return false;
}

bool CSDLSurface::ReLoadBMP(LPCSTR szBitmap)
{
	// Not implemented
	(void)szBitmap;
	return false;
}

bool CSDLSurface::Restore()
{
	// Not implemented - SDL surfaces don't need restore
	return false;
}

//-----------------------------------------------------------------------------
// Surface Operations (Stubs - deprecated)
//-----------------------------------------------------------------------------

void CSDLSurface::SetTransparency(DWORD dwValue)
{
	// Not implemented - use SDL blend modes instead
	(void)dwValue;
}

void CSDLSurface::FillSurface(WORD color)
{
	// Not implemented - use SDL_FillRect instead
	(void)color;
}

void CSDLSurface::FillRect(RECT* pRect, WORD color)
{
	// Not implemented - use SDL_FillRect instead
	(void)pRect;
	(void)color;
}

//-----------------------------------------------------------------------------
// Lock/Unlock (Minimal stub for compatibility)
//-----------------------------------------------------------------------------

bool CSDLSurface::Lock()
{
	ZeroMemory(&m_ddsd, sizeof(m_ddsd));
	m_ddsd.dwSize = sizeof(m_ddsd);
	m_ddsd.lPitch = m_Width * 2;  // Assume 16-bit color
	m_ddsd.lpSurface = nullptr;  // No actual surface memory
	m_bLock = true;
	return true;
}

bool CSDLSurface::Unlock()
{
	m_bLock = false;
	return true;
}

void CSDLSurface::LockW(WORD*& lpSurface, WORD& lPitch)
{
	// Stub - no actual surface to lock
	lpSurface = nullptr;
	lPitch = m_Width * 2;
	m_bLock = true;
}

//-----------------------------------------------------------------------------
// Blitting (Stubs - deprecated)
//-----------------------------------------------------------------------------

void CSDLSurface::Blt(POINT* pPoint, CSDLSurface* SourceSurface, RECT* pRect)
{
	// Not implemented - use SDL_BlitSurface instead
	(void)pPoint;
	(void)SourceSurface;
	(void)pRect;
}

void CSDLSurface::BltNoColorkey(POINT* pPoint, CSDLSurface* SourceSurface, RECT* pRect)
{
	// Not implemented - use SDL_BlitSurface instead
	(void)pPoint;
	(void)SourceSurface;
	(void)pRect;
}

void CSDLSurface::Blt(RECT* pDestRect, CSDLSurface* SourceSurface, RECT* pSourceRect)
{
	// Not implemented - use SDL_BlitSurface instead
	(void)pDestRect;
	(void)SourceSurface;
	(void)pSourceRect;
}

void CSDLSurface::BltPrimarySurface(POINT* pPoint, RECT* pRect)
{
	// Not implemented
	(void)pPoint;
	(void)pRect;
}

//-----------------------------------------------------------------------------
// Gamma Correction (Not implemented - use shader effects instead)
//-----------------------------------------------------------------------------

void CSDLSurface::GammaBox555(RECT* pRect, int p)
{
	// Not implemented - use SDL2 shader effects
	(void)pRect;
	(void)p;
}

void CSDLSurface::GammaBox565(RECT* pRect, int p)
{
	// Not implemented - use SDL2 shader effects
	(void)pRect;
	(void)p;
}

void CSDLSurface::Gamma4Pixel565(void *pDest, int len, int p)
{
	// Not implemented - use SDL2 shader effects
	(void)pDest;
	(void)len;
	(void)p;
}

void CSDLSurface::Gamma4Pixel555(void *pDest, int len, int p)
{
	// Not implemented - use SDL2 shader effects
	(void)pDest;
	(void)len;
	(void)p;
}

//-----------------------------------------------------------------------------
// GDI Text (Not implemented - use SDL2 text rendering instead)
//-----------------------------------------------------------------------------

void CSDLSurface::ShowFPS(int x, int y, COLORREF fcolor, COLORREF bcolor)
{
	// Not implemented - use SDL2 text rendering
	(void)x;
	(void)y;
	(void)fcolor;
	(void)bcolor;
}

void CSDLSurface::GDI_Text(int x, int y, const char *str, COLORREF fcolor, COLORREF bcolor, bool option)
{
	// Not implemented - use SDL2 text rendering
	(void)x;
	(void)y;
	(void)str;
	(void)fcolor;
	(void)bcolor;
	(void)option;
}

//-----------------------------------------------------------------------------
// BMP Save/Load (Not implemented - use SDL_image instead)
//-----------------------------------------------------------------------------

bool CSDLSurface::SaveToBMP(const char* szFilename)
{
	// Not implemented - use SDL_image PNG/BMP saving
	(void)szFilename;
	return false;
}

bool CSDLSurface::CopyBitmap(HBITMAP hbm, int x, int y, int dx, int dy)
{
	// Not implemented - Windows GDI only
	(void)hbm;
	(void)x;
	(void)y;
	(void)dx;
	(void)dy;
	return false;
}
