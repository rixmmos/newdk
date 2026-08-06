//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopBuyFailHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopBuyFail.h"
#include "ClientDef.h"
#include "UIDialog.h"
#include "TempInformation.h"
#include "MGameStringTable.h"
#include "UIFunction.h"

void GCShopBuyFailHandler::execute ( GCShopBuyFail * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	
	g_pTempInformation->SetMode(TempInformation::MODE_NULL);

	
	UI_UnlockItemTrade();

	switch (pPacket->getCode())
	{
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_BLACK_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_RED_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_BLUE_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_GREEN_STAR :
		case GC_SHOP_BUY_FAIL_NOT_ENOUGH_CYAN_STAR :
			
	
		default :
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_NO_ITEM].GetString() );
	}

#endif

	__END_CATCH
}
