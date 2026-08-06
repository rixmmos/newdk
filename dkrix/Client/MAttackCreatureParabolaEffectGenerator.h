//----------------------------------------------------------------------
// MAttackCreatureParabolaEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACKCREATUREPARABOLAEFFECTGENERATOR_H__
#define	__MATTACKCREATUREPARABOLAEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackCreatureParabolaEffectGenerator : public MEffectGenerator {
	public :
		MAttackCreatureParabolaEffectGenerator()	{}
		~MAttackCreatureParabolaEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_CREATURE_PARABOLA; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackCreatureParabolaEffectGenerator	g_AttackCreatureParabolaEffectGenerator;

#endif

