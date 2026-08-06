//----------------------------------------------------------------------
// MAttackCreatureEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACKCREATUREEFFECTGENERATOR_H__
#define	__MATTACKCREATUREEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttackCreatureEffectGenerator : public MEffectGenerator {
	public :
		MAttackCreatureEffectGenerator()	{}
		~MAttackCreatureEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACK_CREATURE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttackCreatureEffectGenerator	g_AttackCreatureEffectGenerator;

#endif

