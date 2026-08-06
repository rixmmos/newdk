//----------------------------------------------------------------------
// MShop.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MShop.h"
#include "MShopShelf.h"

#ifdef __GAME_CLIENT__
	#include "MPlayer.h"
#endif

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MShop::MShop()
{
	m_Size			= 0;
	m_CurrentShelf	= 0;
	m_pShelf		= NULL;

	m_ShopType		= SHOP_NORMAL;
}

MShop::~MShop()
{
	Release();
}

//----------------------------------------------------------------------
// Init ( n )
//----------------------------------------------------------------------
void				
MShop::Init(unsigned int n)
{
	Release();
	
	m_Size		= n;
	m_pShelf	= new MShopShelf* [m_Size];

	
	for (int i=0; i<m_Size; i++)
	{
		m_pShelf[i] = NULL;
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void				
MShop::Release()
{
	//---------------------------------------------------
	
	//---------------------------------------------------
	if (m_pShelf!=NULL)
	{
		for (int i=0; i<m_Size; i++)
		{
			if (m_pShelf[i]!=NULL)
			{
				delete m_pShelf[i];
			}
		}

		delete [] m_pShelf;
	}

	m_pShelf		= NULL;
	m_Size			= 0;
	m_CurrentShelf	= 0;
}

//----------------------------------------------------------------------
// SetShelf( n,  pShelf )
//----------------------------------------------------------------------
bool				
MShop::SetShelf(unsigned int n, MShopShelf* pShelf)
{
	if (pShelf==NULL)
	{
		return false;
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (m_pShelf[n]!=NULL)
	{
		delete m_pShelf[n];
	}

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	m_pShelf[n] = pShelf;

	return true;
}

//----------------------------------------------------------------------
// Get Shelf ( n )
//----------------------------------------------------------------------
MShopShelf*			
MShop::GetShelf(unsigned int n) const
{
	if (n>=m_Size)
	{
		return NULL;
	}

	return m_pShelf[n];
}

//----------------------------------------------------------------------
// Set Current( n )
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
MShop::SetCurrent(unsigned int n)
{
	if (n>=m_Size)
	{
		return;
	}

	//------------------------------------------------------
	
	//------------------------------------------------------
	#ifdef __GAME_CLIENT__
		MShopShelf* pShopShelf = m_pShelf[n];

		if (pShopShelf!=NULL)
		{		
			for (int i=0; i<SHOP_SHELF_SLOT; i++)
			{
				MItem* pItem = pShopShelf->GetItem( i );

				if (pItem!=NULL)
				{	
					g_pPlayer->CheckAffectStatus( pItem );
				}
			}
		}
	#endif

	m_CurrentShelf = n;
}

