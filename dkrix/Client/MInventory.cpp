//----------------------------------------------------------------------
// MInventory.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MInventory.h"

#include "MHelpManager.h"
#ifdef __GAME_CLIENT__
	#include "ClientFunction.h"
	#include "MPlayer.h"
#endif

/*
#ifdef __GAME_CLIENT__
	#include "MEffect.h"
	#include "MEffectGeneratorTable.h"
#endif
	*/

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MInventory*		g_pInventory = NULL;

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MInventory::MInventory()
{
}

MInventory::~MInventory()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Check AffectStatus
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
MInventory::CheckAffectStatus(MItem* pItem)	
{
#ifdef __GAME_CLIENT__

	g_pPlayer->CheckAffectStatus( pItem );
		
#endif
}

//----------------------------------------------------------------------
// Add item ( pItem )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MInventory::AddItem(MItem* pItem)
{
	if (pItem->IsInventoryItem())
	{
		if (MGridItemManager::AddItem( pItem ))
		{
			
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetInventorySoundID() );
			#endif
		
						
			return true;
		}

		return false;
	}

	return false;
}

//----------------------------------------------------------------------
// Add item ( pItem )
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MInventory::AddItem(MItem* pItem, BYTE x, BYTE y)
{
	if (pItem->IsInventoryItem())
	{
		if (MGridItemManager::AddItem( pItem, x, y ))
		{
			
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetInventorySoundID() );
			#endif

			return true;
		}

		return false;
	}

	return false;
}

//----------------------------------------------------------------------
// Replace item ( pItem,  x,y,  pOldItem )
//----------------------------------------------------------------------



//----------------------------------------------------------------------
bool			
MInventory::ReplaceItem(MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem)
{
	if (pItem->IsInventoryItem())
	{
		if (MGridItemManager::ReplaceItem(pItem, x,y, pOldItem))
		{
			
			#ifdef __GAME_CLIENT__
				PlaySound( pItem->GetInventorySoundID() );
			#endif

			return true;
		}
		
		return false;
	}

	return false;
}

//----------------------------------------------------------------------
// Get Fit Position ( pItem, point )
//----------------------------------------------------------------------

//



//----------------------------------------------------------------------
bool			
MInventory::GetFitPosition(MItem* pItem, POINT& point)
{
	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (pItem->IsPileItem())
	{
		
		
		
		ITEM_MAP::iterator iItem = m_mapItem.begin();

		while (iItem != m_mapItem.end())
		{
			MItem* pInventoryItem = (*iItem).second;

			//--------------------------------------------
			
			
			//--------------------------------------------
			if (pInventoryItem->GetItemClass()==pItem->GetItemClass()
				&& pInventoryItem->GetItemType()==pItem->GetItemType()
				&& pInventoryItem->GetNumber() + pItem->GetNumber() <= pItem->GetMaxNumber()
				&& !pInventoryItem->IsQuestItem())
			{
				point.x = pInventoryItem->GetGridX();
				point.y = pInventoryItem->GetGridY();

				return true;
			}

			iItem++;
		}
	}

	return MGridItemManager::GetFitPosition( pItem, point );
}

//----------------------------------------------------------------------
// Find Item  (class , type)
//----------------------------------------------------------------------



//----------------------------------------------------------------------
MItem*	
MInventory::FindItem( ITEM_CLASS itemClass, TYPE_ITEMTYPE itemType )
{
	ITEM_MAP::iterator iItem = m_mapItem.begin();

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (itemType==ITEMTYPE_NULL)
	{
		while (iItem != m_mapItem.end())
		{
			MItem* pItem = (*iItem).second;

			
			if (pItem->GetItemClass()==itemClass)
			{
				return pItem;
			}

			iItem++;
		}
	}
	//------------------------------------------------------
	
	//------------------------------------------------------
	else
	{
		while (iItem != m_mapItem.end())
		{
			MItem* pItem = (*iItem).second;

			
			if (pItem->GetItemClass()==itemClass
				&& pItem->GetItemType()==itemType)
			{
				return pItem;
			}

			iItem++;
		}
	}

	return NULL;
}

 

