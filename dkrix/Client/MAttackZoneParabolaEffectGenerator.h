//----------------------------------------------------------------------
// MAttackZoneParabolaEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACKZONEPARABOLAEFFECTGENERATOR_H__
#define	__MATTACKZONEPARABOLAEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackZoneParabolaEffectGenerator : public MEffectGenerator {
	public :
		MAttackZoneParabolaEffectGenerator() {}
		~MAttackZoneParabolaEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_ZONE_PARABOLA; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackZoneParabolaEffectGenerator	g_AttackZoneParabolaEffectGenerator;

#endif

