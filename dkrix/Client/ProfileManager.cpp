//----------------------------------------------------------------------
// ProfileManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#ifdef PLATFORM_WINDOWS
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/dir.h>
#endif
#include "CDirectDraw.h"
#include "CDirectDrawSurface.h"
#include "CSpritePack.h"
#include "UserInformation.h"
//#include "MFileDef.h"

#ifdef __GAME_CLIENT__
	#include "Packet/Properties.h"
#else
	#include "..\client\packet\Properties.h"
#endif

#include "UtilityFunction.h"
#include "ProfileManager.h"

#ifdef __GAME_CLIENT__
	#include "RequestUserManager.h"

	#include "Packet/Cpackets/CGRequestIP.h"

	#include "ServerInfo.h"
	#include "Packet/RequestClientPlayerManager.h"
#endif

#include "DebugInfo.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
ProfileManager*		g_pProfileManager = NULL;

#define	PROFILE_NULL	"NULL"

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
ProfileManager::ProfileManager()
{
	InitializeCriticalSection(&m_Lock);
}

ProfileManager::~ProfileManager()
{
	Release();

	DeleteCriticalSection(&m_Lock);
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
ProfileManager::Release()
{
	m_Profiles.clear();

	ReleaseRequire();
}

//----------------------------------------------------------------------
// Release Require
//----------------------------------------------------------------------
void
ProfileManager::ReleaseRequire()
{
	// Debug output before locking
	DEBUG_ADD("[ProfileManager] ReleaseRequire: attempting to lock...\n");

	Lock();

	DEBUG_ADD("[ProfileManager] ReleaseRequire: lock acquired, clearing...\n");

	m_Requires.clear();

	DEBUG_ADD("[ProfileManager] ReleaseRequire: done, releasing lock...\n");

	Unlock();
}

//----------------------------------------------------------------------
// Has Profile
//----------------------------------------------------------------------
bool
ProfileManager::HasProfile(const char* pName) const
{
	PROFILE_MAP::const_iterator iProfile = m_Profiles.find( std::string(pName) );

	if (iProfile!=m_Profiles.end())
	{
		if (iProfile->second==PROFILE_NULL)
		{
			return false;
		}

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Add Profile
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
ProfileManager::AddProfile(const char* pName, const char* pFilename)
{
	m_Profiles[std::string(pName)] = std::string(pFilename);
}

//----------------------------------------------------------------------
// Add ProfileNULL
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
ProfileManager::AddProfileNULL(const char* pName)
{
	m_Profiles[std::string(pName)] = PROFILE_NULL;
}

//----------------------------------------------------------------------
// Has ProfileNULL
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
ProfileManager::HasProfileNULL(const char* pName) const
{
	PROFILE_MAP::const_iterator iProfile = m_Profiles.find( std::string(pName) );

	if (iProfile!=m_Profiles.end())
	{
		if (iProfile->second==PROFILE_NULL)
		{
			return true;
		}
	}

	return false;	
}

//----------------------------------------------------------------------
// Remove Profile
//----------------------------------------------------------------------
bool
ProfileManager::RemoveProfile(const char* pName)
{
	PROFILE_MAP::iterator iProfile = m_Profiles.find( std::string(pName) );

	if (iProfile!=m_Profiles.end())
	{
		m_Profiles.erase( iProfile );

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Get UserInfo
//----------------------------------------------------------------------
const char*
ProfileManager::GetFilename(const char* pName) const
{
	PROFILE_MAP::const_iterator iProfile = m_Profiles.find( std::string(pName) );

	if (iProfile!=m_Profiles.end())
	{
		
		if (iProfile->second==PROFILE_NULL)
		{
			return NULL;
		}

		return iProfile->second.c_str();
	}

	return NULL;
}

//----------------------------------------------------------------------
// RequestProfile
//----------------------------------------------------------------------
void			
ProfileManager::RequestProfile(const char* pName)
{
	if(pName == NULL)
		return;

	if (!HasRequire(pName))
	{
		AddRequire( pName );
	}

	 
}

//----------------------------------------------------------------------
// Add Require
//----------------------------------------------------------------------
void			
ProfileManager::AddRequire(const char* pName)
{
	Lock();

	if (!HasProfile(pName)
		&& !HasProfileNULL(pName))
	{
		m_Requires[std::string(pName)] = 0;
	}

	Unlock();
}

//----------------------------------------------------------------------
// Has Require
//----------------------------------------------------------------------
bool			
ProfileManager::HasRequire(const char* pName) const
{
	if (m_Requires.find( std::string(pName) )==m_Requires.end())
	{
		return false;
	}

	return true;
}

//----------------------------------------------------------------------
// Remove Require
//----------------------------------------------------------------------
bool			
ProfileManager::RemoveRequire(const char* pName)
{
	Lock();

	REQUIRE_MAP::iterator iRequire = m_Requires.find( std::string(pName) );

	if (iRequire==m_Requires.end())
	{
		Unlock();
		return false;
	}

	m_Requires.erase( iRequire );

	Unlock();
	return true;
}

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------
void			
ProfileManager::Update()
{
#ifdef __GAME_CLIENT__
	if (g_pRequestClientPlayerManager==NULL 
		|| g_pRequestUserManager==NULL)
	{				
		m_Requires.clear();
		return;
	}
#endif

	Lock();

	//--------------------------------------------------------------
	// request
	//--------------------------------------------------------------
	REQUIRE_MAP::iterator iRequire = m_Requires.begin();

	while (iRequire != m_Requires.end())
	{
		const char* pName = iRequire->first.c_str();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (HasProfile(pName)
			|| HasProfileNULL(pName))
		{
			REQUIRE_MAP::iterator iTemp = iRequire;
			iRequire ++;
			m_Requires.erase( iTemp );
			
			continue;
		}

		#ifdef __GAME_CLIENT__
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (g_pRequestClientPlayerManager->HasConnection(pName)
				|| g_pRequestClientPlayerManager->HasTryingConnection(pName)
				|| g_pRequestUserManager->HasRequestingUser( pName ))
			{				
			}
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			else
			{
				RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo(pName);

				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (pUserInfo!=NULL)
				{
					if( g_pUserInformation->bKorean == true )	
					{
						g_pRequestClientPlayerManager->Connect(pUserInfo->IP.c_str(), 
																pName, 
																REQUEST_CLIENT_MODE_PROFILE);				
					}
				}
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				else
				{
					if (!g_pRequestUserManager->HasRequestingUser( pName ))
					{
						if( g_pUserInformation->bKorean == true )
						{
							
							CGRequestIP _CGRequestIP;
							_CGRequestIP.setName( pName );

							g_pSocket->sendPacket( &_CGRequestIP );			
			
						
						g_pRequestUserManager->AddRequestingUser( pName, RequestUserManager::REQUESTING_FOR_PROFILE );
						}
					}
				}
			}	
		#endif

		iRequire ++;
	}

	Unlock();
}


//----------------------------------------------------------------------
// Init Profiles
//----------------------------------------------------------------------

//

//----------------------------------------------------------------------
void		
ProfileManager::InitProfiles()
{
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	char CWD[_MAX_PATH];

	if (_getcwd( CWD, _MAX_PATH )!=NULL)
	{	
		if (_chdir( g_pFileDef->getProperty("DIR_PROFILE").c_str()) == 0)
		{
			
			_chdir( CWD );
		}
		else
		{
			
#ifdef PLATFORM_WINDOWS
			_mkdir( g_pFileDef->getProperty("DIR_PROFILE").c_str() );
#else
			mkdir( g_pFileDef->getProperty("DIR_PROFILE").c_str(), 0755 );
#endif
		}		
	}

#ifdef SPRITELIB_BACKEND_SDL
	// Profile portrait conversion still depends on legacy surface assumptions.
	// The game can run without pre-generated profile sprites; requests are filled later.
	return;
#endif

	char profileFiles[256];
	sprintf(profileFiles, "%s\\*.*", g_pFileDef->getProperty("DIR_PROFILE").c_str());
	
	char bmpFilename[256];

	struct _finddata_t	FileData;
	long				hFile;

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if ( (hFile = _findfirst( profileFiles, &FileData )) != -1L )
	{
		CSpritePack SPK;

		
		
		SPK.Init( 2);

#if defined(PLATFORM_WINDOWS) && !defined(SPRITELIB_BACKEND_SDL)
		CDirectDrawSurface surface;

		const POINT bigSize = { 55, 70 };
		const POINT smallSize = { 30, 38 };

		surface.InitOffsurface( bigSize.x, bigSize.y, DDSCAPS_SYSTEMMEMORY );

		RECT destBigRect = { 0, 0, bigSize.x, bigSize.y };
		RECT destSmallRect = { 0, 0, smallSize.x, smallSize.y };
#else
		// SDL backend: Use CSpriteSurface instead
		CSpriteSurface surface;

		const POINT bigSize = { 55, 70 };
		const POINT smallSize = { 30, 38 };

		// Initialize surface with big size
		// Note: SDL backend doesn't have InitOffsurface, surface will be created when needed

		RECT destBigRect = { 0, 0, bigSize.x, bigSize.y };
		RECT destSmallRect = { 0, 0, smallSize.x, smallSize.y };
#endif

		do
		{
			sprintf(bmpFilename, "%s\\%s", g_pFileDef->getProperty("DIR_PROFILE").c_str(), FileData.name);
	
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			char charName[256], spkFilename[256], spkiFilename[256];
			int lenFilename = strlen(FileData.name);

			
			if (lenFilename< 8)
			{
				continue;
			}

			strncpy( charName, FileData.name, lenFilename-4 );	
			charName[lenFilename-4] = '\0';

#if defined(PLATFORM_WINDOWS) && !defined(SPRITELIB_BACKEND_SDL)
			CDirectDrawSurface bmpSurface;

			if (LoadImageToSurface(bmpFilename, bmpSurface))
			{
				WORD* lpSurface;
				unsigned short pitch;

				
				
				RECT bmpRect = { 0, 0, bmpSurface.GetWidth(), bmpSurface.GetHeight() };

				// SmallSize
				surface.Blt(&destSmallRect, &bmpSurface, &bmpRect);
				surface.LockW(lpSurface, pitch);
				SPK[0].SetPixelNoColorkey(lpSurface, pitch, smallSize.x, smallSize.y);
				surface.Unlock();

				// BigSize
				surface.FillSurface( 0 );
				surface.Blt(&destBigRect, &bmpSurface, &bmpRect);
				surface.LockW(lpSurface, pitch);
				SPK[1].SetPixelNoColorkey(lpSurface, pitch, bigSize.x, bigSize.y);
				surface.Unlock();

				// filename.spk
				int lenBmpFilename = strlen(bmpFilename);
				strncpy(spkFilename, bmpFilename, lenBmpFilename-3);
				spkFilename[lenBmpFilename-3] = '\0';
				strcat(spkFilename, "spk");

				// filename.spki
				strcpy(spkiFilename, spkFilename);
				strcat(spkiFilename, "i");

				std::ofstream	spkFile(spkFilename, ios::binary);
				std::ofstream	spkiFile(spkiFilename, ios::binary);
				SPK.SaveToFile( spkFile, spkiFile );
				spkFile.close();
				spkiFile.close();

				g_pProfileManager->AddProfile( charName, spkFilename );
			}
#else
			// SDL backend: Profile image loading not yet implemented
			// This is a non-critical feature (profile character portraits)
			// TODO: Implement SDL_image based loading
			WORD* lpSurface;
			unsigned short pitch;

			// Create temporary surfaces for the profile
			RECT bmpRect = { 0, 0, smallSize.x, smallSize.y };
			RECT bmpRectBig = { 0, 0, bigSize.x, bigSize.y };

			// For now, just initialize empty sprites
			// The profile will load but without character portrait image
			lpSurface = new WORD[smallSize.x * smallSize.y];
			memset(lpSurface, 0, smallSize.x * smallSize.y * 2);
			pitch = smallSize.x * 2;
			SPK[0].SetPixelNoColorkey(lpSurface, pitch, smallSize.x, smallSize.y);
			delete[] lpSurface;

			lpSurface = new WORD[bigSize.x * bigSize.y];
			memset(lpSurface, 0, bigSize.x * bigSize.y * 2);
			pitch = bigSize.x * 2;
			SPK[1].SetPixelNoColorkey(lpSurface, pitch, bigSize.x, bigSize.y);
			delete[] lpSurface;

			// filename.spk
			int lenBmpFilename = strlen(bmpFilename);
			strncpy(spkFilename, bmpFilename, lenBmpFilename-3);
			spkFilename[lenBmpFilename-3] = '\0';
			strcat(spkFilename, "spk");

			// filename.spki
			strcpy(spkiFilename, spkFilename);
			strcat(spkiFilename, "i");

			std::ofstream	spkFile(spkFilename, ios::binary);
			std::ofstream	spkiFile(spkiFilename, ios::binary);
			SPK.SaveToFile( spkFile, spkiFile );
			spkFile.close();
			spkiFile.close();

			g_pProfileManager->AddProfile( charName, spkFilename );
#endif
		}
		while (_findnext( hFile, &FileData ) == 0);

		_findclose( hFile );
	}
}

//----------------------------------------------------------------------
// Delete Profiles
//----------------------------------------------------------------------

//

//----------------------------------------------------------------------
void		
ProfileManager::DeleteProfiles()
{
	char profileFiles[256], tempProfileFiles[256];
	sprintf(profileFiles, "%s\\*.spk*", g_pFileDef->getProperty("DIR_PROFILE").c_str());
	sprintf(tempProfileFiles, "%s\\*-spk*", g_pFileDef->getProperty("DIR_PROFILE").c_str());
	
	char spkFilename[256];

	struct _finddata_t	FileData;
	long				hFile;

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if ( (hFile = _findfirst( profileFiles, &FileData )) != -1L )
	{
		do
		{
			sprintf(spkFilename, "%s\\%s", g_pFileDef->getProperty("DIR_PROFILE").c_str(), FileData.name);			
			remove(spkFilename);
		}
		while (_findnext( hFile, &FileData ) == 0);

		_findclose( hFile );			
	}

	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	if ( (hFile = _findfirst( tempProfileFiles, &FileData )) != -1L )
	{
		do
		{
			sprintf(spkFilename, "%s\\%s", g_pFileDef->getProperty("DIR_PROFILE").c_str(), FileData.name);			
			remove(spkFilename);
		}
		while (_findnext( hFile, &FileData ) == 0);

		_findclose( hFile );			
	}
}

