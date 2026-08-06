//----------------------------------------------------------------------
// MAttackCreatureHomingEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACKCREATUREHOMINGEFFECTGENERATOR_H__
#define	__MATTACKCREATUREHOMINGEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackCreatureHomingEffectGenerator : public MEffectGenerator {
	public :
		MAttackCreatureHomingEffectGenerator()	{}
		~MAttackCreatureHomingEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_CREATURE_HOMING; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackCreatureHomingEffectGenerator	g_AttackCreatureHomingEffectGenerator;

#endif

