//----------------------------------------------------------------------
// MEffect.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MEffect.h"
#include "MTopView.h"
#include <fstream>
#include "DebugInfo.h"

//----------------------------------------------------------------------
// Init Static Members
//----------------------------------------------------------------------
TYPE_OBJECTID	MEffect::s_ID	= 0;

#ifdef OUTPUT_DEBUG
	int g_EffectCount = 0;
#endif

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
MEffect::MEffect(BYTE bltType)
: CAnimationFrame(bltType)
{
	// instance ID
	m_ID			= s_ID++;

	m_ObjectType	= TYPE_EFFECT;
//	m_EffectType	= EFFECT_SECTOR;

	m_Direction		= 0;

	m_PixelX		= 0;
	m_PixelY		= 0;
	m_PixelZ		= 0;
	m_StepPixel		= 0;

	
	m_pEffectTarget = NULL;

	m_Light = 0;

	m_EndFrame = 0;
	m_EndLinkFrame = 0;
	m_DelayFrame = 0;

	m_bMulti = false;
	m_bDrawSkip = false;

	
	m_pResources = nullptr;

	#ifdef OUTPUT_DEBUG
		g_EffectCount++;
	#endif
}


MEffect::MEffect(BYTE bltType, EffectResourceContainer* resources)
: CAnimationFrame(bltType)
{
	// instance ID
	m_ID			= s_ID++;

	m_ObjectType	= TYPE_EFFECT;

	m_Direction		= 0;

	m_PixelX		= 0;
	m_PixelY		= 0;
	m_PixelZ		= 0;
	m_StepPixel		= 0;

	
	m_pEffectTarget = NULL;

	m_Light = 0;

	m_EndFrame = 0;
	m_EndLinkFrame = 0;
	m_DelayFrame = 0;

	m_bMulti = false;
	m_bDrawSkip = false;

	
	m_pResources = resources;

	#ifdef OUTPUT_DEBUG
		g_EffectCount++;
	#endif
}

MEffect::~MEffect()
{
	if (m_pEffectTarget!=NULL)
	{
		//if (!m_pEffectTarget->IsEnd())
		delete m_pEffectTarget;
		m_pEffectTarget = NULL;
	}

	#ifdef OUTPUT_DEBUG
		g_EffectCount--;
	#endif
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Set Count
//----------------------------------------------------------------------
void			
MEffect::SetCount(DWORD last, DWORD linkCount)
{ 
	m_EndFrame = g_CurrentFrame + last - 1; 

	if (linkCount==MAX_LINKCOUNT)
	{
		m_EndLinkFrame = m_EndFrame;
	}
	else
	{
		m_EndLinkFrame = g_CurrentFrame + linkCount - 1;
	}
}
//----------------------------------------------------------------------
// Set Link
//----------------------------------------------------------------------
void			
MEffect::SetLink(TYPE_ACTIONINFO nActionInfo, MEffectTarget* pEffectTarget)
{
	#ifdef OUTPUT_DEBUG
		if (pEffectTarget==NULL)
		{
			DEBUG_ADD_FORMAT("[MEffect]SetLink: ai=%d, effectTarget=NULL", nActionInfo);
		}
		else
		{
			DEBUG_ADD_FORMAT("[MEffect]SetLink: ai=%d, effectTargetID=%d", nActionInfo, (int)pEffectTarget->GetEffectID());
		}
	#endif
	
	if (m_pEffectTarget!=NULL)
	{
		DEBUG_ADD_FORMAT("[MEffect]Delete EffectTarget for New One: eid=%d", (int)m_pEffectTarget->GetEffectID());
		
		delete m_pEffectTarget;
		m_pEffectTarget = NULL;
	}

	m_nActionInfo	= nActionInfo;
	m_pEffectTarget	= pEffectTarget;
}

//----------------------------------------------------------------------
// SetEffectTargetNULL
//----------------------------------------------------------------------
void
MEffect::SetEffectTargetNULL()
{
	m_pEffectTarget = NULL;
}


//----------------------------------------------------------------------
// Set Position(x,y)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void			
MEffect::SetPixelPosition(int x, int y, int z)
{
	m_PixelX = x;
	m_PixelY = y;
	m_PixelZ = z;

	AffectPosition();
}


//----------------------------------------------------------------------
// Affect Position
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MEffect::AffectPosition()
{
	
	m_X = MTopView::PixelToMapX( m_PixelX );
	m_Y = MTopView::PixelToMapY( m_PixelY );
}


//----------------------------------------------------------------------
// SetFrameID
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MEffect::SetFrameID(TYPE_FRAMEID FrameID, BYTE max)
{ 
	CAnimationFrame::SetFrameID(FrameID, max);

	
	m_Light = g_pTopView->GetEffectLight((BLT_TYPE)m_BltType, FrameID, m_Direction, 0);
}

//----------------------------------------------------------------------
// SetPosition(x,y)
//----------------------------------------------------------------------
void		
MEffect::SetPosition(TYPE_SECTORPOSITION x, TYPE_SECTORPOSITION y)
{
	m_X = x; 
	m_Y = y; 
	
	m_PixelX = MTopView::MapToPixelX(x); 
	m_PixelY = MTopView::MapToPixelY(y);
}

//----------------------------------------------------------------------
// SetX
//----------------------------------------------------------------------
void		
MEffect::SetX(TYPE_SECTORPOSITION x)
{ 
	m_X = x; 
	m_PixelX = MTopView::MapToPixelX(x); 
}

//----------------------------------------------------------------------
// SetY
//----------------------------------------------------------------------
void		
MEffect::SetY(TYPE_SECTORPOSITION y)
{ 
	m_Y = y; 
	m_PixelY = MTopView::MapToPixelY(y); 
}

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
MEffect::Update()
{
	
	NextFrame();

	m_Light = g_pTopView->GetEffectLight((BLT_TYPE)m_BltType, m_FrameID, m_Direction, m_CurrentFrame);
		
	
	return g_CurrentFrame < m_EndFrame;
}

void
MEffect::SetDelayFrame(DWORD frame)
{
	m_DelayFrame = g_CurrentFrame + frame; 
}

bool
MEffect::IsDelayFrame() const
{
	return g_CurrentFrame < m_DelayFrame;
}

void
MEffect::SetWaitFrame(DWORD frame)
{
	m_dwWaitFrame = g_CurrentFrame + frame; 
}

bool
MEffect::IsWaitFrame() const
{
	return g_CurrentFrame < m_dwWaitFrame;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MEffect::SetResourceContainer(EffectResourceContainer* resources)
{
	m_pResources = resources;
}
