#include <fstream>
//----------------------------------------------------------------------
// CAlphaSprite565.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__CALPHASPRITE565_H__
#define	__CALPHASPRITE565_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif


#include "CAlphaSprite.h"


class CAlphaSprite565 : public CAlphaSprite {
	public :
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
		//bool	LoadFromFileToBuffer(std::ifstream& file);
};


#endif



