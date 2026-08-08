//--------------------------------------------------------------------------------
//
// Filename    : LCDeletePCOKHandler.cpp
// Written By  : Reiot
// Description : 
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCDeletePCOK.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
	#include "CLGetPCList.h"
#endif

#include "ClientDef.h"
#include "UIFunction.h"

//--------------------------------------------------------------------------------
//

//
//--------------------------------------------------------------------------------
void LCDeletePCOKHandler::execute ( LCDeletePCOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	#if __LINUX__

		ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

		cout << "                            " << endl;
		cout << "+--------------------------+" << endl;
		cout << "| PC deleted successfully! |" << endl;
		cout << "+--------------------------+" << endl;
		cout << "                            " << endl;

		
		CLGetPCList clGetPCList;
		pClientPlayer->sendPacket( &clGetPCList );	

		pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

	#elif defined(PLATFORM_WINDOWS)

		ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

		
		UI_DeleteCharacterOK();

		
		CLGetPCList clGetPCList;
		pClientPlayer->sendPacket( &clGetPCList );	

		pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CL_GET_PC_LIST );

		
		g_ModeNext = MODE_WAIT_PCLIST;

	#endif
	
#endif

	__END_CATCH
}
