//----------------------------------------------------------------------
// MStopZoneWallEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTOPZONWALLEEFFECTGENERATOR_H__
#define	__MSTOPZONWALLEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneWallEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneWallEffectGenerator() {}
		~MStopZoneWallEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_WALL; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

#endif

