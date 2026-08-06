//----------------------------------------------------------------------
// MHomingEffect.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include <math.h>
#include "MHomingEffect.h"
#include "MCreature.h"
#include "MathTable.h"
#include "MZone.h"
#include "MTopView.h"
#include "SkillDef.h"
//----------------------------------------------------------------------
// 
// constructor/destructor
//
//----------------------------------------------------------------------

MHomingEffect::MHomingEffect(BYTE bltType, int currentAngle, int turnAngle)
: MGuidanceEffect(bltType)
{		
	m_RadCurrent = MathTable::GetAngle360( currentAngle );		

	m_RadStep = MathTable::GetAngle360( turnAngle );			
}

MHomingEffect::~MHomingEffect()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set Target
//----------------------------------------------------------------------
void		
MHomingEffect::SetTarget(int x, int y, int z, WORD speed)
{
	
	//MLinearEffect::SetTarget(x, y, z, speed);
	m_TargetX = x;
	m_TargetY = y;
	m_TargetZ = z;
	m_StepPixel = speed;

	
	m_StepZ = (m_TargetZ - m_PixelZ) / 16.0f;
	
	CalculateAngle();
}

//----------------------------------------------------------------------
// TraceCreature
//----------------------------------------------------------------------
bool
MHomingEffect::TraceCreature()
{
	MCreature* pCreature = g_pZone->GetCreature( m_CreatureID );

	
	if (pCreature == NULL)
	{
		m_CreatureID = OBJECTID_NULL;
		m_EndFrame = 0;
		return false;
	}

	
	m_TargetX = pCreature->GetPixelX();
	m_TargetY = pCreature->GetPixelY();
	
	return true;
}

//----------------------------------------------------------------------
// CalculateAngle
//----------------------------------------------------------------------
void
MHomingEffect::CalculateAngle()
{
	//--------------------------------------------------
	
	
	//--------------------------------------------------
	int targetAngle = MathTable::GetAngleToTarget(m_PixelX, m_PixelY, m_TargetX, m_TargetY);

	int dir = MathTable::GetAngleDir( m_RadCurrent, targetAngle );

	m_RadStep = dir * abs(m_RadStep);		// +-speed	
}

//----------------------------------------------------------------------
// SetDirectionByAngle
//----------------------------------------------------------------------
void
MHomingEffect::SetDirectionByAngle()
{
	if (m_RadCurrent < MathTable::ANGLE_180)
	{
		if (m_RadCurrent < MathTable::ANGLE_90)
		{
			if (m_RadCurrent < MathTable::ANGLE_30)
			{
				m_Direction = DIRECTION_RIGHT;
			}
			else if (m_RadCurrent < MathTable::ANGLE_60)
			{
				m_Direction = DIRECTION_RIGHTUP;
			}
			else
			{
				m_Direction = DIRECTION_UP;
			}
		}
		else
		{
			if (m_RadCurrent < MathTable::ANGLE_120)
			{
				m_Direction = DIRECTION_UP;
			}
			else if (m_RadCurrent < MathTable::ANGLE_150)
			{
				m_Direction = DIRECTION_LEFTUP;
			}
			else
			{
				m_Direction = DIRECTION_LEFT;
			}
		}
	}
	else
	{
		if (m_RadCurrent < MathTable::ANGLE_270)
		{
			if (m_RadCurrent < MathTable::ANGLE_210)
			{
				m_Direction = DIRECTION_LEFT;
			}
			else if (m_RadCurrent < MathTable::ANGLE_240)
			{
				m_Direction = DIRECTION_LEFTDOWN;
			}
			else
			{
				m_Direction = DIRECTION_DOWN;
			}
		}
		else
		{
			if (m_RadCurrent < MathTable::ANGLE_300)
			{
				m_Direction = DIRECTION_DOWN;
			}
			else if (m_RadCurrent < MathTable::ANGLE_330)
			{
				m_Direction = DIRECTION_RIGHTDOWN;
			}
			else
			{
				m_Direction = DIRECTION_RIGHT;
			}
		}
	}
}

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------
bool
MHomingEffect::Update()
{	
	if (g_CurrentFrame < m_EndFrame)
	{
		if (GetActionInfo() != SKILL_CLIENT_HALO_ATTACK )
		{ 
			if(!TraceCreature())
				return false;

			CalculateAngle();
		}

		

		//--------------------------------
		
		//--------------------------------
		
		m_RadCurrent += m_RadStep;
		m_RadCurrent &= MathTable::MAX_ANGLE_1;		
		
		m_PixelX += ((MathTable::FCos(m_RadCurrent)*m_StepPixel)>>16);
		m_PixelY -= ((MathTable::FSin(m_RadCurrent)*m_StepPixel)>>16);
		m_PixelZ += m_StepZ;

		if (fabs(m_PixelZ-m_TargetZ) < m_StepZ)
		{
			m_PixelZ = m_TargetZ;
			m_StepZ = 0;
		}

		//------------------------------------------
		
		//------------------------------------------
		if (fabs(m_PixelX-m_TargetX)<m_StepPixel &&
			fabs(m_PixelY-m_TargetY)<m_StepPixel &&
			GetActionInfo() != SKILL_CLIENT_HALO_ATTACK 
			)		
		{
			m_PixelX = (float)m_TargetX;
			m_PixelY = (float)m_TargetY;
			m_PixelZ = (float)m_TargetZ;

			m_StepX = 0;
			m_StepY = 0;
			m_StepZ = 0;

			m_RadStep = 0;

			//------------------------------------------
			
			//------------------------------------------
			m_EndFrame = 0;

			return false;
		}
		else
		{
			//--------------------------------
			
			//--------------------------------
		//	SetDirectionByAngle();
		}

		//--------------------------------
		
		//--------------------------------
		AffectPosition();

		//--------------------------------
		
		//--------------------------------
		NextFrame();

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
