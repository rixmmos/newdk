//----------------------------------------------------------------------
// CThingFrameSetManager.h
//----------------------------------------------------------------------
//

//
//
//----------------------------------------------------------------------

#ifndef	__CANIMATIONFRAMESETMANAGER_H__
#define	__CANIMATIONFRAMESETMANAGER_H__

#include "CFrameSetManager.h"
#include "CFramePack.h"

class CAnimationFrameSetManager : public CFrameSetManager {
	public :
		CAnimationFrameSetManager();
		~CAnimationFrameSetManager();

	protected :
		//--------------------------------------------------------
		
		
		//--------------------------------------------------------
		bool	SaveSpriteSetIndex(CAnimationFramePack* pAnimationFramePack, std::ofstream& setIndex, std::ifstream& packIndex);
};

#endif

