//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTradeFinishHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTradeFinish.h"
#include "ClientDef.h"
#include "TempInformation.h"
#include "MTradeManager.h"
#include "UIFunction.h"
#include "UIFunction.h"

void GCTradeFinishHandler::execute ( GCTradeFinish * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	//------------------------------------------------------------------------
	
	//------------------------------------------------------------------------
	/*
	if (g_pTradeManager==NULL)
	{
		DEBUG_ADD(DEBUG_ADD( "[Error] TradeManager is NULL");
		
		return;
	}
	*/


	switch (pPacket->getCode())
	{
		//------------------------------------------------------------------------
		
		//------------------------------------------------------------------------
		case GC_TRADE_FINISH_ACCEPT :
			if (g_pTradeManager!=NULL)
			{
				g_pTradeManager->AcceptOtherTrade();
			}
		break;

		//------------------------------------------------------------------------
		
		//------------------------------------------------------------------------
		case GC_TRADE_FINISH_REJECT :
			if (g_pTradeManager!=NULL)
			{
				g_pTradeManager->CancelTrade();
			}

			if (UI_IsRunningExchange())
			{
				UI_CloseExchange();
			}
			//else 
			{
				if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
				{
					UI_CloseExchangeCancel();
					UI_CloseExchangeAsk();					
				}				
			}

		break;

		//------------------------------------------------------------------------
		
		//------------------------------------------------------------------------
		case GC_TRADE_FINISH_RECONSIDER :
			if (g_pTradeManager!=NULL)
			{
				g_pTradeManager->RefuseOtherTrade();
			}
		break;

		//------------------------------------------------------------------------
		
		//------------------------------------------------------------------------
		case GC_TRADE_FINISH_EXECUTE :
			if (g_pTradeManager!=NULL)
			{
				g_pTradeManager->Trade();

				//----------------------------------------------------------------
				
				//----------------------------------------------------------------
				g_pTradeManager->RefuseMyTrade();
				g_pTradeManager->RefuseOtherTrade();
			}			
			//UI_CloseExchange();
		break;
	}


#endif

	__END_CATCH
}
