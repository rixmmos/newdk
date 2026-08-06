//-----------------------------------------------------------------------------
// MTradeManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MTradeManager.h"
#include "MSortedItemManager.h"
#include "MGameDef.h"
#include "DebugInfo.h"

#define SAFE_DELETE(x)		{ if(x!=NULL) delete x; x=NULL; }

#ifdef __GAME_CLIENT__
	#include "PacketDef.h"
	#include "ClientConfig.h"
	extern DWORD	g_CurrentTime;
#endif

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ItemPosition {
	public :
		MItem*			pItem;
		int				gridX;
		int				gridY;

	public :
		ItemPosition(MItem* _pItem, int _gridX, int _gridY)
		{
			pItem	= _pItem;
			gridX	= _gridX;
			gridY	= _gridY;
		}
};

class ItemPositionMap : public std::map<TYPE_OBJECTID, ItemPosition*> {
	public :
		ItemPositionMap() {}
		~ItemPositionMap()
		{
			iterator iItemPosition = begin();

			while (iItemPosition!=end())
			{
				ItemPosition* pItemPosition = iItemPosition->second;

				delete pItemPosition;

				iItemPosition++;
			}
		}
};


//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
MTradeManager*		g_pTradeManager = NULL;

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
MTradeManager::MTradeManager()
{
	m_OtherID	= OBJECTID_NULL;

	m_pMyInventory = NULL;
	m_pOtherInventory = NULL;
	
	m_pMyMoney = NULL;
	m_pOtherMoney = NULL;

	m_bAcceptMyTrade = false;
	m_bAcceptOtherTrade = false;
	
	m_NextAcceptTime = 0;
}

MTradeManager::~MTradeManager()
{
	Release();
}

//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void				
MTradeManager::Init()
{
	Release();

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	m_pMyInventory = g_pInventory;	//new MInventory;
	m_pOtherInventory = new MInventory;
	
	//m_pMyInventory->Init(TRADE_INVENTORY_WIDTH, TRADE_INVENTORY_HEIGHT);
	m_pOtherInventory->Init(TRADE_INVENTORY_WIDTH, TRADE_INVENTORY_HEIGHT);


	//--------------------------------------------------------------
	// Money Manager
	//--------------------------------------------------------------
	m_pMyMoney = new MMoneyManager;
	m_pOtherMoney = new MMoneyManager;

	m_bAcceptMyTrade = false;
	m_bAcceptOtherTrade = false;

	m_OtherID	= OBJECTID_NULL;
}

//-----------------------------------------------------------------------------
// Release
//-----------------------------------------------------------------------------
void				
MTradeManager::Release()
{
	//if (m_pMyInventory != NULL)
	//{
	//	delete m_pMyInventory;
	//	m_pMyInventory = NULL;
	//}

	if (m_pOtherInventory != NULL)
	{
		delete m_pOtherInventory;
		m_pOtherInventory = NULL;
	}

	if (m_pMyMoney != NULL)
	{
		delete m_pMyMoney;
		m_pMyMoney = NULL;
	}

	if (m_pOtherMoney != NULL)
	{
		delete m_pOtherMoney;
		m_pOtherMoney = NULL;
	}

	m_bAcceptMyTrade = false;
	m_bAcceptOtherTrade = false;

	m_NextAcceptTime = 0;
}

//-----------------------------------------------------------------------------
// Is AcceptTime
//-----------------------------------------------------------------------------
bool				
MTradeManager::IsAcceptTime() const
{
	#ifdef __GAME_CLIENT__
		return g_CurrentTime >= m_NextAcceptTime;
	#else
		return true;
	#endif
}

//-----------------------------------------------------------------------------
// Set NextAcceptTime
//-----------------------------------------------------------------------------
void
MTradeManager::SetNextAcceptTime()
{
	#ifdef __GAME_CLIENT__
		m_NextAcceptTime = g_CurrentTime + g_pClientConfig->TRADE_ACCEPT_DELAY_TIME;
	#endif
}

//-----------------------------------------------------------------------------
// Refuse MyTrade
//-----------------------------------------------------------------------------
void				
MTradeManager::RefuseMyTrade()						
{ 
	if (m_bAcceptMyTrade)
	{
		#ifdef __GAME_CLIENT__
			SetNextAcceptTime();
		#endif

		m_bAcceptMyTrade = false; 
	}
}

//-----------------------------------------------------------------------------
// Refuse OtherTrade
//-----------------------------------------------------------------------------
void				
MTradeManager::RefuseOtherTrade()					
{ 
	if (m_bAcceptOtherTrade)
	{
		#ifdef __GAME_CLIENT__
			SetNextAcceptTime();
		#endif

		m_bAcceptOtherTrade = false; 
	}
}

//-----------------------------------------------------------------------------
// Can Trade
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool				
MTradeManager::CanTrade() const
{
	if (m_pOtherInventory==NULL)
	{
		return false;
	}

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	if (!g_pMoneyManager->CanAddMoney( m_pOtherMoney->GetMoney() ))
	{
		
		return false;
	}

	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	
	//if (g_pInventory->GetItemNum() + m_pOtherInventory->GetItemNum()
		//> g_pInventory->GetWidth()*g_pInventory->GetHeight())
	//{
	//	return false;
	//}

	//-----------------------------------------------------------
	//
	
	//
	//-----------------------------------------------------------
	

	MInventory	tempInventory;
	
	tempInventory.Init( g_pInventory->GetWidth(), g_pInventory->GetHeight() );

	//-----------------------------------------------------------
	// g_pInventory ---> tempInventory
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	bool bOK = true;		

	g_pInventory->SetBegin();

	while ( g_pInventory->IsNotEnd() )
	{
		const MItem* pItem = g_pInventory->Get();

		//-----------------------------------------------------------
		
		
		//-----------------------------------------------------------
		if (!pItem->IsTrade())
		{
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			if (!tempInventory.AddItem( (MItem*)pItem, pItem->GetGridX(), pItem->GetGridY() ))
			{
				bOK = false;
				break;
			}
		}

		g_pInventory->Next();
	}

	//-----------------------------------------------------------
	
	//-----------------------------------------------------------	
	ItemPositionMap		mapItemPosition;	

	//-----------------------------------------------------------
	//
	
	
	//
	//-----------------------------------------------------------
	if (bOK)
	{			
		MSortedItemManager	SIM;

		//-----------------------------------------------------------
		
		
		//-----------------------------------------------------------
		m_pOtherInventory->SetBegin();

		while ( m_pOtherInventory->IsNotEnd() )
		{
			MItem* pItem = m_pOtherInventory->Get();

			ItemPosition* pItemPosition = new ItemPosition( (MItem*)pItem, pItem->GetGridX(), pItem->GetGridY() );
			mapItemPosition.insert( ItemPositionMap::value_type(pItem->GetID(), pItemPosition) );

			//---------------------------------------------------
			
			//---------------------------------------------------
			SIM.AddItem( pItem );

			m_pOtherInventory->Next();
		}	

		//---------------------------------------------------------------
		//
		//			SIM --> tempInventory
		//
		//---------------------------------------------------------------
		MSortedItemManager::iterator iItem = SIM.begin();

		while (iItem != SIM.end())
		{
			MItem* pItem = iItem->second;

			//-----------------------------------------------------
			
			//-----------------------------------------------------
			if (!tempInventory.AddItem( pItem ))
			{				
				bOK = false;
				
				break;
			}

			iItem ++;
		}

		SIM.Clear();
	}

	//-----------------------------------------------------------
	//
	
	//
	//-----------------------------------------------------------
	tempInventory.SetBegin();

	while ( tempInventory.IsNotEnd() )
	{
		const MItem* pItem = tempInventory.Get();

		tempInventory.Next();

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		if (!tempInventory.RemoveItem( pItem->GetGridX(), pItem->GetGridY() ))
		{			
			

			
			
		}
	}

	//-----------------------------------------------------------
	
	
	//-----------------------------------------------------------
	m_pOtherInventory->SetBegin();

	while ( m_pOtherInventory->IsNotEnd() )
	{
		const MItem* pItem = m_pOtherInventory->Get();

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		ItemPositionMap::iterator iItemPosition = mapItemPosition.find( pItem->GetID() );

		if (iItemPosition == mapItemPosition.end())
		{
			
			break;
		}

		ItemPosition* pItemPosition = iItemPosition->second;

		MItem* pOriginalItem = pItemPosition->pItem;
		pOriginalItem->SetGridXY( pItemPosition->gridX, pItemPosition->gridY );

		m_pOtherInventory->Next();
	}

	

	return bOK;
}

//-----------------------------------------------------------------------------
// Trade
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
bool				
MTradeManager::Trade()
{
	if (
		!m_bAcceptMyTrade || !m_bAcceptOtherTrade
		
		|| m_pMyInventory==NULL
		|| m_pOtherInventory==NULL
		|| m_pMyMoney==NULL
		|| m_pOtherMoney==NULL)
	{
		#ifdef OUTPUT_DEBUG
			DEBUG_ADD_FORMAT("[Error] Trade Failed! %d %d %d %d %d %d"
								,!m_bAcceptMyTrade
								,!m_bAcceptOtherTrade
								,m_pMyInventory==NULL
								,m_pOtherInventory==NULL
								,m_pMyMoney==NULL
								,m_pOtherMoney==NULL);			
		#endif

		
		return false;
	}

	//---------------------------------------------------------------
	//
	
	//
	//---------------------------------------------------------------
	if (!CanTrade())
	{
		DEBUG_ADD("[Error] Can't Trade");			
		
		return false;
	}

	MSortedItemManager	SIM;

	//---------------------------------------------------------------
	//
	
	//
	//---------------------------------------------------------------
	DEBUG_ADD("[Trade] Remove Trading Items");
	
	m_pMyInventory->SetBegin();

	while ( m_pMyInventory->IsNotEnd() )
	{
		const MItem* pItem = m_pMyInventory->Get();

		m_pMyInventory->Next();

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		if (pItem->IsTrade())
		{
			DEBUG_ADD_FORMAT("[Trade] Remove: id=%d. xy=(%d, %d)", pItem->GetID(), pItem->GetX(), pItem->GetY() );
			
			MItem* pRemovedItem = m_pMyInventory->RemoveItem( pItem->GetGridX(), pItem->GetGridY() );

			if (pRemovedItem!=NULL)
			{
				delete pRemovedItem;
			}
			else
			{
				
				DEBUG_ADD("[Error] Remove Error");				
			}

		}				
	}

	//---------------------------------------------------------------
	//
	//		m_pOtherInventory --> SortedItemManager
	//
	//---------------------------------------------------------------
	DEBUG_ADD("[Trade] Move OtherInventory --> SortedItemManager");
	
	m_pOtherInventory->SetBegin();

	while ( m_pOtherInventory->IsNotEnd() )
	{
		const MItem* pItem = m_pOtherInventory->Get();
		
		m_pOtherInventory->Next();

		//-----------------------------------------------------------
		
		//-----------------------------------------------------------
		MItem* pRemovedItem = m_pOtherInventory->RemoveItem( pItem->GetGridX(), pItem->GetGridY() );

		if (pRemovedItem!=NULL)
		{
			//-----------------------------------------------------------
			
			//-----------------------------------------------------------
			if (SIM.AddItem( pRemovedItem ))
			{
				//-------------------------------------------------------
				
				//-------------------------------------------------------				
			}
			else
			{
				
				delete pRemovedItem;	
			}
		}
		else	
		{
			
			
		}

	}

	//---------------------------------------------------------------
	//
	//			SIM --> g_pInventory
	//
	//---------------------------------------------------------------
	DEBUG_ADD("[Trade] SIM --> Inventory");
		
	MSortedItemManager::iterator iItem = SIM.begin();

	while (iItem != SIM.end())
	{
		MItem* pItem = iItem->second;

		if (g_pInventory->AddItem( pItem ))
		{
			pItem->UnSetTrade();	// -_-;

			
			if (pItem->GetItemClass()==ITEM_CLASS_EVENT_GIFT_BOX)
			{
				if(pItem->GetItemType()==0)
				{
					pItem->SetItemType( 1 );
				}
				else if(pItem->GetItemType() >= 2 && pItem->GetItemType() <= 5)
				{
					pItem->SetItemType(6);
				}
				
				else if(pItem->GetItemType() >= 16 && pItem->GetItemType() <= 18)
				{  
					int TempItemType = pItem->GetItemType() + 3; 
					pItem->SetItemType(TempItemType);
				}
				
			}
		}
		else
		{
			// - -;
//			delete pItem;
			SAFE_DELETE( pItem );
		}

		iItem ++;
	}

	SIM.Clear();


	//-----------------------------------------------------------
	//
	
	//
	//-----------------------------------------------------------
	DEBUG_ADD("[Trade] Money");
		

	if (g_pMoneyManager->AddMoney( m_pOtherMoney->GetMoney() ))
	{
		m_pOtherMoney->SetMoney( 0 );
		m_pMyMoney->SetMoney( 0 );
	}
	else
	{
		
	}

	m_bAcceptMyTrade = false;
	m_bAcceptOtherTrade = false;
	
	return true;
}

//-----------------------------------------------------------------------------
// Cancel Trade 
//-----------------------------------------------------------------------------

//
// 
//-----------------------------------------------------------------------------
bool
MTradeManager::CancelTrade()
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (g_pMoneyManager->AddMoney( m_pMyMoney->GetMoney() ))
	{
		m_pOtherMoney->SetMoney( 0 );
		m_pMyMoney->SetMoney( 0 );
	}
	 
	return true;
}