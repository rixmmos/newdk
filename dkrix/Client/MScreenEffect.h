//----------------------------------------------------------------------
// MScreenEffect.h
//----------------------------------------------------------------------
//

//

//  --> m_PixelX, m_PixelY
//


//
//----------------------------------------------------------------------

#ifndef	__MSCREENEFFECT_H__
#define	__MSCREENEFFECT_H__

#include "MEffect.h"

class MScreenEffect : public MEffect {
	public :
		MScreenEffect(BYTE bltType);
		~MScreenEffect();

		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_SCREEN; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		
		static void		SetScreenBasis(int bx, int by);

		
		void			SetScreenPosition(int x, int y);

		//--------------------------------------------------------
		// Get
		//--------------------------------------------------------
		int				GetScreenX()	{ return (int)m_PixelX + m_ScreenBasisX; }
		int				GetScreenY()	{ return (int)m_PixelY + m_ScreenBasisY; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool	Update();

	public :
		
		static	int		m_ScreenBasisX;
		static	int		m_ScreenBasisY;
};

#endif

