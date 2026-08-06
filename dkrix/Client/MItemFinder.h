//----------------------------------------------------------------------
// MItemFinder.h
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------

#ifndef __MITEMFINDER_H__
#define __MITEMFINDER_H__

#include "MItem.h"

//----------------------------------------------------------------------
// MItemFinder
//----------------------------------------------------------------------
class MItemFinder {
	public :
		virtual bool operator () ( const MItem* pItem ) = 0;
};

//----------------------------------------------------------------------
// MItemClassFinder
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MItemClassFinder : public MItemFinder {
	public :
		MItemClassFinder(ITEM_CLASS itemClass)		{ m_ItemClass = itemClass; }
		
		void	SetItemClass(ITEM_CLASS itemClass)	{ m_ItemClass = itemClass; }

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==m_ItemClass;
		}

	protected :
		ITEM_CLASS	m_ItemClass;	
};

//----------------------------------------------------------------------
// MItemClassTypeFinder
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MItemClassTypeFinder : public MItemClassFinder {
	public :
		MItemClassTypeFinder(ITEM_CLASS itemClass, TYPE_ITEMTYPE itemType) : MItemClassFinder(itemClass)
		{ 
			m_ItemType = itemType;
		}
		
		void	SetItemType(TYPE_ITEMTYPE itemType)	{ m_ItemType = itemType; }

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==m_ItemClass
					&& pItem->GetItemType()==m_ItemType;
		}

	protected :
		TYPE_ITEMTYPE	m_ItemType;	
};

//----------------------------------------------------------------------
// MVampirePortalItemFinder
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MVampirePortalItemFinder : public MItemFinder {
	public :
		MVampirePortalItemFinder(bool bMarked)
		{
			m_bMarked = bMarked;
		}

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==ITEM_CLASS_VAMPIRE_PORTAL_ITEM
					&& ((MVampirePortalItem*)pItem)->IsMarked()==m_bMarked;
		}

	protected :
		bool	m_bMarked;
};

//----------------------------------------------------------------------
// MSlayerPortalItemFinder
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MSlayerPortalItemFinder : public MItemFinder {
	public :
		MSlayerPortalItemFinder() {}

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==ITEM_CLASS_SLAYER_PORTAL_ITEM
					&& pItem->GetNumber() > 0;
		}
};

//----------------------------------------------------------------------
// MEventStartItemFinder
//----------------------------------------------------------------------

class MItemClassTypeNumberFinder : public MItemClassTypeFinder {
	public :
		MItemClassTypeNumberFinder(ITEM_CLASS itemClass, TYPE_ITEMTYPE itemType) 
			: MItemClassTypeFinder(itemClass, itemType)	
		{
			m_TotalNumber = 0;
		}		
	
		bool	operator () ( const MItem* pItem )
		{
			
			if (pItem->GetItemClass()==m_ItemClass
					&& pItem->GetItemType()==m_ItemType)
			{
				m_TotalNumber += pItem->GetNumber();
			}

			
			return false;
		}

		void	Clear()						{ m_TotalNumber = 0; }
		int		GetTotalNumber()	const	{ return m_TotalNumber; }


	protected :
		int				m_TotalNumber;
};

//----------------------------------------------------------------------
// MItemClassRangeTypeFinder
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MItemClassRangeTypeFinder : public MItemClassFinder {
	public :
		MItemClassRangeTypeFinder(ITEM_CLASS itemClass, TYPE_ITEMTYPE minItemType, TYPE_ITEMTYPE maxItemType) : MItemClassFinder(itemClass)
		{ 
			m_MinItemType = minItemType;
			m_MaxItemType = maxItemType;
		}
		
		void	SetItemType(TYPE_ITEMTYPE minItemType, TYPE_ITEMTYPE maxItemType)	{ m_MinItemType = minItemType;	m_MaxItemType = maxItemType; }

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==m_ItemClass
					&& pItem->GetItemType()>=m_MinItemType
					&& pItem->GetItemType()<=m_MaxItemType;
		}

	protected :
		TYPE_ITEMTYPE	m_MinItemType;	
		TYPE_ITEMTYPE	m_MaxItemType;	
};

//----------------------------------------------------------------------
// MOustersSummonGemItemFinder 
//----------------------------------------------------------------------
//----------------------------------------------------------------------
class MOustersSummonGemItemFinder : public MItemFinder {
	public :
		MOustersSummonGemItemFinder() {}

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==ITEM_CLASS_OUSTERS_SUMMON_ITEM
					&& pItem->GetNumber() > 0;
		}
};

//----------------------------------------------------------------------
// MEventGiftBoxItemFinder 
//----------------------------------------------------------------------
//----------------------------------------------------------------------
class MEventGiftBoxItemFinder : public MItemFinder {
	public :
		MEventGiftBoxItemFinder() {}

		bool	operator () ( const MItem* pItem )
		{
			return pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX
					&& pItem->GetItemType() > 1 && pItem->IsTrade();
		}
};


#endif