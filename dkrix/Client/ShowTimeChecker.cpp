//-----------------------------------------------------------------------------
// ShowTimeChecker.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "ShowTimeChecker.h"
#include "Client_PCH.h"
#ifdef __GAME_CLIENT__
	#include "MGameTime.h"
#endif

extern DWORD	g_CurrentTime;

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
ShowTimeChecker::ShowTimeChecker()
{
	Loop = false;

	
	MinDelay = 60000;
	MaxDelay = 60000;

	
	StartHour = 0;
	EndHour = 24;

	NextPlayTime = 0;
}

ShowTimeChecker::~ShowTimeChecker()
{
}

//-----------------------------------------------------------------------------
// Is Show Time
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
bool					
ShowTimeChecker::IsShowTime() const
{
	if (IsShowHour())
	{
		//----------------------------------------------------------------
		
		
		//----------------------------------------------------------------
		return Loop || g_CurrentTime >= NextPlayTime;		
	}

	return false;		
}

//-----------------------------------------------------------------------------
// IsShowHour
//-----------------------------------------------------------------------------

// false : x

//-----------------------------------------------------------------------------
bool					
ShowTimeChecker::IsShowHour() const
{
	if (g_pGameTime!=NULL)
	{
		int hour = g_pGameTime->GetHour();

		//----------------------------------------------------------------
		
		//----------------------------------------------------------------
		if (StartHour <= EndHour)
		{
			// ......[Start]xxxxx[End]......
			if (hour >= StartHour && hour <= EndHour)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		// xxxxx[End]......[Start]xxxxx
		if (hour >= StartHour || hour <= EndHour)
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Set NextShowTime
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void					
ShowTimeChecker::SetNextShowTime()
{
	DWORD delayGap = MaxDelay - MinDelay;

	if (delayGap==0)
	{
		NextPlayTime = g_CurrentTime;
	}
	else
	{
		NextPlayTime = g_CurrentTime + MinDelay + rand()%delayGap;
	}
}


//-----------------------------------------------------------------------------
// Save To File
//-----------------------------------------------------------------------------
void					
ShowTimeChecker::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&Loop, 1);

	
	file.write((const char*)&MinDelay, 4);
	file.write((const char*)&MaxDelay, 4);
	
	
	file.write((const char*)&StartHour, 1);
	file.write((const char*)&EndHour, 1);
}

//-----------------------------------------------------------------------------
// Load From File
//-----------------------------------------------------------------------------
void					
ShowTimeChecker::LoadFromFile(std::ifstream& file)
{
	file.read((char*)&Loop, 1);

	
	file.read((char*)&MinDelay, 4);
	file.read((char*)&MaxDelay, 4);
	
	
	file.read((char*)&StartHour, 1);
	file.read((char*)&EndHour, 1);
}