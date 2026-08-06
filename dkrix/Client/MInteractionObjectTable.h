//----------------------------------------------------------------------
// MInteractionObjectTable.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------
//

//
// - Type ID


// 
//----------------------------------------------------------------------

#ifndef	__MINTERACTIONOBJECTTABLE_H__
#define	__MINTERACTIONOBJECTTABLE_H__


#include "MObject.h"
#include "CTypeTable.h"
#include "DrawTypeDef.h"
#include "MTypeDef.h"

#include <fstream>
using namespace std;

//----------------------------------------------------------------------
//

//
//----------------------------------------------------------------------
class INTERACTIONOBJECTTABLE_INFO {
	public :
		BYTE					Type;			// InteractionObject Type
		TYPE_FRAMEID			FrameID;		// FrameID		
		int						Property;		
		TYPE_SOUNDID			SoundID;		

	public :
		//-------------------------------------------------------
		// File I/O
		//-------------------------------------------------------
		void			SaveToFile(std::ofstream& file);		
		void			LoadFromFile(std::ifstream& file);
		
};


typedef CTypeTable<INTERACTIONOBJECTTABLE_INFO>		INTERACTIONOBJECT_TABLE;
extern INTERACTIONOBJECT_TABLE* 	g_pInteractionObjectTable;

#endif

