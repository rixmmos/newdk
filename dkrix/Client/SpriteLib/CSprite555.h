#include <fstream>
//----------------------------------------------------------------------
// CSprite555.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__CSPRITE555_H__
#define	__CSPRITE555_H__

#ifdef PLATFORM_WINDOWS
	#include <Windows.h>
#else
	#include "../basic/Platform.h"
#endif
std::ofstream;
std::ifstream;


#include "CSprite.h"


class CSprite555 : public CSprite {
	public :
		CSprite555() = default;
		virtual ~CSprite555() = default;

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		bool		SaveToFile(std::ofstream& file);
		bool		LoadFromFile(std::ifstream& file);
		//bool		LoadFromFileToBuffer(std::ifstream& file);

};


#endif



