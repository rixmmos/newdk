//----------------------------------------------------------------------
// MChaseEffect.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include <math.h>
#include "MChaseEffect.h"
#include "MCreature.h"
#include "MZone.h"
#include "MTopView.h"

//----------------------------------------------------------------------
// 
// constructor/destructor
//
//----------------------------------------------------------------------

MChaseEffect::MChaseEffect(BYTE bltType)
: MGuidanceEffect(bltType)
{
	m_bChaseOver = false;
}

MChaseEffect::~MChaseEffect()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MChaseEffect::Update()
{	
	//if (g_CurrentFrame < m_EndFrame)
	{
		
		
		if (m_CreatureID!=OBJECTID_NULL && !TraceCreature())
			return false;

		//--------------------------------
		
		//--------------------------------
		NextFrame();

		//--------------------------------
		
		//--------------------------------
		
		m_PixelX += m_StepX;
		m_PixelY += m_StepY;
		m_PixelZ += m_StepZ;

		//------------------------------------------
		
		//------------------------------------------
		if (fabs(m_PixelX-m_TargetX)<m_StepPixel &&
			fabs(m_PixelY-m_TargetY)<m_StepPixel &&
			fabs(m_PixelZ-m_TargetZ)<m_StepPixel)
		{
			m_PixelX = (float)m_TargetX;
			m_PixelY = (float)m_TargetY;
			m_PixelZ = (float)m_TargetZ;

			m_StepX = 0;
			m_StepY = 0;
			m_StepZ = 0;

			m_bChaseOver = true;

			return true;
		}
		else
		{
			m_bChaseOver = false;
		}

		//--------------------------------
		
		//--------------------------------
		AffectPosition();

		
		if (m_BltType == BLT_EFFECT)
		{
			m_Light = g_pTopView->m_EffectAlphaFPK[m_FrameID][m_Direction][m_CurrentFrame].GetLight();
		}

		//--------------------------------
		
		//--------------------------------
		//m_Count--;

		return true;
	}

	

	return false;
}
