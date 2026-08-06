//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTradeRemoveItemHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTradeRemoveItem.h"
#include "MTradeManager.h"
#include "ClientDef.h"

void GCTradeRemoveItemHandler::execute ( GCTradeRemoveItem * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	//------------------------------------------------------------------------
	
	//------------------------------------------------------------------------
	if (g_pTradeManager==NULL)
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
		
		return;
	}

	//ObjectID_t getTargetObjectID() const throw() { return m_TargetObjectID; }
	
	TYPE_OBJECTID itemID = pPacket->getItemObjectID();

	MItem* pItem = g_pTradeManager->GetOtherInventory()->RemoveItem( itemID );

	//------------------------------------------------------------------------
	
	//------------------------------------------------------------------------
	if (pItem==NULL)
	{
		DEBUG_ADD_FORMAT( "[Error] There is no such item. id=%d", itemID );
	}
	//------------------------------------------------------------------------
	
	//------------------------------------------------------------------------
	else
	{
		delete pItem;
	}
	
	//-----------------------------------------------------------
	
	//-----------------------------------------------------------
	g_pTradeManager->RefuseOtherTrade();
	g_pTradeManager->RefuseMyTrade();

	g_pTradeManager->SetNextAcceptTime();

#endif

	__END_CATCH
}
