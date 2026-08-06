//----------------------------------------------------------------------
// MPlayerGear.cpp
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef __MPLAYERGEAR_H__
#define __MPLAYERGEAR_H__

#include "MSlotItemManager.h"

class MPlayerGear : public MSlotItemManager {
	public :
		enum ITEM_STATUS
		{
			ITEM_STATUS_OK,					
			ITEM_STATUS_SOMEWHAT_BROKEN,		
			ITEM_STATUS_ALMOST_BROKEN		
		};

	public :
		MPlayerGear();
		~MPlayerGear();

		//------------------------------------------------------
		// Init / Release
		//------------------------------------------------------
		void			Init(BYTE size);
		void			Release();

		//------------------------------------------------------
		// Add / Get / Remove
		//------------------------------------------------------		
		bool			AddItem(MItem* pItem, BYTE n);		

		MItem*			GetItem(BYTE n) const;				
		
		MItem*			RemoveItem(BYTE n);					
		MItem*			RemoveItem(TYPE_OBJECTID id);		

		bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);	

		//------------------------------------------------------
		
		//------------------------------------------------------
		bool			ModifyDurability(BYTE n, int changeValue);

		//------------------------------------------------------
		
		//------------------------------------------------------
		ITEM_STATUS		GetItemStatus(int slot) const	{ return m_pItemStatus[slot]; }
		BOOL			HasBrokenItem() const			{ return m_nBrokenItem!=0; }
		void			CheckItemStatusAll();

	protected :
		
		void			SetItemStatusOK(int slot);	
		void			CheckItemStatus(const MItem* pItem, int slot);
		
	protected :
		int				m_nBrokenItem;	
		ITEM_STATUS*	m_pItemStatus;
};

#endif

