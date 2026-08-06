//----------------------------------------------------------------------
// MSortedItemManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MSortedItemManager.h"
#include "MItem.h"

#define	TWO_BY_TWO_PACKING_SIZE		12

//----------------------------------------------------------------------
//
// constructor / destructor
// 
//----------------------------------------------------------------------
MSortedItemManager::MSortedItemManager()
{
	m_NumTwoByTwo = 0;
	m_NumTwoByTwoTemp = 0;
}

MSortedItemManager::~MSortedItemManager()
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

//----------------------------------------------------------------------
void		
MSortedItemManager::Release()
{
	ITEM_MAP::iterator iItem = begin();

	while (iItem != end())
	{
		MItem* pItem = iItem->second;

		if (pItem!=NULL)
		{
			delete pItem;
		}

		iItem++;
	}

	clear();
}

//----------------------------------------------------------------------
// Set Two By Two Number
//----------------------------------------------------------------------
void		
MSortedItemManager::SetTwoByTwoNumber(int n)
{ 
	m_NumTwoByTwo = n; 
	m_NumTwoByTwoTemp = (n/3) * 3;
}

//----------------------------------------------------------------------
// Add Item
//----------------------------------------------------------------------
bool		
MSortedItemManager::AddItem(MItem* pItem)
{
	ULONGLONG key = GetKey( pItem );

	ITEM_MAP::const_iterator iItem = find( key );	

	
	if (iItem == end())
	{
		insert( ITEM_MAP::value_type( key, pItem ) );

		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Get Key
//----------------------------------------------------------------------
//

//
// 4 Byte : gridWidth*gridHeight
// 4 Byte : ItemObjectID
//

//----------------------------------------------------------------------


//----------------------------------------------------------------------
ULONGLONG		
MSortedItemManager::GetKey(MItem* pItem)
{
	TYPE_OBJECTID itemObjectID= pItem->GetID();

	
	int gridWidth		= pItem->GetGridWidth();
	int gridHeight		= pItem->GetGridHeight();
	int gridSize		= gridWidth*gridHeight;
	
	if (gridSize==4)	//gridWidth==2 && gridHeight==2)
	{
		if (m_NumTwoByTwoTemp > 0)
		{
			m_NumTwoByTwoTemp --;

			gridSize = TWO_BY_TWO_PACKING_SIZE;	
		}
	}

	
	gridSize = 0xFF - gridSize;

	
	//int gridX			= pItem->GetGridX();
	//int gridY			= pItem->GetGridY();

	ULONGLONG key = gridSize;	//(gridSize << 16) | (gridX << 8) | gridY;

	key = (key << 32) | itemObjectID;

	return key;
}

//----------------------------------------------------------------------
// Merge
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
 