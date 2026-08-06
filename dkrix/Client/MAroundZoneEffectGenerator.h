//----------------------------------------------------------------------
// MAroundZoneEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MAROUNDZONEEFFECTGENERATOR_H__
#define	__MAROUNDZONEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAroundZoneEffectGenerator : public MEffectGenerator {
	public :
		MAroundZoneEffectGenerator()	{}
		~MAroundZoneEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_AROUND_ZONE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAroundZoneEffectGenerator	g_StopZoneEffectGenerator;

#endif

