//----------------------------------------------------------------------
// MBloodyWallEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MBLOODYWALLEFFECTGENERATOR_H__
#define	__MBLOODYWALLEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MBloodyWallEffectGenerator : public MEffectGenerator {
	public :
		MBloodyWallEffectGenerator()	{}
		~MBloodyWallEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_BLOODY_WALL; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MBloodyWallEffectGenerator	g_StopZoneEffectGenerator;

#endif

