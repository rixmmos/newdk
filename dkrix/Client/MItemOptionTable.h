//----------------------------------------------------------------------
// MItemOptionTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MITEMOPTIONTABLE_H__
#define	__MITEMOPTIONTABLE_H__

#include "CTypeTable.h"
#include "MTypeDef.h"
#include "MString.h"

#include <fstream>
using namespace std;

//#define	MAX_PARTNAME_LENGTH		48
#define UNIQUE_ITEM_COLOR		0xffff
#define QUEST_ITEM_COLOR		0xfffe

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class ITEMOPTION_INFO {
	public :
	public :
		MString					EName;					
		MString					Name;					
		int						Part;					// ItemOption Part				
		int						PlusPoint;				
		int						PriceMultiplier;		
		

		
		int						RequireSTR;
		int						RequireDEX;
		int						RequireINT;
		int						RequireSUM;		
		int						RequireLevel;
		
		int						ColorSet;				
		int						UpgradeOptionType;		
		int						PreviousOptionType;		

	public :
		ITEMOPTION_INFO();
		~ITEMOPTION_INFO();

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(std::ifstream& file);		
};


//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
class ITEMOPTION_TABLE:public CTypeTable<ITEMOPTION_INFO>
{
public:
	enum ITEMOPTION_PART
	{
		PART_STR = 0,      // increase STR
		PART_DEX,          // increase DEX 
		PART_INT,          // increase INT 
		PART_HP,           // increase HP 
		PART_MP,           // increase MP 
		PART_HP_STEAL,     // steal HP 
		PART_MP_STEAL,     // steal MP 
		PART_HP_REGEN,     // regenerate hp over time
		PART_MP_REGEN,     // regenerate mp over time
		PART_TOHIT,        // increase tohit
		PART_DEFENSE,      // increase defense
		PART_DAMAGE,       // increase damage 
		PART_PROTECTION,   // increase protection
		PART_DURABILITY,   // increase item durability
		PART_POISON,       // increase poison resistence
		PART_ACID,         // increase acid resistence
		PART_CURSE,        // increase curse resistence
		PART_BLOOD,        // increase blood resistence
		PART_VISION,       // increase vision range
		PART_ATTACK_SPEED, // increase attack speed
		PART_CRITICAL_HIT,
			
		
		PART_LUCK,         // increase looting item type
		PART_ALL_RES,      // increase all registance
		PART_ALL_ATTR,     // increase all attributes(str, dex, int)

		
		PART_STR_TO_DEX,   // STR to DEX
		PART_STR_TO_INT,   // STR to INT
		PART_DEX_TO_STR,   // DEX to STR
		PART_DEX_TO_INT,   // DEX to INT
		PART_INT_TO_STR,   // INT to STR
		PART_INT_TO_DEX,   // INT to DEX

		// 2003.2.12
		PART_CONSUME_MP,       // decrease consume mana
		PART_TRANS,        // translate race language
		PART_MAGIC_DAMAGE,  // increase magic damage
		PART_PHYSIC_DAMAGE, // increase physical damage
		PART_GAMBLE_PRICE, // decrease gamble price
		PART_POTION_PRICE, // decrease gamble price
 
		OPTION_MAGIC_PRO,	
		OPTION_PHYSIC_PRO,	

		MAX_PART
	};

	
	MString ITEMOPTION_PARTNAME[MAX_PART];
	MString ITEMOPTION_PARTENAME[MAX_PART];
		

	void	LoadFromFile(std::ifstream& file);
};

extern	ITEMOPTION_TABLE*		g_pItemOptionTable;


#endif
