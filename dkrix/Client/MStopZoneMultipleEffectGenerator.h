//----------------------------------------------------------------------
// MStopZoneMultipleEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTOPZONEMULTIPLEEFFECTGENERATOR_H__
#define	__MSTOPZONEMULTIPLEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneMultipleEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneMultipleEffectGenerator() {}
		~MStopZoneMultipleEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_MULTIPLE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MMultipleFallingEffectGenerator	g_StopZoneCrossEffectGenerator;

#endif

