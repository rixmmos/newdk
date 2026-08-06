#include <fstream>
//----------------------------------------------------------------------
// CSprite565.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__CSPRITE565_H__
#define	__CSPRITE565_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
std::ofstream;
std::ifstream;


#include "CSprite.h"


class CSprite565 : public CSprite {
	public :
		CSprite565() = default;
		virtual ~CSprite565() = default;

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
		//bool	LoadFromFileToBuffer(std::ifstream& file);
};


#endif



