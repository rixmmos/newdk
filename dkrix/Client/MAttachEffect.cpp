//----------------------------------------------------------------------
// MAttachEffect.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include <math.h>
#include "MMovingEffect.h"
#include "MAttachEffect.h"
#include "MTopView.h"
#include "MEffectSpriteTypeTable.h"
#include "DebugLog.h"

//----------------------------------------------------------------------
// 
// constructor/destructor
//
//----------------------------------------------------------------------

MAttachEffect::MAttachEffect(TYPE_EFFECTSPRITETYPE type, DWORD last, DWORD linkCount)
: MMovingEffect( (type < g_pEffectSpriteTypeTable->GetSize()? (*g_pEffectSpriteTypeTable)[type].BltType : BLT_EFFECT) )
{
	
	//m_EffectType	= EFFECT_ATTACH;

	
	m_CreatureID	= OBJECTID_NULL;

	m_bEffectSprite = true;

	
	m_EffectSpriteType	= type;
	
	
	
	if(last == 0xFFFF)
		m_EndFrame			= 0xFFFFFFFF;
	else
	// 2004, 12, 9, sobeit add end
		m_EndFrame			= g_CurrentFrame + last - 1;
	
	
	
	m_bEffectColorPart = ADDON_NULL;

	// linkCount
	if (linkCount==MAX_LINKCOUNT)
	{
		m_EndLinkFrame = m_EndFrame;
	}
	else
	{
		m_EndLinkFrame = g_CurrentFrame + linkCount - 1;
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (type < g_pEffectSpriteTypeTable->GetSize())
	{
		TYPE_FRAMEID	frameID = (*g_pEffectSpriteTypeTable)[type].FrameID;
		BYTE			maxFrame;

		// BLT_NORMAL
		BLT_TYPE bltType = (*g_pEffectSpriteTypeTable)[type].BltType;
		maxFrame = g_pTopView->GetMaxEffectFrame(bltType, frameID);

        LOG_INFO("[EFFECT CREATE] type=%d, FrameID=%d, BltType=%d, maxFrame=%d",
            type, frameID, (int)bltType, (int)maxFrame);

		SetFrameID( frameID, maxFrame );

		
		if (m_BltType == BLT_EFFECT)
		{
			m_Light = g_pTopView->m_EffectAlphaFPK[m_FrameID][m_Direction][m_CurrentFrame].GetLight();
		}
		else
		{
			m_Light = 0;
		}
	}
	else
	{
		SetFrameID( 0, 1 );
		m_Light = 0;
	}
}

MAttachEffect::~MAttachEffect()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Move
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
MAttachEffect::SetAttachCreatureID(TYPE_OBJECTID id)
{ 
	MCreature* pCreature = g_pZone->GetCreature( id );

	if (pCreature==NULL)
	{
		pCreature = g_pZone->GetFakeCreature( id );

		if (pCreature==NULL)
		{
			MItem* pItem = g_pZone->GetItem( id );

			if (pItem!=NULL
				&& pItem->GetItemClass()==ITEM_CLASS_CORPSE)
			{
				pCreature = ((MCorpse*)pItem)->GetCreature();
			}			
		}
	}

	SetAttachCreature( pCreature );
}

//----------------------------------------------------------------------
// TraceCreature
//----------------------------------------------------------------------
bool
MAttachEffect::SetAttachCreature(MCreature* pCreature)
{	
	
	if (pCreature == NULL)
	{
		m_EndFrame = 0;
		return false;	
	}

	m_CreatureID = pCreature->GetID(); 

	
	POINT point;// = MTopView::MapToPixel(pCreature->GetX(), pCreature->GetY());
	//point.x += pCreature->GetSX();
	//point.y += pCreature->GetSY();
	point.x = pCreature->GetPixelX();
	point.y = pCreature->GetPixelY();

	
	m_PixelX = point.x;
	m_PixelY = point.y;
	m_PixelZ = pCreature->GetZ();

	//--------------------------------
	
	//--------------------------------
	AffectPosition();

	
	//#ifdef	OUTPUT_DEBUG
	//	sprintf(g_pDebugMessage->GetCurrent(), "Set AttachEffect : Type=%d, Light=%d, (%d, %d)", pEffect->GetFrameID(), m_Light, m_X, m_Y);	
	//	g_pDebugMessage->Next();
	//#endif

	return true;
}

//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------
bool
MAttachEffect::Update()
{	
	if (g_CurrentFrame < m_EndFrame)
	{	
		
		NextFrame();

		
		//m_Count--;

		if (m_BltType == BLT_EFFECT)
		{
			m_Light = g_pTopView->m_EffectAlphaFPK[m_FrameID][m_Direction][m_CurrentFrame].GetLight();
		}

		return true;
	}

	
	
	//AttachCreature();

	return false;
}
