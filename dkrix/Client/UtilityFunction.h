//-----------------------------------------------------------------------------
// UtilityFunction.h
//-----------------------------------------------------------------------------

#ifndef __UTILITYFUNCTION_H__
#define __UTILITYFUNCTION_H__

class CDirectDrawSurface;
class CSpriteSurface;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void	RemoveStringSpace(char*& str);

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int		IsValidSSN(const char* strSSN1, const char* strSSN2);

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int		IsValidID(const char* strID, const char* strPermit);

//-----------------------------------------------------------------------------
// Is Valid Password
//-----------------------------------------------------------------------------
int		IsValidPassword(const char* str);

//-----------------------------------------------------------------------------
// LoadImageToSurface
//-----------------------------------------------------------------------------
bool	LoadImageToSurface(const char* pFilename, CDirectDrawSurface& surface);

//-----------------------------------------------------------------------------
// SaveSurfaceToImage
//-----------------------------------------------------------------------------
bool	SaveSurfaceToImage(const char* pFilename, CDirectDrawSurface& surface);
bool	SaveSurfaceToImage(const char* pFilename, CSpriteSurface& surface);

//-----------------------------------------------------------------------------
// Get DiskFreeSpace
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
unsigned long	GetDiskFreeSpace(const char* pDrive=NULL);

// SDL backend overloads
bool	LoadImageToSurface(const char* pFilename, CSpriteSurface& surface);

#endif
