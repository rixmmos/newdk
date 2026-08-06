//----------------------------------------------------------------------
// MSlotItemManager.h
//----------------------------------------------------------------------

//

//----------------------------------------------------------------------

#ifndef	__MSLOTITEMMANAGER_H__
#define	__MSLOTITEMMANAGER_H__


#include "MItemManager.h"


class MSlotItemManager : public MItemManager {
	public :
		MSlotItemManager();
		~MSlotItemManager();

		//------------------------------------------------------
		// Init / Release
		//------------------------------------------------------
		void		Init(BYTE size);
		void		Release();

		//------------------------------------------------------
		// get
		//------------------------------------------------------
		BYTE		GetSize() const	{ return m_Size; }
	
		//------------------------------------------------------
		// Add / Get / Remove
		//------------------------------------------------------		
		bool			AddItem(MItem* pItem, BYTE n);		

		MItem*			GetItem(BYTE n) const;				
		
		MItem*			RemoveItem(BYTE n);					
		MItem*			RemoveItem(TYPE_OBJECTID id);		

		bool			ReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);	

		bool			CanReplaceItem(MItem* pItem, BYTE n, MItem*& pOldItem);


	protected :
		BYTE			m_Size;
		MItem**			m_ItemSlot;
};

#endif


