//--------------------------------------------------------------------------------
//
// Filename    : GCMoveHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCMove.h"
#include "../Player.h"
#include "ClientDef.h"

#ifdef __GAME_CLIENT__
	#include "../Cpackets/CGMove.h"
	#include "../ClientPlayer.h"
#endif


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCMoveHandler::execute ( GCMove * pPacket , Player * pPlayer )
	 
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
			MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());

			//--------------------------------------------------
			
			//--------------------------------------------------
			if (pCreature==NULL)
			{
				// message
				DEBUG_ADD("[Error] Not Exist");			 
				
					/*
				pCreature = new MCreature;
				pCreature->SetZone( g_pZone );

				pCreature->SetCreatureType( 0 );
				pCreature->SetGroundCreature();
				pCreature->SetID(pPacket->getObjectID());
				//pCreature->SetAction(ACTION_MOVE);
				pCreature->SetPosition( pPacket->getX(), pPacket->getY() );
				pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
				pCreature->SetDirection(pPacket->getDir());
				pCreature->SetAction( ACTION_STAND );

				g_pZone->AddCreature( pCreature );			
				*/
//				_MinTrace("afsaf");
//				_MinTrace("[GCMove] Not Exist Creature. (MyX:%d,MyY:%d,CreatureID:%d,MoveX:%d,MoveY:%d,Direction:%d)\n",
//					g_pPlayer->GetX(), g_pPlayer->GetY(), pPacket->getObjectID(), pPacket->getX(), pPacket->getY(),
//					pPacket->getDir() );
			}
			//--------------------------------------------------
			
			//--------------------------------------------------
			else
			{
				pCreature->PacketMove(pPacket->getX(), pPacket->getY(), pPacket->getDir());		
			}
		}
	

//	#endif

#endif

	__END_CATCH
}
