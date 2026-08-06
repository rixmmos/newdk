//----------------------------------------------------------------------
// MLinearEffect.h
//----------------------------------------------------------------------

#ifndef	__MLINEAREFFECT_H__
#define	__MLINEAREFFECT_H__

#include "MMovingEffect.h"

class MLinearEffect : public MMovingEffect {
	public :
		MLinearEffect(BYTE bltType);
		~MLinearEffect();

		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_LINEAR; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual void		SetTarget(int x, int y, int z, WORD stepPixel);
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool		Update();
	

	protected :
		
		int			m_TargetX;
		int			m_TargetY;
		int			m_TargetZ;

		
		float		m_StepX;
		float		m_StepY;
		float		m_StepZ;		
	
		
		float		m_Len;	
};

#endif

