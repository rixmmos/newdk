//----------------------------------------------------------------------
// MAttackZoneBombEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACKZONEBOMBEFFECTGENERATOR_H__
#define	__MATTACKZONEBOMBEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackZoneBombEffectGenerator : public MEffectGenerator {
	public :
		MAttackZoneBombEffectGenerator() {}
		~MAttackZoneBombEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_ZONE_BOMB; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackZoneParabolaEffectGenerator	g_AttackZoneParabolaEffectGenerator;

#endif

