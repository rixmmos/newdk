//----------------------------------------------------------------------
// MItemTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//

//



// 
//

//
//----------------------------------------------------------------------
//


//









//----------------------------------------------------------------------

#ifndef	__MITEMTABLE_H__
#define	__MITEMTABLE_H__

#pragma warning(disable:4786)

#include "CTypeTable.h"
#include "DrawTypeDef.h"
#include "MTypeDef.h"
#include "MString.h"
#include "SoundDef.h"
//#include "SkillDef.h"
#include <list>

//----------------------------------------------------------------------

//----------------------------------------------------------------------
#define	REQUIRE_STR							0x01
#define	REQUIRE_DEX							0x02
#define	REQUIRE_INT							0x04
#define	REQUIRE_LEVEL						0x08

extern COLORREF g_ELEMENTAL_COLOR[5];
extern int g_ELEMENTAL_STRING_ID[5];

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class ITEMTABLE_INFO {
	public :
		enum ELEMENTAL_TYPE
		{
			ELEMENTAL_TYPE_ANY = -1,
			ELEMENTAL_TYPE_FIRE,
			ELEMENTAL_TYPE_WATER,
			ELEMENTAL_TYPE_EARTH,
			ELEMENTAL_TYPE_WIND,
			ELEMENTAL_TYPE_SUM,

			ELEMENTAL_TYPE_MAX,
		};

		MString					HName;				
		MString					EName;				
		MString					Description;		
		
	 	// Frame ID
		TYPE_FRAMEID			TileFrameID;		
		TYPE_FRAMEID			InventoryFrameID;	
		TYPE_FRAMEID			GearFrameID;		
		TYPE_FRAMEID			AddonMaleFrameID;	
		TYPE_FRAMEID			AddonFemaleFrameID;	
		TYPE_FRAMEID			DropFrameID;		

		
		TYPE_FRAMEID			DescriptionFrameID;	
		// 2005, 1, 14, sobeit add end
		
		// Sound ID
		TYPE_SOUNDID			UseSoundID;			
		TYPE_SOUNDID			TileSoundID;		
		TYPE_SOUNDID			InventorySoundID;	
		TYPE_SOUNDID			GearSoundID;		

		bool					bMaleOnly;
		bool					bFemaleOnly;
		
		
		BYTE					GridWidth;
		BYTE					GridHeight;

		
		TYPE_ITEM_WEIGHT		Weight;				
		TYPE_ITEM_PRICE			Price;
		int						SilverMax;			
		int						ToHit;				// ToHit

		
		int						Value1;
		int						Value2;
		int						Value3;				
		int						Value4;
		int						Value5;
		int						Value6;
		int						Value7;

		
		TYPE_ACTIONINFO			UseActionInfo;	

		
		TYPE_ITEM_NUMBER		MaxNumber;

		// critical
		int						CriticalHit;
		std::list<TYPE_ITEM_OPTION>	DefaultOptionList;
		int						ItemStyle;
		
		// ousters info
		ELEMENTAL_TYPE			ElementalType;
		WORD					Elemental;
		BYTE					Race;
		
	//protected :	
	public:
		
		BYTE					RequireSTR;
		BYTE					RequireDEX;
		BYTE					RequireINT;		
		BYTE					RequireLevel;
		BYTE					RequireAdvancementLevel; 
		WORD					RequireSUM;		

	public :
		ITEMTABLE_INFO();
		~ITEMTABLE_INFO();

		//-------------------------------------------------------
		// Set
		//-------------------------------------------------------
		void	SetSoundID(TYPE_SOUNDID tile, TYPE_SOUNDID inventory, TYPE_SOUNDID gear, TYPE_SOUNDID use);
		void	SetFrameID(TYPE_FRAMEID tile, TYPE_FRAMEID inventory, TYPE_FRAMEID gear);
		void	SetDropFrameID(TYPE_FRAMEID drop);
		void	SetAddonFrameID(TYPE_FRAMEID male, TYPE_FRAMEID female);
		void	SetGrid(BYTE width, BYTE height);
		void	SetValue(int v1, int v2=-1, int v3=-1, int v4=-1, int v5=-1, int v6=-1, int v7=-1);
		void	SetDescriptionFrameID(TYPE_FRAMEID fID) { DescriptionFrameID = fID;}

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void	SetRequireSTR(BYTE str=0)			{ RequireSTR = str; }
		void	SetRequireDEX(BYTE dex=0)			{ RequireDEX = dex; }
		void	SetRequireINT(BYTE intel=0)			{ RequireINT = intel; }
		void	SetRequireSUM(WORD sum=0)			{ RequireSUM = sum; }
		void	SetRequireLevel(BYTE level=0)		{ RequireLevel = level; }		
		void	SetRequireAdvancementLevel(BYTE level=0)		{ RequireAdvancementLevel = level; }		

		//-------------------------------------------------------
		// Get
		//-------------------------------------------------------
		BYTE		GetRequireSTR()	const				{ return RequireSTR; }
		BYTE		GetRequireDEX()	const				{ return RequireDEX; }
		BYTE		GetRequireINT()	const				{ return RequireINT; }
		WORD		GetRequireSUM()	const				{ return RequireSUM; }
		BYTE		GetRequireLevel() const				{ return RequireLevel; }		
		BYTE		GetRequireAdvancementLevel() const	{ return RequireAdvancementLevel; }		

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		bool		IsGenderForMale() const				{ return bMaleOnly; }
		bool		IsGenderForFemale() const			{ return bFemaleOnly; }
		bool		IsGenderForAll() const				{ return !bMaleOnly && !bFemaleOnly; }
		

		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(std::ifstream& file);		
};


//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
class ITEMTYPE_TABLE : public CTypeTable<ITEMTABLE_INFO>
{
public :
	void	LoadFromFile(std::ifstream& file);

	int		GetAveragePrice() const	{ return m_AveragePrice; }

private :
	int		m_AveragePrice;
};

//----------------------------------------------------------------------
//

//
//  :  (*g_pItemTable)[ itemClass ][ itemType ]
//
//----------------------------------------------------------------------
class ITEMCLASS_TABLE : public CTypeTable<ITEMTYPE_TABLE> {

	public :
		ITEMCLASS_TABLE();
		~ITEMCLASS_TABLE();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		void		InitClass( int c, int size );
		void		InitItem2();
};

extern	ITEMCLASS_TABLE	*	g_pItemTable;


#endif
