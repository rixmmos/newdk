//----------------------------------------------------------------------
// MEffectSpriteTypeTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//

//



// 
//----------------------------------------------------------------------

#ifndef	__MEFFECTSPRITETYPETABLE_H__
#define	__MEFFECTSPRITETYPETABLE_H__

#pragma warning(disable:4786)

#include "MTypeDef.h"
#include "DrawTypeDef.h"
#include "CTypeTable.h"

#include <fstream>
#include <list>
using namespace std;
//#include "EffectSpriteTypeDef.h"

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class EFFECTSPRITETYPETABLE_INFO {
	public :
		typedef std::list<TYPE_FRAMEID>		FRAMEID_LIST;

	public :
		BLT_TYPE					BltType;				
		TYPE_FRAMEID				FrameID;				// Frame ID	
		bool						RepeatFrame;			
		TYPE_FRAMEID				ActionEffectFrameID;	
		
		FRAMEID_LIST				PairFrameIDList;		
		bool						bPairFrameBack;			
		TYPE_EFFECTSPRITETYPE		FemaleEffectSpriteType;	

	public :
		EFFECTSPRITETYPETABLE_INFO();
		//-------------------------------------------------------
		// Save
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);	
		void			LoadFromFile(std::ifstream& file);
		
};

typedef CTypeTable<EFFECTSPRITETYPETABLE_INFO>	EFFECTSPRITETYPE_TABLE;


//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class FRAME_TYPE {
	public :
		TYPE_FRAMEID		FrameID;

	public :
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);
};

typedef CTypeTable<FRAME_TYPE>	FRAMEID_ARRAY;
class ACTION_FRAMEID_ARRAY : public FRAMEID_ARRAY {
	public :
		ACTION_FRAMEID_ARRAY();
		~ACTION_FRAMEID_ARRAY();
};

typedef CTypeTable<ACTION_FRAMEID_ARRAY>	ACTION_FRAMEID_TABLE;

class MActionEffectSpriteTypeTable : public ACTION_FRAMEID_TABLE {
	public :
		MActionEffectSpriteTypeTable();
		~MActionEffectSpriteTypeTable();
};


extern	EFFECTSPRITETYPE_TABLE*			g_pEffectSpriteTypeTable;
extern	MActionEffectSpriteTypeTable*	g_pActionEffectSpriteTypeTable;

#ifdef __SANITIZE_ADDRESS__
// Shadow copies for corruption detection
extern EFFECTSPRITETYPE_TABLE* g_pEffectSpriteTypeTable_shadow;
extern EFFECTSPRITETYPE_TABLE::TYPE* g_pEffectSpriteTypeTable_m_pTypeInfo_shadow;

// Function to validate that g_pEffectSpriteTypeTable hasn't been corrupted
extern void validate_effect_sprite_table_pointer(const char* location);
#endif

#endif

