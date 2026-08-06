//----------------------------------------------------------------------
// MCreatureWear.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MCREATUREWEAR_H__
#define	__MCREATUREWEAR_H__

#include "MTypeDef.h"
#include "MCreature.h"
#include "MItem.h"
#include "MemoryPool.h"

//----------------------------------------------------------------------
// define
//----------------------------------------------------------------------

class MCreatureWear : public MCreature {
	public :
		//----------------------------------------------------
		
		//----------------------------------------------------
		struct ADDON_INFO 
		{
			BOOL				bAddon;			
			TYPE_FRAMEID		FrameID;		
			ITEM_CLASS	ItemClass;		// item class
			TYPE_ITEMTYPE		ItemType;		// item type
			WORD				ColorSet1;		// colorset1
			WORD				ColorSet2;		// colorset2
			BOOL				bEffectColor;	
			WORD				EffectColorSet;	// EffectColor			
		};

	public :
		MCreatureWear();
		virtual ~MCreatureWear();
		
		void* operator new( size_t size )
		{
			return g_CreatureWearMemoryPool.Alloc();
		}

		void operator delete( void* pmem )
		{
			g_CreatureWearMemoryPool.Free( pmem );
		}

		virtual CLASS_TYPE	GetClassType() const	{ return CLASS_CREATUREWEAR; }

		BOOL		IsWear() const					{ return TRUE; }

		//------------------------------------------------------
		
		//------------------------------------------------------
		virtual void	SetCreatureType(TYPE_CREATURETYPE type);


		//------------------------------------------------------
		
		//------------------------------------------------------			
		
		void			SetAddonHair(TYPE_FRAMEID id, WORD cs1);
		TYPE_FRAMEID	GetHairFrameID()			{ return m_HairFrameID; }
		
		
		bool			RemoveAddon(int Addon);

		
		//BOOL			IsExistAddon(int Addon) const		{ return m_Addon[Addon].bAddon; }
		//TYPE_FRAMEID	GetAddonFrameID(int Addon) const	{ return m_Addon[Addon].FrameID; }
		//ITEM_CLASS	GetAddonItemClass(int Addon) const	{ return m_Addon[Addon].ItemClass; }
		//TYPE_ITEMTYPE	GetAddonItemType(int Addon) const	{ return m_Addon[Addon].ItemType; }
		const ADDON_INFO&	GetAddonInfo(int Addon) const	{ return m_Addon[Addon]; }
		const ADDON_INFO&	GetACAddonInfo(int Addon) const;

		//------------------------------------------------------			
		
		//------------------------------------------------------			
		void			SetAddonColorSet1(int Addon, WORD colorSet);
		void			SetAddonColorSet2(int Addon, WORD colorSet);	
		
		//------------------------------------------------------
		
		//------------------------------------------------------
		virtual	bool		SetAddonItem(MItem* pItem);
		virtual bool		RemoveAddonItem(MItem* pItem);

		//------------------------------------------------------
		// SetSameWear
		//------------------------------------------------------
		void				SetSameWear(const MCreatureWear* pCreature);

		//------------------------------------------------------
		// Update AttachEffect
		//------------------------------------------------------
		virtual bool		RemoveEffectStatus(EFFECTSTATUS status);
		virtual void		UpdateAttachEffect();

	protected :
		void			ClearAddonInfo(int Addon);		
		MItem*			NewItemFromAddonInfo(int Addon);

	protected :
		
		ADDON_INFO			m_Addon[ ADDON_MAX ];
//		ADDON_INFO			m_ACAddon[ AC_ADDON_MAX ];
		ADDON_INFO			m_ShoulderAddon;

		
		
		TYPE_FRAMEID		m_HairFrameID;
		WORD				m_HairColorSet;

	public :
		
		static int			s_AddonOrder[DIRECTION_MAX][ADDON_MAX];
		static int			s_ACAddonOrder[ DIRECTION_MAX ][ AC_ADDON_MAX ];
};

#endif


