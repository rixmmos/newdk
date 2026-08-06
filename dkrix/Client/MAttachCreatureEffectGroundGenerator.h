//----------------------------------------------------------------------
// MAttachCreatureEffectGroundGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACHCREATUREEFFECTGROUNDGENERATOR_H__
#define	__MATTACHCREATUREEFFECTGROUNDGENERATOR_H__

#include "MEffectGenerator.h"

class MAttachCreatureEffectGroundGenerator : public MEffectGenerator {
	public :
		MAttachCreatureEffectGroundGenerator() {}
		~MAttachCreatureEffectGroundGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACH_CREATURE_GROUND; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttachCreatureEffectGenerator	g_AttachCreatureEffectGenerator;

#endif

