//----------------------------------------------------------------------
// MGridItemManager.h
//----------------------------------------------------------------------

//

//


//


//----------------------------------------------------------------------
 
//----------------------------------------------------------------------

#ifndef	__MGridItemManager_H__
#define	__MGridItemManager_H__

#include "MItemManager.h"


class MGridItemManager : public MItemManager {
	public :
		MGridItemManager();
		~MGridItemManager();

		//------------------------------------------------------
		// Init / Release
		//------------------------------------------------------
		void		Init(BYTE width, BYTE height);
		void		Release();

		//------------------------------------------------------
		// get
		//------------------------------------------------------
		BYTE		GetWidth() const	{ return m_Width; }
		BYTE		GetHeight() const	{ return m_Height; }
		
		//------------------------------------------------------
		// Add / Get / Remove
		//------------------------------------------------------		
		bool			AddItem(MItem* pItem);					
		bool			AddItem(MItem* pItem, BYTE x, BYTE y);	

		MItem*			GetItem(BYTE x, BYTE y) const;			
		MItem*			GetItem(TYPE_OBJECTID id) const;
		
		MItem*			RemoveItem(BYTE x, BYTE y);				
		MItem*			RemoveItem(TYPE_OBJECTID id);			

		bool			ReplaceItem(MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem);	

		//------------------------------------------------------		
		
		//------------------------------------------------------		
		bool			CanReplaceItem(MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem);

		//------------------------------------------------------		
		// Get FitPosition
		//------------------------------------------------------		
		bool			GetFitPosition(MItem* pItem, POINT& point);	

		//------------------------------------------------------		
		// FindItemGridOrder
		//------------------------------------------------------		
		MItem*			FindItemGridOrder( MItemFinder& itemFinder ) const;

	protected :
		BYTE			m_Width;
		BYTE			m_Height;
		MItem***		m_ItemGrid;
};

#endif

