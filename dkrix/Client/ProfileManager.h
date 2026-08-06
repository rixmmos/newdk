//----------------------------------------------------------------------
// ProfileManager.h
//----------------------------------------------------------------------

//


//

//
//

//



//

//



//
//----------------------------------------------------------------------

#ifndef __PROFILE_MANAGER_H__
#define __PROFILE_MANAGER_H__


#pragma warning(disable:4786)

#include "../basic/Platform.h"

#include <map>
#include <string>

//----------------------------------------------------------------------
// ProfileManager
//----------------------------------------------------------------------
class ProfileManager {
	public :
		enum HAS_PROFILE
		{

		};

	public :
		typedef std::map<std::string, std::string>		PROFILE_MAP;

		typedef std::map<std::string, int>				REQUIRE_MAP;
		
	public :
		ProfileManager();
		~ProfileManager();

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		static void		DeleteProfiles();
		static void		InitProfiles();		

		//-------------------------------------------------------------
		// Release
		//-------------------------------------------------------------
		void				Release();		

		//-------------------------------------------------------------
		// Add / Remove Profile
		//-------------------------------------------------------------
		bool			HasProfile(const char* pName) const;
		void			RequestProfile(const char* pName);
		void			AddProfile(const char* pName, const char* pFilename);		
		bool			RemoveProfile(const char* pName);

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		bool			HasProfileNULL(const char* pName) const;
		void			AddProfileNULL(const char* pName);

		//-------------------------------------------------------------
		// Get
		//-------------------------------------------------------------
		const char*		GetFilename(const char* pName) const;


		//-------------------------------------------------------------
		// Require
		//-------------------------------------------------------------
		void			AddRequire(const char* pName);
		bool			HasRequire(const char* pName) const;
		bool			RemoveRequire(const char* pName);
		void			ReleaseRequire();

		void			Update();
		
	protected :
		//----------------------------------------------------------------------
		// Lock / Unlock
		//----------------------------------------------------------------------
		void		Lock()					{ EnterCriticalSection(&m_Lock); }
		void		Unlock()				{ LeaveCriticalSection(&m_Lock); }

	private :
		PROFILE_MAP		m_Profiles;

		REQUIRE_MAP		m_Requires;

		CRITICAL_SECTION		m_Lock;
};

// Compile-time check to ensure CRITICAL_SECTION is fully defined
// Windows: sizeof(CRITICAL_SECTION) >= 68 bytes
// POSIX/Emscripten: sizeof(CRITICAL_SECTION) = sizeof(pthread_mutex_t) + sizeof(int)
 #ifdef PLATFORM_WINDOWS
	static_assert(sizeof(CRITICAL_SECTION) >= sizeof(void*), "CRITICAL_SECTION is incomplete");
#else
	// For POSIX systems (including Emscripten), the size may vary
	// Just ensure it contains the mutex (basic sanity check)
	static_assert(sizeof(CRITICAL_SECTION) >= sizeof(int), "CRITICAL_SECTION is incomplete");
#endif

extern ProfileManager*		g_pProfileManager;


#endif



