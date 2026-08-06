//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopSellFailHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopSellFail.h"
#include "ClientDef.h"
#include "UIDialog.h"
#include "TempInformation.h"
#include "MGameStringTable.h"
#include "UIFunction.h"

void GCShopSellFailHandler::execute ( GCShopSellFail * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	
	g_pTempInformation->SetMode(TempInformation::MODE_NULL);

	
	UI_UnlockItemTrade();

	
	if(UI_IsRunningSwapAdvancementItem())
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[UI_STRING_MESSAGE_SWAP_ADVANCEMENT_ITEM_ERROR].GetString() );
	else
	// 2005, 1, 3, sobeit add end
		g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SELL].GetString() );

#endif

	__END_CATCH
}
