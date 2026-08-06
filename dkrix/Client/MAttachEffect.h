//----------------------------------------------------------------------
// MAttachEffect.h
//----------------------------------------------------------------------
//

//

//

//
//----------------------------------------------------------------------

#ifndef	__MATTACHEFFECT_H__
#define	__MATTACHEFFECT_H__

#pragma warning(disable:4786)

#include "MMovingEffect.h"
#include "MTypeDef.h"
//#include "EffectSpriteTypeDef.h"
#include <list>
class MCreature;

class MAttachEffect : public MMovingEffect {
	public :
		MAttachEffect(TYPE_EFFECTSPRITETYPE type, DWORD last, DWORD linkCount=MAX_LINKCOUNT);
		~MAttachEffect();

		virtual EFFECT_TYPE		GetEffectType()	const	{ return EFFECT_ATTACH; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		void					SetAttachCreatureID(TYPE_OBJECTID id);	
		bool					SetAttachCreature(MCreature* pCreature);		
		TYPE_OBJECTID			GetAttachCreatureID()					{ return m_CreatureID; }

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		bool					IsEffectSprite() const	{ return m_bEffectSprite; }
		bool					IsEffectColor() const	{ return !m_bEffectSprite; }
		void					SetEffectSprite(TYPE_EFFECTSPRITETYPE es)	{ m_EffectSpriteType = es; m_bEffectSprite = true; }
		void					SetEffectColor(WORD colorSet)				{ m_EffectColor = colorSet; m_bEffectSprite = false; }
		
		void					SetEffectColorPart(ADDON part)				{ m_bEffectColorPart = part; }
		ADDON					GetEffectColorPart() const					{ return m_bEffectColorPart; }

		TYPE_EFFECTSPRITETYPE	GetEffectSpriteType() const	{ return m_EffectSpriteType; }
		WORD					GetEffectColor() const		{ return m_EffectColor; }		
		
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		virtual bool			Update();

		
	protected :
		TYPE_OBJECTID				m_CreatureID;

		bool						m_bEffectSprite;		

		union {
			TYPE_EFFECTSPRITETYPE	m_EffectSpriteType;		
			WORD					m_EffectColor;			
		};

		ADDON						m_bEffectColorPart;		
};


typedef	std::list<MAttachEffect*>	ATTACHEFFECT_LIST;

#endif

