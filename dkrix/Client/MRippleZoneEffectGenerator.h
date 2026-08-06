//----------------------------------------------------------------------
// MRippleZoneEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MRIPPLEZONEEFFECTGENERATOR_H__
#define	__MRIPPLEZONEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MRippleZoneEffectGenerator : public MEffectGenerator {
	public :
		MRippleZoneEffectGenerator() {}
		~MRippleZoneEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_RIPPLE_ZONE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MRippleZoneEffectGenerator	g_RippleZoneEffectGenerator;

#endif

