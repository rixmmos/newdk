//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTradeErrorHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTradeError.h"
#include "ClientDef.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "TempInformation.h"
#include "MTradeManager.h"
#include "UIFunction.h"

void GCTradeErrorHandler::execute ( GCTradeError * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	
	if (g_pPlayer==NULL
		|| g_pTempInformation==NULL
		|| g_pUIDialog==NULL
		|| g_pGameStringTable==NULL)
	{
		return;
	}


	int code = pPacket->getCode();

	//----------------------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------------------
	if (code==GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST
		|| code==GC_TRADE_ERROR_CODE_RACE_DIFFER
		|| code==GC_TRADE_ERROR_CODE_NOT_SAFE)
	{	
		if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
		{
			
			UI_CloseExchangeCancel();

			
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE)
			{
				g_pPlayer->SetWaitVerifyNULL();
			}

			g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		}
	}

	switch (code)
	{
		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST :						
			
			UI_CloseExchangeCancel();

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_NOBODY].GetString());
		break;
		
		
		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_RACE_DIFFER :
			
			UI_CloseExchangeCancel();

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE].GetString());
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_NOT_SAFE :
			
			UI_CloseExchangeCancel();

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_SAFETY_ZONE_ONLY].GetString());			
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_MOTORCYCLE :
			
			UI_CloseExchangeCancel();

			if( g_pPlayer->IsSlayer() )
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_CANNOT_ON_MOTORCYCLE].GetString());
			else if ( g_pPlayer->IsOusters() )
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE_SUMMON_SYLPH].GetString() );
		break;
		

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_ALREADY_TRADING :
			
			UI_CloseExchangeCancel();

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE_ALREADY_TRADING].GetString());
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_NOT_TRADING :
			// - -;
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] Server is Not Trade Mode");
				}
			#endif
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_ADD_ITEM :
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] There is no such item to Add");
				}
			#endif
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_REMOVE_ITEM :
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] There is no such item to Remove");
				}
			#endif
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_INCREASE_MONEY :
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] GC_TRADE_ERROR_CODE_INCREASE_MONEY");
				}
			#endif
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_DECREASE_MONEY :
			#ifdef OUTPUT_DEBUG
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] GC_TRADE_ERROR_CODE_DECREASE_MONEY");
				}
			#endif
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE :
			
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE_NO_SPACE].GetString());

			#ifdef OUTPUT_DEBUG			
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE");
				}
			#endif
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_ERROR_CODE_UNKNOWN :
			
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TRADE].GetString());

			#ifdef OUTPUT_DEBUG			
				if (g_pDebugMessage)
				{
					DEBUG_ADD("[Error] GC_TRADE_ERROR_CODE_UNKNOWN");
				}
			#endif
		break;
		
		case GC_TRADE_ERROR_CODE_EVENT_GIFT_BOX:
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_EVENT_GIFT_BOX].GetString());
			break;
	}

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	if (g_pTradeManager!=NULL)
	{
		g_pTradeManager->CancelTrade();
	}

	UI_CloseExchange();

#endif

	__END_CATCH
}
