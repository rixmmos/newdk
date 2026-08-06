//----------------------------------------------------------------------
// MFirePiecingEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MFIREPIECINGEFFECTGENERATOR_H__
#define	__MFIREPIECINGEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MFirePiecingEffectGenerator : public MEffectGenerator {
	public :
		MFirePiecingEffectGenerator() {}
		~MFirePiecingEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_FIRE_PIECING; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MStopZoneEffectGenerator	g_StopZoneEffectGenerator;

#endif

