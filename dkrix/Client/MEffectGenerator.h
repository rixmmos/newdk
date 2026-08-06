//----------------------------------------------------------------------
// MEffectGenerator.h
//----------------------------------------------------------------------
//

//

//

//










//
//----------------------------------------------------------------------

#ifndef	__MEFFECTGENERATOR_H__
#define	__MEFFECTGENERATOR_H__

#include "MTypeDef.h"
#include "MEffectGeneratorDef.h"
#include "DrawTypeDef.h"
#include "MEffectTarget.h"
class MEffect;

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class EFFECTGENERATOR_INFO
{
	public :
		TYPE_ACTIONINFO			nActionInfo;				
		MEffectTarget*			pEffectTarget;				
		TYPE_EFFECTSPRITETYPE	effectSpriteType;			
		int						x0, y0, z0;					
		int						x1, y1, z1;					
		TYPE_OBJECTID			creatureID;					
		BYTE					direction;					
		BYTE					step;						
		WORD					count;						
		WORD					linkCount;					
		BYTE					power;						
		MEffect*				pPreviousEffect;			
		BYTE					temp1,temp2;				
};

//----------------------------------------------------------------------
//
//		 Effect Generator
//
//----------------------------------------------------------------------
class MEffectGenerator {
	public :
		MEffectGenerator()	{}
		virtual ~MEffectGenerator() {}

		virtual TYPE_EFFECTGENERATORID		GetID()		{ return EFFECTGENERATORID_NULL; }

		virtual bool	Generate(	const EFFECTGENERATOR_INFO& egInfo	) = 0;

	protected :
};

#endif
