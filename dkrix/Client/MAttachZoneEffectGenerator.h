//----------------------------------------------------------------------
// MAttachZoneEffectGenerator.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MATTACHZONEEFFECTGENERATOR_H__
#define	__MATTACHZONEEFFECTGENERATOR_H__

#include "MEffectGenerator.h"

class MAttachZoneEffectGenerator : public MEffectGenerator {
	public :
		MAttachZoneEffectGenerator()	{}
		~MAttachZoneEffectGenerator() {}

		TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_ATTACH_ZONE; }

		
		bool	Generate( const EFFECTGENERATOR_INFO& egInfo );
};

//
//extern MAttachZoneEffectGenerator	g_StopZoneEffectGenerator;

#endif

