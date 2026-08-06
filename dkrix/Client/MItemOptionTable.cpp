//--------------------------------------------------------------------------
// MItemOptionTable.cpp
//--------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItemOptionTable.h"

//--------------------------------------------------------------------------
// Global
//--------------------------------------------------------------------------
ITEMOPTION_TABLE*		g_pItemOptionTable = NULL;

//char ITEMOPTION_INFO::ITEMOPTION_PARTENAME[MAX_PART][MAX_PARTNAME_LENGTH] =
//{
//	"STR",
//	"DEX",
//	"INT",
//	"HP",
//	"MP",
//	"HP Steal",
//	"MP Steal",
//	"HP Regeneration",
//	"MP Regeneration",
//	"ToHit",
//	"Defense",
//	"Damage",
//	"Protection",
//	"Durability",
//	"Poison Resistance",
//	"Acid Resistance",
//	"Curse Resistance",
//	"Blood Resistance",
//	"Vision",
//	"Attack Speed",
//	"Critical Hit",
//	"Luck",         // increase looting item type
//	"All Registance",      // increase all registance
//	"All Attributes",     // increase all attributes(str, dex, int)
//};
//
//char ITEMOPTION_INFO::ITEMOPTION_PARTNAME[MAX_PART][MAX_PARTNAME_LENGTH] =
//{
//	"STR",
//	"DEX",
//	"INT",
//	"HP",
//	"MP",



















//};

//--------------------------------------------------------------------------
//
// constructor/destructor
//
//--------------------------------------------------------------------------
ITEMOPTION_INFO::ITEMOPTION_INFO()
{
}

ITEMOPTION_INFO::~ITEMOPTION_INFO()
{
}

//#include <stdio.h>

//--------------------------------------------------------------------------
//
// member functions
//
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// Save To File
//--------------------------------------------------------------------------
//void			
//ITEMOPTION_INFO::SaveToFile(std::ofstream& file)
//{
//	//char str[80];
//	//sprintf(str, "E-%s", Name.GetString());
//	//EName = str;
//


//	file.write((const char*)&Part, 4);				// ItemOption Part	



//	
//
//	file.write((const char*)&RequireSTR, 4);
//	file.write((const char*)&RequireDEX, 4);
//	file.write((const char*)&RequireINT, 4);		
//	file.write((const char*)&RequireSUM, 4);
//	file.write((const char*)&RequireLevel, 4);	
//

//	file.write((const char*)&UpgradeOptionType, 4);
//	file.write((const char*)&PreviousOptionType, 4);
//}

//--------------------------------------------------------------------------
// Load From File
//--------------------------------------------------------------------------
void			
ITEMOPTION_INFO::LoadFromFile(std::ifstream& file)
{
	EName.LoadFromFile(file);							
	Name.LoadFromFile(file);							
	file.read((char*)&Part, 4);				// ItemOption Part	
	file.read((char*)&PlusPoint, 4);			
	file.read((char*)&PriceMultiplier, 4);				
	
	
	file.read((char*)&RequireSTR, 4);
	file.read((char*)&RequireDEX, 4);
	file.read((char*)&RequireINT, 4);		
	file.read((char*)&RequireSUM, 4);
	file.read((char*)&RequireLevel, 4);

	
	file.read((char*)&ColorSet, 4);			
	file.read((char*)&UpgradeOptionType, 4);
	file.read((char*)&PreviousOptionType, 4);
}

//--------------------------------------------------------------------------
// Load From File
//--------------------------------------------------------------------------
void			
ITEMOPTION_INFO::SaveToFile(std::ofstream& file)
{
}

//--------------------------------------------------------------------------
// Load From File
//--------------------------------------------------------------------------
void
ITEMOPTION_TABLE::LoadFromFile(std::ifstream& file)
{
	int size;

	file.read((char *)&size, 4);
	for(int i = 0; i < size; i++)
	{
		ITEMOPTION_PARTENAME[i].LoadFromFile(file);
		ITEMOPTION_PARTNAME[i].LoadFromFile(file);
	}

	CTypeTable<ITEMOPTION_INFO>::LoadFromFile(file);
}