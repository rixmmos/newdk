//-----------------------------------------------------------------------------
// ShowTimeChecker.h
//-----------------------------------------------------------------------------

#ifndef __SHOWTIMECHECKER_H__
#define __SHOWTIMECHECKER_H__

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"

#include <fstream>
using namespace std;
#endif

class ShowTimeChecker {
	public :
		ShowTimeChecker();
		~ShowTimeChecker();

		bool			IsShowTime() const;		
		bool			IsShowHour() const;		

		void			SetNextShowTime();

		//---------------------------------------------------------------
		// File I/O
		//---------------------------------------------------------------
		void			SaveToFile(std::ofstream& file);
		void			LoadFromFile(std::ifstream& file);

	public :
		
		bool			Loop;

		
		DWORD			MinDelay;
		DWORD			MaxDelay;
		
		
		BYTE			StartHour;
		BYTE			EndHour;

		DWORD			NextPlayTime;		
};

#endif


