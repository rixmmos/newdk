//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetOffMotorCycleOKHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGetOffMotorCycleOK.h"
#include "ClientDef.h"
#include "SoundDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCGetOffMotorCycleOKHandler::execute ( GCGetOffMotorCycleOK * pPacket, Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	//------------------------------------------
	
	//------------------------------------------
	g_pPlayer->RemoveAddon( ADDON_MOTOR );

	//------------------------------------------
	
	//------------------------------------------
	g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );

	//------------------------------------------
	
	//------------------------------------------
	g_pPlayer->SetWaitVerifyNULL();

	//------------------------------------------
	
	//------------------------------------------
	PlaySound( SOUND_WORLD_BIKE_STOP );

#endif

	__END_CATCH
}
