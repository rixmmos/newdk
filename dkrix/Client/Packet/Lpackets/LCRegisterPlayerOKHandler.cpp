//////////////////////////////////////////////////////////////////////
//
// Filename    : LCRegisterPlayerOKHandler.cpp
// Written By  : Reiot
// Description : 
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "LCRegisterPlayerOK.h"
#include "UserInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"

#ifdef __GAME_CLIENT__

	#include "../ClientPlayer.h"
	#include "../Cpackets/CLGetPCList.h"

#endif

extern BOOL g_bNeedUpdate;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LCRegisterPlayerOKHandler::execute ( LCRegisterPlayerOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	

	
	// Debug Message

	if (!g_bNeedUpdate)
	{
		ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

		CLGetPCList clGetPCList;

		pClientPlayer->sendPacket( &clGetPCList );
		
		
		pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		UI_CloseUserRegistrationWindow();

		
		if( g_pUserInformation->UserID != NULL && g_pUserInformation->UserID.GetLength() < 15 )
			UI_BackupLoginID( g_pUserInformation->UserID );
		else
			UI_BackupLoginID( "DarkEden" );


		//------------------------------------------------------------
		
		//------------------------------------------------------------
		SetServerGroupName( pPacket->getGroupName().c_str() );
		//SetServerName( pPacket->getServerName().c_str() );

		//------------------------------------------------------------
		
		//------------------------------------------------------------	
		SetMode( MODE_WAIT_PCLIST );

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		bool bGoreLevel = pPacket->isAdult() && !g_pUserOption->UseTeenVersion;

		SetGoreLevel( bGoreLevel);	
		
	}

#endif

	__END_CATCH
}
