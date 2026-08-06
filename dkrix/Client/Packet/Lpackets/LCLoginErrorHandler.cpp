//////////////////////////////////////////////////////////////////////
//
// Filename    : LCLoginErrorHandler.cpp
// Written By  : Reiot
// Description : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "LCLoginError.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "ClientDef.h"

extern void		PopupErrorMessage(ErrorID errorID);

extern BOOL		g_bNeedUpdate;

static void TraceLoginFlowPacket(const char* step)
{
	(void)step;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LCLoginErrorHandler::execute ( LCLoginError * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	TraceLoginFlowPacket("LCLoginErrorHandler begin");

	//cout << "Access Denied... (" << pPacket->toString() << ")" << endl;
	//throw ProtocolException("Login Failed!");
	// Debug Message

	if (!g_bNeedUpdate)
	{
		
		//SetMode( MODE_MAINMENU );
		g_ModeNext = MODE_LOGIN_WRONG;

		
//		if ((ErrorID)pPacket->getErrorID()==ETC_ERROR)
//		{
//			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_ERROR_LOGIN_DENY].GetString());
//		}
//		else if ((ErrorID)pPacket->getErrorID() == IP_DENYED )
//		{
//			g_pUIDialog->PopupFreeMessageDlg("You can't play this. Play your country version, Please.");
//		}
//		else
		{
			//g_UIDialog.PopupFreeMessageDlg( pPacket->getMessage().c_str() );
			PopupErrorMessage( (ErrorID)pPacket->getErrorID() );
		}
	}
	TraceLoginFlowPacket("LCLoginErrorHandler end");

#endif
		
	__END_CATCH
}
