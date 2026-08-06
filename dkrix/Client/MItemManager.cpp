//----------------------------------------------------------------------
// MItemManager.h
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItemManager.h"
#include "MItem.h"
#include "MItemFinder.h"
#include "MQuickSlot.h"

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MItemManager::MItemManager()
{
}

MItemManager::~MItemManager()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MItemManager::Release()
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	
	
	while (iItem != m_mapItem.end())
	{
		MItem* pItem = (*iItem).second;

		delete pItem;
#ifdef __GAME_CLIENT__
		if(pItem == g_pArmsBand1)
		{
			g_pArmsBand1 = NULL;
		}
		else if(pItem == g_pArmsBand2)
		{
			g_pArmsBand2 = NULL;
		}
		else if(pItem == g_pQuickSlot)
		{
			g_pQuickSlot = NULL;
		}
#endif
		iItem++;
	}

	m_mapItem.clear();
}

//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MItemManager::AddItem(MItem* pItem)
{
	ITEM_MAP::iterator	iItem;

	iItem = m_mapItem.find(pItem->GetID());
	
	//-----------------------------------------------
	
	//-----------------------------------------------
	if (iItem == m_mapItem.end())
	{
		m_mapItem.insert(ITEM_MAP::value_type(pItem->GetID(), pItem));

		
		CheckAffectStatus( pItem );			

		return true;
	}

	//-----------------------------------------------
	
	//-----------------------------------------------
	return false;
}

//----------------------------------------------------------------------
// Get Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*	
MItemManager::GetItemToModify(TYPE_OBJECTID id)
{
	ITEM_MAP::iterator	iItem;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iItem = m_mapItem.find(id);

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iItem == m_mapItem.end()) 
	{
		return NULL;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	return (*iItem).second;
}

//----------------------------------------------------------------------
// Get Item (const)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*	
MItemManager::GetItem(TYPE_OBJECTID id) const
{
	ITEM_MAP::const_iterator	iItem;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iItem = m_mapItem.find(id);

	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iItem == m_mapItem.end()) 
	{
		return NULL;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	return (*iItem).second;
}

	

//----------------------------------------------------------------------
// Remove Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*	
MItemManager::RemoveItem(TYPE_OBJECTID id)
{
	ITEM_MAP::iterator	iItem;

	//--------------------------------------------------
	
	//--------------------------------------------------
	iItem = m_mapItem.find(id);
    
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (iItem == m_mapItem.end())
	{
		return NULL;
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	MItem* pItem = (*iItem).second;
	
	
	m_mapItem.erase( iItem );

	//--------------------------------------------------
	
	//--------------------------------------------------
	CheckAffectStatusAll();

	return pItem;
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MItemManager::CheckAffectStatusAll()				
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	//------------------------------------------------------
	
	//------------------------------------------------------
	while (iItem != m_mapItem.end())
	{
		MItem* pItem = (*iItem).second;

		CheckAffectStatus( pItem );

		iItem++;
	}
}

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------
void			
MItemManager::CheckAffectStatus(MItem* pItem)
{
	
}

//----------------------------------------------------------------------
// Transfer Item To (id, pItemManager)
//----------------------------------------------------------------------


//----------------------------------------------------------------------
 

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*
MItemManager::FindItem( MItemFinder& itemFinder ) const
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = iItem->second;
		
		if (itemFinder( pItem ))
		{
			return pItem;
		}
//		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
//		{
//			MItem* pTempItem = ((MSubInventory*)pItem)->FindItemGridOrder( itemFinder );
//			if(NULL != pTempItem)
//				return pTempItem;
//		}
		

		iItem++;
	}

	return NULL;
}
#ifdef __TEST_SUB_INVENTORY__   
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*			
MItemManager::FindItemAll( MItemFinder& itemFinder, MItem*& pSubInventoryItem) const
{
	ITEM_MAP::const_iterator iItem = m_mapItem.begin();

	while (iItem != m_mapItem.end())
	{
		MItem* pItem = iItem->second;
		
		if (itemFinder( pItem ))
		{
			return pItem;
		}
		else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
		{
			MItem* pTempItem = ((MSubInventory*)pItem)->FindItem( itemFinder );
			if(NULL != pTempItem)
			{
				pSubInventoryItem = pItem;
				return pTempItem;
			}
		}
		iItem++;
	}

	return NULL;

}
#endif

#ifdef __TEST_SUB_INVENTORY__   


//----------------------------------------------------------------------
// GetItemToModifyAll
//----------------------------------------------------------------------

//----------------------------------------------------------------------
	MItem*			
	MItemManager::GetItemToModifyAll( TYPE_OBJECTID id, MItem*& pSubInventoryItem)
	{
		ITEM_MAP::const_iterator iItem = m_mapItem.begin();

		while (iItem != m_mapItem.end())
		{
			MItem* pItem = iItem->second;
			
			if (pItem != NULL && pItem->GetID() == id)
			{
				return pItem;
			}
			else if(pItem->GetItemClass() == ITEM_CLASS_SUB_INVENTORY)
			{
				MItem* pTempItem = ((MSubInventory*)pItem)->GetItemToModify( id );
				if(NULL != pTempItem)
				{
					pSubInventoryItem = pItem;
					return pTempItem;
				}
			}
			iItem++;
		}

		return NULL;

	}

#endif