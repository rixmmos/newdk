//----------------------------------------------------------------------
// MChaseEffect.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MCHASEEFFECT_H__
#define	__MCHASEEFFECT_H__

#include "MGuidanceEffect.h"
#include "MTypeDef.h"

class MChaseEffect : public MGuidanceEffect {
	public :
		MChaseEffect(BYTE bltType);
		~MChaseEffect();

		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_CHASE; }		
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		bool				IsChaseOver() const		{ return m_bChaseOver; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool		Update();

	protected :
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		bool				m_bChaseOver;
};

#endif