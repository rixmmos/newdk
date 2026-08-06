//----------------------------------------------------------------------
// MShopShelf.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MShopShelf.h"
//#include "MShopFixedItemTable.h"

#ifdef __GAME_CLIENT__
	#include "MPlayer.h"
#endif
#include "DebugInfo.h"

//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MShopShelf::FUNCTION_NEWSHELFCLASS
MShopShelf::s_NewShelfClassTable[MAX_SHELF] =
{
	MShopFixedShelf::NewShelf,
	MShopSpecialShelf::NewShelf,
	MShopUnknownShelf::NewShelf
};

//----------------------------------------------------------------------
//
//							 MShop Shelf
//
//----------------------------------------------------------------------
MShopShelf::MShopShelf()
{
	m_bShelfEnable = false;

	m_Version = 0;

	
	for (int i=0; i<SHOP_SHELF_SLOT; i++)
	{
		m_pItems[i] = NULL;
	}
}

MShopShelf::~MShopShelf()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// New Shelf
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MShopShelf*		
MShopShelf::NewShelf(MShopShelf::SHELF_TYPE ShelfClass)
{
	DEBUG_ADD_FORMAT("MShopShelf::NewShelf %d", ShelfClass);

	return (MShopShelf*)(*s_NewShelfClassTable[ShelfClass])();
};

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MShopShelf::Release()
{
	m_bShelfEnable = false;

	m_Version = 0;

	for (int i=0; i<SHOP_SHELF_SLOT; i++)
	{
		
		if (m_pItems[i]!=NULL)
		{
			delete m_pItems[i];
		}

		m_pItems[i] = NULL;
	}
}

//----------------------------------------------------------------------
// Is Empty
//----------------------------------------------------------------------
bool		
MShopShelf::IsEmptySlot(unsigned int slot)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return true;
	}
	
	return m_pItems[slot]==NULL;
}

//----------------------------------------------------------------------
// Get item
//----------------------------------------------------------------------
MItem*		
MShopShelf::GetItem(unsigned int slot) const
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return NULL;
	}

	return m_pItems[slot];
}
//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
MShopShelf::AddItem(MItem* pItem)
{
	for (int i=0; i<SHOP_SHELF_SLOT; i++)
	{
		
		if (m_pItems[i]==NULL)
		{
			if (SetItem( i, pItem ))
			{
				return true;
			}

			return false;
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Set Item
//----------------------------------------------------------------------
bool		
MShopShelf::SetItem(unsigned int  slot, MItem* pItem)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return false;
	}

	
	if (m_pItems[slot]!=NULL)
	{
		delete m_pItems[slot];
	}

	m_pItems[slot] = pItem;

	//------------------------------------------------------
	
	//------------------------------------------------------
	#ifdef __GAME_CLIENT__
		g_pPlayer->CheckAffectStatus( pItem );
	#endif

	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*		
MShopShelf::RemoveItem(unsigned int slot)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return NULL;
	}

	
	MItem* pRemoveItem = m_pItems[slot];

	
	m_pItems[slot] = NULL;

	return pRemoveItem;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MShopShelf::DeleteItem(unsigned int slot)
{
	if (slot >= SHOP_SHELF_SLOT)
	{
		return;
	}

	
	if (m_pItems[slot]!=NULL)
	{
		delete m_pItems[slot];
	}

	
	m_pItems[slot] = NULL;
}


//----------------------------------------------------------------------
//
//							MShop FixedShelf 
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init From FixedItemTable
//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
//
// MShop FixedShelf
//
//----------------------------------------------------------------------
MShopFixedShelf::MShopFixedShelf()
{
	m_Name = "Normal";
}

MShopFixedShelf::~MShopFixedShelf()
{
}

//----------------------------------------------------------------------
//
// MShop SpecialShelf
//
//----------------------------------------------------------------------
MShopSpecialShelf::MShopSpecialShelf()
{
	m_Name = "Special";
}

MShopSpecialShelf::~MShopSpecialShelf()
{
}

//----------------------------------------------------------------------
//
// MShop UnknownShelf
//
//----------------------------------------------------------------------
MShopUnknownShelf::MShopUnknownShelf()
{
	m_Name = "Mysterious";
}

MShopUnknownShelf::~MShopUnknownShelf()
{
}

