//----------------------------------------------------------------------
// MRippleZonePixelEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MRIPPLEZONEPIXELEFFECTGENERATOR_H__
#define	__MRIPPLEZONEPIXELEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MRippleZonePixelEffectGenerator : public MEffectGenerator {
	public :
		MRippleZonePixelEffectGenerator() {}
		~MRippleZonePixelEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_RIPPLE_ZONE_PIXEL; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MRippleZoneEffectGenerator	g_RippleZoneEffectGenerator;

#endif

