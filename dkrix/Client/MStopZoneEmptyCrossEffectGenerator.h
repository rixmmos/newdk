//----------------------------------------------------------------------
// MStopZoneEmptyCrossEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTOPZONEEMPTYCROSSEFFECTGENERATOR_H__
#define	__MSTOPZONEEMPTYCROSSEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneEmptyCrossEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneEmptyCrossEffectGenerator()	 {}
		~MStopZoneEmptyCrossEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_EMPTY_CROSS; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MStopZoneCrossEffectGenerator	g_StopZoneCrossEffectGenerator;

#endif

