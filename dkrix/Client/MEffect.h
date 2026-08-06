//----------------------------------------------------------------------
// MEffect.h
//-------------------------------------------------------------------------
// Effects displayed on the screen
//----------------------------------------------------------------------------------
//
// < What is an Effect?
// - Effects displayed on the screen.
// - Excluding characters, items, buildings, obstacles, etc. (what else?),
// these are animations.
// - All Effects are expressed using CAlphaSprite and alpha channel blending.
// - Things like a character swinging a sword or a priest's aura
// are not included in the Effects discussed here.
// - An Effect can be considered an independent object.
///
// < Effect Types >
// = Missile
// - Pixel-based movement.
// - Animated from the launch location to the target location
// / / Moves at a constant speed (pixels).
// - May or may not be guided missiles. // - May be destroyed by collision with obstacles, characters, buildings, etc.
// - Disappears upon reaching the target location.
//
// = Tile Magic
// - Animated on a specific Tile.
// - Size may be larger than the Tile.
// - Output is displayed last on the Tile.
// - Disappears after a certain number of frames.
//
//----------------------------------------------------------------------
// - All Effects must belong to a Sector within a Zone.
// Since the output order must be sorted by y-coordinate,
// we decided to place them in a Sector to output them according to the Sector's Object output.
//
// - The Effect lasts for a certain amount of time.
// That is, we set a counter so that it ends when it reaches 0.
//----------------------------------------------------------------------
//
// The Effect is not saved to a file. //
//----------------------------------------------------------------------
/* class hierarchy

MEffect --+-- MMovingEffect ---- ....

MEffect: Effect anchored to a Tile
*/
//----------------------------------------------------------------------

#ifndef	__MEFFECT_H__
#define	__MEFFECT_H__

#pragma warning(disable:4786)

#include "framelib/CAnimationFrame.h"
#include "MTypeDef.h"
#include "MObject.h"
#include "MEffectTarget.h"
#include "EffectResourceContainer.h"

#include <fstream>
using namespace std;



extern DWORD	g_CurrentFrame;


class MEffect : public MObject, public CAnimationFrame {
	
	public :
		enum EFFECT_TYPE 
		{
			EFFECT_SECTOR = 0,		
			EFFECT_MOVING,			
			EFFECT_LINEAR,			
			EFFECT_GUIDANCE,		
			EFFECT_HOMING,			
			EFFECT_PARABOLA,		
			EFFECT_ATTACH,			
			EFFECT_SCREEN,			
			EFFECT_CHASE,			
			EFFECT_ATTACH_ORBIT,	
		};

	public :
		// New constructor: supports dependency injection (requires explicit resource container)
		MEffect(BYTE bltType, EffectResourceContainer* resources);

		// Old constructor: maintain backward compatibility (no resource container)
		MEffect(BYTE bltType);

		~MEffect();

		//--------------------------------------------------------
		// Resource container management (newly added)
		//--------------------------------------------------------
		// Set resource container (for dependency injection)
		void SetResourceContainer(EffectResourceContainer* resources);

		// Get resource container
		EffectResourceContainer* GetResourceContainer() const { return m_pResources; }

		//--------------------------------------------------------
		// Set FrameID
		//--------------------------------------------------------
		void			SetFrameID(TYPE_FRAMEID FrameID, BYTE max);

		//--------------------------------------------------------
		// GetEffectType
		//--------------------------------------------------------
		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_SECTOR; }

		void SetEst( int est ) { m_est = est; }
		int GetEst() const { return m_est; }
		//--------------------------------------------------------
		//	Is Selectable
		//--------------------------------------------------------
		virtual bool		IsSelectable() const		{ return false; }		

		//--------------------------------------------------------
		
		//--------------------------------------------------------		
		
		void			SetCount(DWORD last, DWORD linkCount=0xFFFF);
		DWORD			GetEndFrame() const			{ return m_EndFrame; }
		DWORD			GetEndLinkFrame() const		{ return m_EndLinkFrame; }
		bool			IsEnd() const				{ return g_CurrentFrame >= m_EndFrame; }

		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool	Update();
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------	
		void			SetPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y);
		void			SetX(TYPE_SECTORPOSITION x);
		void			SetY(TYPE_SECTORPOSITION y);
		void			SetZ(int z)						{ m_PixelZ = (float)z; }
		void			SetDirection(BYTE d)			{ m_Direction = d; }
		BYTE			GetDirection() const			{ return m_Direction; }
		void			SetLight(char light)			{ m_Light = light; }

	
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void			SetPixelPosition(int x, int y, int z);

		//--------------------------------------------------------
		// Get Functions
		//--------------------------------------------------------
		char			GetLight() const	{ return m_Light; }
		virtual int		GetPixelX() const	{ return (int)m_PixelX; }
		virtual int		GetPixelY() const	{ return (int)m_PixelY; }
		virtual int		GetPixelZ() const	{ return (int)m_PixelZ; }

		//--------------------------------------------------------
		// Power
		//--------------------------------------------------------
		void			SetPower(BYTE power)	{ m_Power = power; }
		BYTE			GetPower() const		{ return m_Power; }

		//--------------------------------------------------------
		// Step
		//--------------------------------------------------------
		void			SetStepPixel(WORD step)	{ m_StepPixel=step; }
		WORD			GetStepPixel() const	{ return m_StepPixel; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void			SetLink(TYPE_ACTIONINFO nActionInfo, MEffectTarget* pEffectTarget);		

		void			SetEffectTargetNULL();

		
		TYPE_ACTIONINFO	GetActionInfo()	const { return m_nActionInfo; }

		
		MEffectTarget*	GetEffectTarget()		{ return m_pEffectTarget; }
		int				GetLinkSize()			{ return (m_pEffectTarget==NULL || m_pEffectTarget->IsEnd())? 0 : m_pEffectTarget->GetCurrentPhase(); }	

		void			SetMulti(bool bMulti)	{ m_bMulti = bMulti; }
		const bool		IsMulti()				{ return m_bMulti; }

		void			SetDelayFrame(DWORD frame);
		bool			IsDelayFrame() const;

		
		void			SetWaitFrame(DWORD frame);
		bool			IsWaitFrame() const;
		// 2004, 10, 15, sobeit add end

		
		bool			IsSkipDraw() const			{ return m_bDrawSkip; }
		void			SetDrawSkip(bool bSkip)		{ m_bDrawSkip = bSkip;}
		// 2004, 9, 30, sobeit add end
	protected :
		
		void			AffectPosition();



	protected :
		DWORD					m_DelayFrame;
		
		int	m_est;

		
		DWORD					m_EndFrame;
		DWORD					m_EndLinkFrame;	
		
		BYTE					m_Direction;	

		
		char					m_Light;

		
		float			m_PixelX;
		float			m_PixelY;
		float			m_PixelZ;
		WORD			m_StepPixel;

		BYTE			m_Power;

		
		TYPE_ACTIONINFO			m_nActionInfo;
		MEffectTarget*			m_pEffectTarget;

		static TYPE_OBJECTID	s_ID;			

		
		bool			m_bMulti;

		bool			m_bDrawSkip;
		DWORD			m_dwWaitFrame;

		// Newly added: resource container (dependency injection)
		EffectResourceContainer*	m_pResources;
};

//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MSelectableEffect : public MEffect {
	public :
		MSelectableEffect(BYTE bltType) : MEffect(bltType) {}
		~MSelectableEffect() {}

		//--------------------------------------------------------
		//	Is Selectable
		//--------------------------------------------------------
		bool		IsSelectable() const		{ return true; }
};

#endif



