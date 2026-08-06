//////////////////////////////////////////////////////////////////////
//
// Filename    : GCStashSellHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCStashSell.h"
#include "ClientDef.h"
#include "MGameStringTable.h"
#include "TempInformation.h"
#include "UIDialog.h"
#include "UIFunction.h"


void GCStashSellHandler::execute ( GCStashSell * pPacket , Player * pPlayer )
	 


throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	__BEGIN_DEBUG
	
#ifdef __GAME_CLIENT__


	if ( g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK )
	{
		g_pPlayer->SetWaitVerifyNULL();
	
		DEBUG_ADD("[Verified] NPC Ask Answer OK");
	
		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pPacket->getPrice()==0)
		{
			
			g_pUIDialog->ShowPCTalkDlg();

			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_MORE].GetString());
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else
		{
			//------------------------------------------------------
			
			//------------------------------------------------------
			UI_RunStorageBuy( pPacket->getPrice() );

			//------------------------------------------------------
			
			//------------------------------------------------------
			g_pTempInformation->SetMode(TempInformation::MODE_STORAGE_BUY);
			g_pTempInformation->Value1	= pPacket->getPrice();
		}
	}

#endif

	__END_DEBUG
	__END_CATCH
}
