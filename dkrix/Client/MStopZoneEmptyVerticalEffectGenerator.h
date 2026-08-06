//----------------------------------------------------------------------
// MStopZoneEmptyVerticalWallEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTOPZONEEMPTYVERTICALWALLEEFFECTGENERATOR_H__
#define	__MSTOPZONEEMPTYVERTICALWALLEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneEmptyVerticalWallEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneEmptyVerticalWallEffectGenerator() {}
		~MStopZoneEmptyVerticalWallEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_EMPTY_VERTICAL_WALL; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

#endif

