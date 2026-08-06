//----------------------------------------------------------------------
// CCreatureFrameSetManager.h
//----------------------------------------------------------------------
//

//
//
//----------------------------------------------------------------------

#ifndef	__CCREATUREFRAMESETMANAGER_H__
#define	__CCREATUREFRAMESETMANAGER_H__

#include "CFrameSetManager.h"
#include "CFramePack.h"

class CCreatureFrameSetManager : public CFrameSetManager {
	public :
		CCreatureFrameSetManager();
		~CCreatureFrameSetManager();

	protected :
		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		bool	SaveSpriteSetIndex(CCreatureFramePack* pCreatureFramePack, std::ofstream& setIndex, std::ifstream& packIndex);
};

#endif

