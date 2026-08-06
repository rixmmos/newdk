//----------------------------------------------------------------------
// MAttachZoneEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSKIPEFFECTGENERATOR_H__
#define	__MSKIPEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MSkipEffectGenerator : public MEffectGenerator {
	public :
		MSkipEffectGenerator()	{}
		~MSkipEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_SKIP_DRAW; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttachZoneEffectGenerator	g_StopZoneEffectGenerator;

#endif

