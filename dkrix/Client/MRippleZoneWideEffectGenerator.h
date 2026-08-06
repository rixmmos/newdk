//----------------------------------------------------------------------
// MRippleZoneWideEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MRIPPLEZONEWIDEEFFECTGENERATOR_H__
#define	__MRIPPLEZONEWIDEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MRippleZoneWideEffectGenerator : public MEffectGenerator {
	public :
		MRippleZoneWideEffectGenerator() {}
		~MRippleZoneWideEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_RIPPLE_ZONE_WIDE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MRippleZoneWideEffectGenerator	g_RippleZoneWideEffectGenerator;

#endif

