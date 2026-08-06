//----------------------------------------------------------------------
// MGridItemManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MItem.h"
#include "MItemFinder.h"
#include "MGridItemManager.h"

//#include "DebugInfo.h"
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MGridItemManager::MGridItemManager()
{
	m_Width = 0;
	m_Height = 0;
	m_ItemGrid = NULL;
}

MGridItemManager::~MGridItemManager()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void		
MGridItemManager::Init(BYTE width, BYTE height)
{
	Release();

	if (width!=0 && height!=0)
	{
		m_Width = width;
		m_Height = height;

		
		m_ItemGrid = new MItem** [m_Height];

		for (int i=0; i<m_Height; i++)
		{
			m_ItemGrid[i] = new MItem* [m_Width];

			
			for (int j=0; j<m_Width; j++)
			{
				m_ItemGrid[i][j] = NULL;
			}
		}
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void		
MGridItemManager::Release()
{
	MItemManager::Release();


	if (m_ItemGrid!=NULL)
	{
		for (int i=0; i<m_Height; i++)
		{
			delete [] m_ItemGrid[i];
		}			

		delete [] m_ItemGrid;
		
		m_ItemGrid = NULL;

		m_Width = 0;
		m_Height = 0;
	}
}

//----------------------------------------------------------------------
// Find 
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MGridItemManager::GetFitPosition(MItem* pItem, POINT& point)
{
	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();
	
	int x, y;
	int i, j;
	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	int yLimit = m_Height - height;
	int xLimit = m_Width - width;

	int yPlusHeight, xPlusWidth;

	for (x=0; x<=xLimit; x++)
	{
		xPlusWidth = x+width;
		
		for (y=0; y<=yLimit; y++)
		{			
			yPlusHeight = y+height;

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			bool bPlace = true;

			for (i=y; bPlace && i<yPlusHeight; i++)
			{
				for (j=x; bPlace && j<xPlusWidth; j++)
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					if (m_ItemGrid[i][j]!=NULL)
					{
						bPlace = false;

						
						//x = j + m_ItemGrid[i][j]->GetGridWidth() - 1;
						//y = i + m_ItemGrid[i][j]->GetGridHeight() - 1;

						break;
					}
				}
			}
			
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (bPlace)
			{
				point.x = x;
				point.y = y;
				
				return true;
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Can Replace Item
//----------------------------------------------------------------------




//----------------------------------------------------------------------
bool			
MGridItemManager::CanReplaceItem(MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem)
{
	pOldItem = NULL;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (x>=m_Width || y>=m_Height)
	{
		return false;
	}

	
	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();

	int i,j;
	int yPlusHeight = y+height;
	int xPlusWidth = x+width;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (xPlusWidth > m_Width || yPlusHeight > m_Height)
	{
		return false;
	}

	//---------------------------------------------------------
	
	
	//---------------------------------------------------------
	MItem* pCheckItem = NULL;
	MItem* pCurrentCheckItem;

	for (i=y; i<yPlusHeight; i++)
	{
		for (j=x; j<xPlusWidth; j++)
		{
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			pCurrentCheckItem = m_ItemGrid[i][j];
			if (pCurrentCheckItem!=NULL)
			{
				if (pCheckItem==NULL)
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					pCheckItem = pCurrentCheckItem;				
				}
				else
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					if (pCheckItem->GetID() != pCurrentCheckItem->GetID())
					{					
						return false;
					}
				}

				
			}
		}
	}

	
	pOldItem = pCheckItem;

	return true;
}

//----------------------------------------------------------------------
// Add
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MGridItemManager::AddItem(MItem* pItem)
{
	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();

	
	int x, y;
	int i, j;
	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	int yLimit = m_Height - height;
	int xLimit = m_Width - width;

	int yPlusHeight, xPlusWidth;

	for (x=0; x<=xLimit; x++)
	{
		xPlusWidth = x+width;
		
		for (y=0; y<=yLimit; y++)	
		{			
			yPlusHeight = y+height;

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			bool bPlace = true;

			for (i=y; bPlace && i<yPlusHeight; i++)
			{
				for (j=x; bPlace && j<xPlusWidth; j++)
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					if (m_ItemGrid[i][j]!=NULL)
					{
						bPlace = false;

						
						y = i + m_ItemGrid[i][j]->GetGridHeight() - 1;
						
						break;
					}
				}
			}
			
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (bPlace)
			{
				//---------------------------------------------------------
				
				
				//---------------------------------------------------------
				if (MItemManager::AddItem( pItem ))
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					pItem->SetGridXY( x, y );

					//---------------------------------------------------------
					
					//---------------------------------------------------------
					for (i=y; i<yPlusHeight; i++)
					{
						for (j=x; j<xPlusWidth; j++)
						{
							m_ItemGrid[i][j] = pItem;
						}
					}

					return true;
				}
			}
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Add
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool			
MGridItemManager::AddItem(MItem* pItem, BYTE x, BYTE y)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (x>=m_Width || y>=m_Height)
	{
		return false;
	}

	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();

	
	int i,j;
	int yPlusHeight = y+height;
	int xPlusWidth = x+width;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (xPlusWidth > m_Width || yPlusHeight > m_Height)
	{
		return false;
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	for (i=y; i<yPlusHeight; i++)
	{
		for (j=x; j<xPlusWidth; j++)
		{
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (m_ItemGrid[i][j]!=NULL)
				return false;
		}
	}	

	//---------------------------------------------------------
	
	
	//---------------------------------------------------------
	if (MItemManager::AddItem( pItem ))
	{
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		pItem->SetGridXY( x, y );

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		for (i=y; i<yPlusHeight; i++)
		{
			for (j=x; j<xPlusWidth; j++)
			{
				m_ItemGrid[i][j] = pItem;
			}
		}

		return true;
	}

	return false;	
}

//----------------------------------------------------------------------
// Get
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*	
MGridItemManager::GetItem(BYTE x, BYTE y) const
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (x>=m_Width || y>=m_Height)
	{
		return NULL;
	}

	return m_ItemGrid[y][x];
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*			
MGridItemManager::RemoveItem(BYTE x, BYTE y)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (x>=m_Width || y>=m_Height)
	{
		return NULL;
	}

	MItem* pItem = m_ItemGrid[y][x];

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (pItem == NULL)
	{
		return NULL;
	}

	//---------------------------------------------------------	
	
	//---------------------------------------------------------	
	pItem = MItemManager::RemoveItem( pItem->GetID() );

	
	if (pItem == NULL)
	{
		return NULL;
	}

	
	

	//---------------------------------------------------------	
	
	//---------------------------------------------------------
	
	x = pItem->GetGridX();
	y = pItem->GetGridY();
	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();

	int i,j;
	int yPlusHeight = y + height;
	int xPlusWidth = x + width;

	
	for (i=y; i<yPlusHeight; i++)
	{
		for (j=x; j<xPlusWidth; j++)
		{
			m_ItemGrid[i][j] = NULL;				
		}
	}	

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	//pItem->SetGridXY(0, 0);

	return pItem;
}

MItem *MGridItemManager::GetItem(TYPE_OBJECTID id) const
{
	return MItemManager::GetItem(id);
}

//----------------------------------------------------------------------
// Remove
//----------------------------------------------------------------------

//----------------------------------------------------------------------
MItem*			
MGridItemManager::RemoveItem(TYPE_OBJECTID id)
{
	MItem* pItem = MItemManager::RemoveItem( id );

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (pItem==NULL)
	{
		return NULL;
	}

	//---------------------------------------------------------	
	
	//---------------------------------------------------------
	int x	= pItem->GetGridX();
	int y	= pItem->GetGridY();
	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();

	int i,j;
	int yPlusHeight = y + height;
	int xPlusWidth = x + width;

	
	for (i=y; i<yPlusHeight; i++)
	{
		for (j=x; j<xPlusWidth; j++)
		{
			m_ItemGrid[i][j] = NULL;				
		}
	}	

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	//pItem->SetGridXY(0, 0);

	return pItem;
}

//----------------------------------------------------------------------
// Replace Item
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool			
MGridItemManager::ReplaceItem(MItem* pItem, BYTE x, BYTE y, MItem*& pOldItem)
{
	pOldItem = NULL;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (x>=m_Width || y>=m_Height)
	{
		return false;
	}	

	
	int width = pItem->GetGridWidth();
	int height = pItem->GetGridHeight();

	int i,j;
	int yPlusHeight = y+height;
	int xPlusWidth = x+width;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (xPlusWidth > m_Width || yPlusHeight > m_Height)
	{
		return false;
	}

	//---------------------------------------------------------
	
	
	//---------------------------------------------------------
	MItem* pCheckItem = NULL;
	MItem* pCurrentCheckItem;

	for (i=y; i<yPlusHeight; i++)
	{
		for (j=x; j<xPlusWidth; j++)
		{
			//---------------------------------------------------------
			
			//---------------------------------------------------------
			pCurrentCheckItem = m_ItemGrid[i][j];
			if (pCurrentCheckItem!=NULL)
			{
				if (pCheckItem==NULL)
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					pCheckItem = pCurrentCheckItem;				
				}
				else
				{
					//---------------------------------------------------------
					
					//---------------------------------------------------------
					if (pCheckItem->GetID() != pCurrentCheckItem->GetID())
					{					
						return false;
					}
				}

				
			}
		}
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (pCheckItem!=NULL)
	{
		//---------------------------------------------------------	
		
		//---------------------------------------------------------	
		
		//---------------------------------------------------------	
		if (MItemManager::RemoveItem( pCheckItem->GetID() ) == NULL)
		{
			return false;
		}

		
		

		//---------------------------------------------------------	
		
		//---------------------------------------------------------
		
		int ox = pCheckItem->GetGridX();
		int oy = pCheckItem->GetGridY();
		int owidth = pCheckItem->GetGridWidth();
		int oheight = pCheckItem->GetGridHeight();

		int oyPlusHeight = oy + oheight;
		int oxPlusWidth = ox + owidth;

		
		for (i=oy; i<oyPlusHeight; i++)
		{
			for (j=ox; j<oxPlusWidth; j++)
			{
				m_ItemGrid[i][j] = NULL;				
			}
		}	

		
		//pCheckItem->SetGridXY(0, 0);

		
		pOldItem = pCheckItem;
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	
	
	//---------------------------------------------------------
	if (MItemManager::AddItem( pItem ))
	{
		//---------------------------------------------------------
		
		//---------------------------------------------------------
		pItem->SetGridXY( x, y );

		//---------------------------------------------------------
		
		//---------------------------------------------------------
		for (i=y; i<yPlusHeight; i++)
		{
			for (j=x; j<xPlusWidth; j++)
			{
				m_ItemGrid[i][j] = pItem;
			}
		}
	}

	return true;
}

//----------------------------------------------------------------------
// Find Item Grid Order
//----------------------------------------------------------------------


//----------------------------------------------------------------------
MItem*			
MGridItemManager::FindItemGridOrder( MItemFinder& itemFinder ) const
{
	int x, y;
	int i, j;	

	//---------------------------------------------------------
	
	//---------------------------------------------------------	
	bool** bCheck = new bool* [m_Height];
	for (i=0; i<m_Height; i++)
	{
		bCheck[i] = new bool [m_Width];

		memset( bCheck[i], 0, m_Width );

		//for (int j=0; j<m_Width; j++)
		//{
		//	bCheck[i][j] = false;
		//}
	}
	
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	for (y=0; y<m_Height; y++)
	{
		for (x=0; x<m_Width; x++)	
		{		
			
			if (bCheck[y][x])
			{
				continue;
			}

			MItem* pItem = m_ItemGrid[y][x];
			
			if (pItem!=NULL)
			{
				if (itemFinder( pItem ))
				{
					return pItem;
				}

				
				int maxY = y + pItem->GetGridHeight();
				int maxX = x + pItem->GetGridWidth();

				
				
				for (i=y; i<maxY; i++)
				{
					for (j=x; j<maxX; j++)
					{
						bCheck[i][j] = true;
					}
				}
			}
		}
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (bCheck!=NULL)
	{
		for (int i=0; i<m_Height; i++)
		{
			delete [] bCheck[i];
		}			

		delete [] bCheck;
	}

	return NULL;
}

