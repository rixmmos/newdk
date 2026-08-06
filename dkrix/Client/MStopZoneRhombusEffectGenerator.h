//----------------------------------------------------------------------
// MStopZoneRhombusEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTOPZONERHOMBUSEFFECTGENERATOR_H__
#define	__MSTOPZONERHOMBUSEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneRhombusEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneRhombusEffectGenerator() {}
		~MStopZoneRhombusEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_RHOMBUS; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MStopZoneRhombusEffectGenerator	g_StopZoneRhombusEffectGenerator;

#endif

