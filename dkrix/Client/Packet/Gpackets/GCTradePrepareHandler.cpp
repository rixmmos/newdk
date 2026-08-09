//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTradePrepareHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTradePrepare.h"
#include "ClientDef.h"
#include "MGameStringTable.h"
#include "UIDialog.h"
#include "TempInformation.h"
#include "MTradeManager.h"

#ifdef __GAME_CLIENT__

	#include "../ClientPlayer.h"
	#include "CGTradePrepare.h"
	#include "UIFunction.h"

#endif


void GCTradePrepareHandler::execute ( GCTradePrepare * pPacket , Player * pPlayer )
	 


throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	ObjectID_t targetID = pPacket->getTargetObjectID();

	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);
		
	//----------------------------------------------------------------------
	//
	
	//
	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	if (g_pPlayer->IsWaitVerify() && g_pPlayer->GetWaitVerify()!=MPlayer::WAIT_VERIFY_TRADE			
		|| g_pPlayer->IsRepeatAction()		
		|| g_pUIDialog->IsLockInput()		
		|| g_pPlayer->GetCreatureType()==CREATURETYPE_BAT	
		|| g_pPlayer->GetCreatureType()==CREATURETYPE_WOLF	
		)
	{
		
		CGTradePrepare _CGTradePrepare;
		_CGTradePrepare.setTargetObjectID( targetID );
		_CGTradePrepare.setCode( CG_TRADE_PREPARE_CODE_REJECT );

		pClientPlayer->sendPacket( &_CGTradePrepare );

		return;
	}


	
	switch (pPacket->getCode())
	{
		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_PREPARE_CODE_REQUEST :
		{
			bool bLookMe = false;

			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE
				&& g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_REQUEST)
			{
				int code;

				//----------------------------------------------------------------------
				
				//----------------------------------------------------------------------
				if (g_pTempInformation->Value1==targetID)
				{
					code = CG_TRADE_PREPARE_CODE_ACCEPT;

					g_pTempInformation->SetMode(TempInformation::MODE_NULL);

					//-------------------------------------------
					
					//-------------------------------------------
					UI_RunExchange( targetID );

					bLookMe = true;
				}
				//----------------------------------------------------------------------
				
				//----------------------------------------------------------------------
				else
				{
					code = CG_TRADE_PREPARE_CODE_BUSY;
				}

				CGTradePrepare _CGTradePrepare;
				_CGTradePrepare.setTargetObjectID( targetID );
				_CGTradePrepare.setCode( code );				

				pClientPlayer->sendPacket( &_CGTradePrepare );

			}
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			else
			{
				//-------------------------------------------
				
				//-------------------------------------------
				UI_RunExchangeAsk( targetID );	
				
				bLookMe = true;
			}

			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			if (bLookMe)
			{
				MCreature* pCreature = g_pZone->GetCreature( targetID );
				
				if (pCreature!=NULL)
				{
					pCreature->SetDirectionToPosition( g_pPlayer->GetX(), g_pPlayer->GetY() );
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] No Such Creature. id=%d", targetID);
				}
			}
		}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_PREPARE_CODE_CANCEL :

			if (g_pTempInformation->GetMode() == TempInformation::MODE_TRADE_REQUEST)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}		

			
			if (UI_IsRunningExchange())
			{
				UI_CloseExchange();
			}
			else
			{
				UI_CloseExchangeAsk();
			}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_PREPARE_CODE_ACCEPT :
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE
				&& g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_REQUEST)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				//-------------------------------------------
				
				//-------------------------------------------
				UI_RunExchange( targetID );
			}

		break;


		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_PREPARE_CODE_REJECT :
		{
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE
				&& g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_REQUEST)
			{			
				
				g_pPlayer->SetWaitVerifyNULL();
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				
				UI_CloseExchangeCancel();

				
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_REJECTED].GetString());
			}
		}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_PREPARE_CODE_BUSY :
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_TRADE
				&& g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_REQUEST)
			{			
				
				g_pPlayer->SetWaitVerifyNULL();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);				

				UI_CloseExchangeAsk();
				UI_CloseExchangeCancel();

				
				g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_BUSY].GetString());
			}
		break;

	}

#endif

	__END_CATCH
}
