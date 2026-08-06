//-----------------------------------------------------------------------------
// MZoneSound.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MZoneSound.h"


//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
ZONESOUND_INFO::ZONESOUND_INFO()
{
	ID = 0;				
	SoundID = 0;		// SoundID
}

ZONESOUND_INFO::~ZONESOUND_INFO()
{
}

//-----------------------------------------------------------------------------
// Save To File
//-----------------------------------------------------------------------------
void					
ZONESOUND_INFO::SaveToFile(std::ofstream& file)
{
	file.write((const char*)&ID, 2);				
	file.write((const char*)&SoundID, SIZE_SOUNDID);		// SoundID
	
	ShowTimeChecker::SaveToFile( file );
}

//-----------------------------------------------------------------------------
// Load From File
//-----------------------------------------------------------------------------
void					
ZONESOUND_INFO::LoadFromFile(std::ifstream& file)
{
	file.read((char*)&ID, 2);				
	file.read((char*)&SoundID, SIZE_SOUNDID);		// SoundID
	
	ShowTimeChecker::LoadFromFile( file );
}