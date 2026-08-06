//----------------------------------------------------------------------
// CFrameSetManager.h
//----------------------------------------------------------------------
//

//

//

// 




//




//
//


//----------------------------------------------------------------------
//



//
//----------------------------------------------------------------------

#ifndef	__CFRAMESETMANAGER_H__
#define	__CFRAMESETMANAGER_H__


#include "DrawTypeDef.h"
#include "../SpriteLib/CSetManager.h"


class CFrameSetManager : public CSetManager<TYPE_FRAMEID, TYPE_FRAMEID> {
	public :
		CFrameSetManager();
		virtual ~CFrameSetManager();

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		bool		SaveFrameSetIndex(std::ofstream& setIndex, std::ifstream& packIndex);

		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		//virtual bool		SaveSpriteSetIndex(std::ofstream& setIndex, std::ifstream& packIndex) = 0;
		

	protected :
		
};


#endif



