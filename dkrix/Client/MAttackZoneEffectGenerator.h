//----------------------------------------------------------------------
// MAttackZoneEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACKZONEEFFECTGENERATOR_H__
#define	__MATTACKZONEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackZoneEffectGenerator : public MEffectGenerator {
	public :
		MAttackZoneEffectGenerator() {}
		~MAttackZoneEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_ZONE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackZoneEffectGenerator	g_AttackZoneEffectGenerator;

#endif

