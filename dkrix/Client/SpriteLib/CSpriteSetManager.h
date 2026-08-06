#include <fstream>
//----------------------------------------------------------------------
// CSpriteSetManager.h
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

#ifndef	__CSPRITESETMANAGER_H__
#define	__CSPRITESETMANAGER_H__


#include "DrawTypeDef.h"
#include "CSetManager.h"


class CSpriteSetManager : public CSetManager<TYPE_SPRITEID, TYPE_SPRITEID> {
	public :
		CSpriteSetManager();
		~CSpriteSetManager();

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		bool		SaveSpriteSetIndex(std::ofstream& setIndex, std::ifstream& spkIndex);
		

	protected :
		
};


#endif
