//----------------------------------------------------------------------
// MBloodyWaveEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MBLOODYWAVEEFFECTGENERATOR_H__
#define	__MBLOODYWAVEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MBloodyWaveEffectGenerator : public MEffectGenerator {
	public :
		MBloodyWaveEffectGenerator()	{}
		~MBloodyWaveEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_BLOODY_WAVE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MBloodyWaveEffectGenerator	g_StopZoneEffectGenerator;

#endif

