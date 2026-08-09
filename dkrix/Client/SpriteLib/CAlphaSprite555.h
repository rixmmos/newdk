#include <fstream>
//----------------------------------------------------------------------
// CAlphaSprite555.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__CALPHASPRITE555_H__
#define	__CALPHASPRITE555_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif


#include "CAlphaSprite.h"


class CAlphaSprite555 : public CAlphaSprite {
	public :
	
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
};


#endif



