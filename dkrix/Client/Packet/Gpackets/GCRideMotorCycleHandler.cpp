//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRideMotorCycleHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRideMotorCycle.h"
#include "ClientDef.h"
#include "MItem.h"
#include "SoundDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCRideMotorCycleHandler::execute ( GCRideMotorCycle * pPacket , Player * pPlayer )
	 
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

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		if (pCreature!=NULL)
		{
			
			if (pCreature->IsWear())
			{
				MCreatureWear* pCreatureWear = (MCreatureWear*)pCreature;

				MItem* pItem = g_pZone->GetItem( pPacket->getTargetObjectID() );

				//------------------------------------------					
				
				
				//------------------------------------------					
				if (pItem != NULL)
				{
					if (pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
					{
						//------------------------------------------					
						
						
						//------------------------------------------
						pCreatureWear->SetAddonItem( pItem );						
						
						//------------------------------------------
						
						//------------------------------------------					
						//pCreatureWear->SetMoveDevice( MCreature::MOVE_DEVICE_RIDE );

						//------------------------------------------
						
						
						//------------------------------------------
						//PlaySound( pItem->GetTileSoundID(),
						//			false,
						//			pCreatureWear->GetX(), pCreatureWear->GetY());

						//gC_vs_ui.PickUpItem(pItem);
						g_pZone->RemoveItem( pItem->GetID() );

						
						PlaySound( SOUND_WORLD_BIKE_GO,
									false,
									g_pPlayer->GetX(),
									g_pPlayer->GetY());
					}
					else
					{
						DEBUG_ADD_FORMAT("[Error] id=%d is Not Motorcycle", pPacket->getTargetObjectID());
					}
				}
			}
		}
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		else
		{
			MItem* pItem = g_pZone->GetItem( pPacket->getTargetObjectID() );

			//------------------------------------------					
			
			
			//------------------------------------------					
			if (pItem != NULL)
			{
				if (pItem->GetItemClass()==ITEM_CLASS_MOTORCYCLE)
				{
					g_pZone->RemoveItem( pItem->GetID() );
				}
			}
		}
	}
	
#endif

	__END_CATCH
}
