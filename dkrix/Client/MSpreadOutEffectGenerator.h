//----------------------------------------------------------------------
// MSpreadOutEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MSPREADOUTEFFECTGENERATOR_H__
#define	__MSPREADOUTEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MSpreadOutEffectGenerator : public MEffectGenerator {
	public :
		MSpreadOutEffectGenerator()	{}
		~MSpreadOutEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_SPREAD_OUT; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MSpreadOutEffectGenerator	g_SpreadOutEffectGenerator;

#endif

