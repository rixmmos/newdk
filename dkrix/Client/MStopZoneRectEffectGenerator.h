//----------------------------------------------------------------------
// MStopZoneRectEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSTOPZONERECTEFFECTGENERATOR_H__
#define	__MSTOPZONERECTEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MStopZoneRectEffectGenerator : public MEffectGenerator {
	public :
		MStopZoneRectEffectGenerator()	{}
		~MStopZoneRectEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_STOP_ZONE_RECT; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MStopZoneRectEffectGenerator	g_StopZoneRectEffectGenerator;

#endif

