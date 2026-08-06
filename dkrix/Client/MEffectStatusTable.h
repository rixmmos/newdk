//----------------------------------------------------------------------
// MEffectStatusTable.h
//----------------------------------------------------------------------


//

//

//
//----------------------------------------------------------------------

#ifndef	__MEFFECTSTATUSTABLE_H__
#define	__MEFFECTSTATUSTABLE_H__

#include "CTypeTable.h"
#include "MTypeDef.h"
//#include "EffectSpriteTypeDef.h"
#include "MEffectStatusDef.h"
#include <fstream>

using namespace std;

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class EFFECTSTATUS_NODE {
	public :
		EFFECTSTATUS_NODE();

	public :
		bool						bUseEffectSprite;		
		bool						bAttachGround;			

		
		TYPE_EFFECTSPRITETYPE		EffectSpriteType;		
		WORD						EffectColor;			
		ADDON						EffectColorPart;		
		
		TYPE_ACTIONINFO				ActionInfo;				
		TYPE_ACTIONINFO				OriginalActionInfo;		
		int							SoundID;				// SoundID
		
	public :
		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(std::ifstream& file);		
};


typedef CTypeTable<EFFECTSTATUS_NODE>	EFFECTSTATUS_TABLE;
extern 	EFFECTSTATUS_TABLE*		g_pEffectStatusTable;


#endif


