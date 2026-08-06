//----------------------------------------------------------------------
// MAttachOrbitEffect.h
//----------------------------------------------------------------------
//

//



//




//

//




//
//----------------------------------------------------------------------

#ifndef	__MATTACH_ORBIT_EFFECT_H__
#define	__MATTACH_ORBIT_EFFECT_H__

#pragma warning(disable:4786)


#define	MAX_EFFECT_ORBIT_TYPE		3
#define	MAX_EFFECT_ORBIT_STEP		64

#include "MAttachEffect.h"



class MAttachOrbitEffect : public MAttachEffect {
	public :
		MAttachOrbitEffect(TYPE_EFFECTSPRITETYPE type, DWORD last, int orbit_type, int orbit_step = -1, DWORD linkCount=MAX_LINKCOUNT);
		~MAttachOrbitEffect();

		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_ATTACH_ORBIT; }
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool			Update();

		//void					NextOrbitStep()		{ m_OrbitStep = ++m_OrbitStep % MAX_EFFECT_ORBIT_STEP; }
		void					NextOrbitStep()		{ m_OrbitStep = ++m_OrbitStep & 0x0000003F; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual int		GetPixelX() const	{ return (int)m_PixelX + s_OrbitPosition[m_Type][m_OrbitStep].x; }
		virtual int		GetPixelY() const	{ return (int)m_PixelY + s_OrbitPosition[m_Type][m_OrbitStep].y; }

		//--------------------------------------------------------
		// Get Orbit X,Y
		//--------------------------------------------------------
		const POINT&	GetOrbitPosition() const	{ return s_OrbitPosition[m_Type][m_OrbitStep]; }
		
		//--------------------------------------------------------
		// Init OrbitPosition
		//--------------------------------------------------------
		static void		InitOrbitPosition();
	
	public :
		bool					m_bRun;
		int						m_OrbitStep;		
		int						m_Type;

		
		static POINT			s_OrbitPosition[MAX_EFFECT_ORBIT_TYPE][MAX_EFFECT_ORBIT_STEP];		
};


typedef	std::list<MAttachEffect*>	ATTACHEFFECT_LIST;

#endif

