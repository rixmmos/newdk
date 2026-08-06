//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRideMotorCycleOKHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRideMotorCycleOK.h"
#include "ClientDef.h"
#include "MItem.h"
#include "SoundDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCRideMotorCycleOKHandler::execute ( GCRideMotorCycleOK * pPacket , Player * pPlayer )
	 
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
	
	//------------------------------------------------------
	else
	{
		MItem* pItem = g_pZone->GetItem( pPacket->getObjectID() );

		//------------------------------------------					
		
		
		//------------------------------------------					
		if (pItem != NULL)
		{
			if (pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
			{
				//------------------------------------------					
				
				//------------------------------------------					
				g_pPlayer->SetAddonItem( pItem );
				
				//------------------------------------------
				
				//------------------------------------------					
				//g_pPlayer->SetMoveDevice( MCreature::MOVE_DEVICE_RIDE );

				//------------------------------------------
				
				
				//------------------------------------------
				//PlaySound( pItem->GetTileSoundID(),
				//			false,
				//			g_pPlayer->GetX(), g_pPlayer->GetY());

				//gC_vs_ui.PickUpItem(pItem);
				g_pZone->RemoveItem( pItem->GetID() );

				PlaySound( SOUND_WORLD_BIKE_GO );
			}
			else
			{
				DEBUG_ADD_FORMAT("[Error] id=%d is Not Motorcycle", pPacket->getObjectID());			
			}
		}
	}
	
	//------------------------------------------
	
	//------------------------------------------
	g_pPlayer->SetWaitVerifyNULL();

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_ITEM_RIDE_MOTORCYCLE );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
