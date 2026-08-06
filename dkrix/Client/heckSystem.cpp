// heckSystem.cpp: implementation of the CheckSystem class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
//#include "checkSystemVer.h"
#include "heckSystem.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CheckSystem::CheckSystem()
{

}

CheckSystem::~CheckSystem()
{

}

BOOL CheckSystem::GetSystem()
{
	OSVERSIONINFO OsInfo;
	OsInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&OsInfo))
	{
		// platform
		switch (OsInfo.dwPlatformId)
		{
		case VER_PLATFORM_WIN32_WINDOWS:
			if(OsInfo.dwMajorVersion == 3) {
				
				
				//return FALSE;
			} else if(OsInfo.dwMajorVersion == 4) {
				
				//return FALSE;
				
			}
			break;
		case VER_PLATFORM_WIN32_NT:
			if(OsInfo.dwMajorVersion == 5) {
				
				//return TRUE;
				
			} else {
				//return TRUE;
				
				
			}
			break;
		default:
			//return FALSE;
			
			break;
		}
		// version and language
	}
	return FALSE;
}
