//----------------------------------------------------------------------
// MMeteorDropEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MMETEORDROPEFFECTGENERATOR_H__
#define	__MMETEORDROPEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MMeteorDropEffectGenerator : public MEffectGenerator {
	public :
		MMeteorDropEffectGenerator()	{}
		~MMeteorDropEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_METEOR_DROP; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MFallingEffectGenerator	g_FallingEffectGenerator;

#endif

