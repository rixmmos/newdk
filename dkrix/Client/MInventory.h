//----------------------------------------------------------------------
// MInventory.h
//----------------------------------------------------------------------
 
//----------------------------------------------------------------------

#ifndef	__MINVENTORY_H__
#define	__MINVENTORY_H__

#include "MGridItemManager.h"
#include "MItem.h"
#include <list>

/*
#ifdef __GAME_CLIENT__
	class MEffect;
#endif
	*/

class MInventory : public MGridItemManager {
	public :
		MInventory();
		~MInventory();

		//------------------------------------------------------
		// Add
		//------------------------------------------------------		
		bool			AddItem(MItem* pItem);					
		bool			AddItem(MItem* pItem, BYTE x, BYTE y);	
	
		bool			ReplaceItem(MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem);	
		
		//------------------------------------------------------		
		// Get FitPosition
		//------------------------------------------------------		
		bool			GetFitPosition(MItem* pItem, POINT& point);	

		//------------------------------------------------------
		// Find Item
		//------------------------------------------------------		
		MItem*			FindItem( ITEM_CLASS itemClass, TYPE_ITEMTYPE itemType=ITEMTYPE_NULL);

		//------------------------------------------------------
		
		//------------------------------------------------------
		void			CheckAffectStatus();				
		void			CheckAffectStatus(MItem* pItem);	

		/*
	#ifdef __GAME_CLIENT__
		public :
			typedef std::list<MEffect*>						EFFECT_LIST;

		public :
			//------------------------------------------------------		
			// Effect
			//------------------------------------------------------
			bool			AddEffect(MEffect* pEffect);
			void			UpdateEffects();
			int				GetEffectCount()				{ return m_listEffect.size(); }
			EFFECT_LIST::const_iterator GetEffects()		{ return m_listEffect.begin(); }

		protected :
			EFFECT_LIST		m_listEffect;
	#endif
	*/

};

extern MInventory*		g_pInventory;



#endif