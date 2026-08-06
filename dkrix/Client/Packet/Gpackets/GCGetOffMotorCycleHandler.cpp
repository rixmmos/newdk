//////////////////////////////////////////////////////////////////////
//
// Filename    : GCGetOffMotorCycleHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCGetOffMotorCycle.h"
#include "ClientDef.h"
#include "SoundDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCGetOffMotorCycleHandler::execute ( GCGetOffMotorCycle * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	//------------------------------------------------------
	
	//------------------------------------------------------
	if (g_pZone==NULL)
	{
		// message
		DEBUG_ADD("[Error] Zone is Not Init.. yet.");			
	}
	//------------------------------------------------------
	
	//
	
	//------------------------------------------------------
	else
	{
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		if (pCreature!=NULL)
		{
			
			if (pCreature->IsWear())
			{
				MCreatureWear* pCreatureWear = (MCreatureWear*)pCreature;
			
				//------------------------------------------
				
				//------------------------------------------
				pCreatureWear->RemoveAddon( ADDON_MOTOR );

				//------------------------------------------
				
				//------------------------------------------
				pCreatureWear->SetMoveDevice( MCreature::MOVE_DEVICE_WALK );

				//------------------------------------------
				
				//------------------------------------------
				PlaySound( SOUND_WORLD_BIKE_STOP,
							false,
							g_pPlayer->GetX(),
							g_pPlayer->GetY());
			}
			else
			{
				DEBUG_ADD_FORMAT("[Error] Creature is not WearCreature id=%d", pPacket->getObjectID());
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] Not Exist Creature id=%d", pPacket->getObjectID());
		}
	}

#endif

	__END_CATCH
}
