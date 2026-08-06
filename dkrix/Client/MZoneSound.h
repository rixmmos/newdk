//-----------------------------------------------------------------------------
// MZoneSound.h
//-----------------------------------------------------------------------------

//







//



//




//-----------------------------------------------------------------------------

#ifndef __MZONESOUND_H__
#define __MZONESOUND_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include "MTypeDef.h"
#include "ShowTimeChecker.h"

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ZONESOUND_INFO : public ShowTimeChecker {
	public :
		ZONESOUND_INFO();
		~ZONESOUND_INFO();

		//---------------------------------------------------------------
		// File I/O
		//---------------------------------------------------------------
		void					SaveToFile(std::ofstream& file);
		void					LoadFromFile(std::ifstream& file);

	public :
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		unsigned short	ID;				
		TYPE_SOUNDID	SoundID;		// SoundID
};


#endif

