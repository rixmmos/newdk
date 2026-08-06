//----------------------------------------------------------------------
// MGuidanceEffect.h
//----------------------------------------------------------------------

#ifndef	__MGUIDANCEEFFECT_H__
#define	__MGUIDANCEEFFECT_H__

#include "MLinearEffect.h"
#include "MTypeDef.h"

class MGuidanceEffect : public MLinearEffect {
	public :
		MGuidanceEffect(BYTE bltType);
		~MGuidanceEffect();
		
		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_GUIDANCE; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void				SetTraceCreatureID(TYPE_OBJECTID id);
		TYPE_OBJECTID		GetTraceCreatureID()					{ return m_CreatureID; }
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool		Update();

	protected :
		virtual bool		TraceCreature();		

	protected :
		TYPE_OBJECTID	m_CreatureID;
};

#endif