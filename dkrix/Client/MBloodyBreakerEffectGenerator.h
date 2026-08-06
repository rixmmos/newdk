//----------------------------------------------------------------------
// MBloodyDelugeEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MBLOODYBREAKEREFFECTGENERATOR_H__
#define	__MBLOODYBREAKEREFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MBloodyBreakerEffectGenerator : public MEffectGenerator {
	public :
		MBloodyBreakerEffectGenerator()	{}
		~MBloodyBreakerEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_BLOODY_BREAKER; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MBloodyWallEffectGenerator	g_StopZoneEffectGenerator;

#endif

