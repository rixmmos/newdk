//--------------------------------------------------------------------------------
//
// Filename    : GCAddMonsterCorpseHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddMonsterCorpse.h"
#include "ClientDef.h"
#include "MItem.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddMonsterCorpseHandler::execute ( GCAddMonsterCorpse * pPacket , Player * pPlayer )
	 
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
		//----------------------------------------	
		
		//----------------------------------------	
		MCreature* pCreature = g_pZone->GetCreatureOnly( pPacket->getObjectID() );
		
		static bool IsIKilled = false;
		
		
		
		int MonsterType = pPacket->getMonsterType();
		if(MonsterType == 735 || MonsterType == 736 ||MonsterType == 737 || (MonsterType>= 792 && MonsterType<=800))
			return ;
		// 2005, 1, 18 , sobeit add end
		//---------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			
			MItem* pItem = g_pZone->GetItem( pPacket->getObjectID() );

			//---------------------------------------------------------
			//
			
			//
			//---------------------------------------------------------
			if (pItem==NULL)
			{
				DEBUG_ADD("New MonsterCorpse");
				
				//----------------------------------------	
				//
				
				//
				//----------------------------------------	
				MCreature*	pCreature = new MCreature;

				pCreature->SetZone( g_pZone );
				
				pCreature->SetCreatureType( pPacket->getMonsterType() );
				pCreature->SetGroundCreature();
				pCreature->SetID( pPacket->getObjectID() );
				pCreature->SetPosition( pPacket->getX(), pPacket->getY() );
				pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
				pCreature->SetDirection( pPacket->getDir() );
				pCreature->SetCurrentDirection( pPacket->getDir() );
				
				pCreature->SetStatus( MODIFY_MAX_HP, 100 );
				pCreature->SetStatus( MODIFY_CURRENT_HP, 0 );

				
				pCreature->SetCorpse();
				//pCreature->SetName( (*g_pCreatureTable)[pPacket->getMonsterType()].Name.GetString() );
				pCreature->SetName( pPacket->getMonsterName().c_str() );
				pCreature->SetDrainCreatureID( pPacket->getLastKiller() );

				
				if(pPacket->getLastKiller() == g_pPlayer->GetID() && !IsIKilled)
				{
					ExecuteHelpEvent(HELP_EVENT_KILL);
					IsIKilled = true;
				}
				// 2004, 5, 7 sobeit add end

				
				pCreature->SetGuildNumber( 1 );				

				
				
				if (!pPacket->gethasHead())
				{
					pCreature->RemoveHead();
				}

				//----------------------------------------	
				//
				
				//
				//----------------------------------------	
				MCorpse* pCorpse = (MCorpse*)MItem::NewItem( ITEM_CLASS_CORPSE );
				pCorpse->SetID( pPacket->getObjectID() );
				pCorpse->SetItemType( 0 );
				pCorpse->SetCreature( pCreature );
				pCorpse->SetPosition( pPacket->getX(), pPacket->getY() );		

				//---------------------------------------------------------
				
				//---------------------------------------------------------
				pCorpse->SetNumber( pPacket->getTreasureCount() );


				//----------------------------------------
				
				//----------------------------------------
				if (g_pZone->AddItem( pCorpse ))
				{
					//------------------------------------------------------------
					
					//------------------------------------------------------------
//					LoadCreatureType( pPacket->getMonsterType() );			
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] Can't add Corpse to Zone. id=%d, xy=(%d, %d)", pPacket->getObjectID(), pPacket->getX(), pPacket->getY());
					
					//---------------------------------------------------------
					
					
					//---------------------------------------------------------
					TYPE_OBJECTID oldItemID = g_pZone->GetItemID( pPacket->getX(), pPacket->getY() );

					DEBUG_ADD_FORMAT("Remove old Item id=%d", oldItemID);
						
					if (g_pZone->RemoveItem( oldItemID ))				
					{
						
						if (!g_pZone->AddItem( pCorpse ))
						{
							DEBUG_ADD_FORMAT("[Error] Can't add Corpse to Zone, too. id=%d, xy=(%d, %d)", pPacket->getObjectID(), pPacket->getX(), pPacket->getY());
							
							delete pCorpse;
						}
					}
					else
					{
						
						DEBUG_ADD_FORMAT("[Error] Can't remove old Item. id=%d, xy=(%d, %d)", oldItemID, pPacket->getX(), pPacket->getY());
						
						delete pCorpse;
					}								
				}
				
				
				if(pCreature->GetCreatureType() == 482 || pCreature->GetCreatureType() == 650)
				{
					if(pCreature->GetCreatureType() == 482)
						pCreature->AddEffectStatus(EFFECTSTATUS_CLIENTONLY_XMAS_TREE, 0xFFFF);
					pCreature->SetChatString((char *)pCreature->GetName());
				}
			}
			//---------------------------------------------------------
			//
			
			//
			//---------------------------------------------------------
			else
			{
				#ifdef	OUTPUT_DEBUG
					if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
					{
						
						DEBUG_ADD("[Collide] Already Exist the Corpse");
					}					
				#endif								

				//---------------------------------------------------------
				
				//---------------------------------------------------------
				pItem->SetNumber( pPacket->getTreasureCount() );
			}

		}
		//---------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------		
		else
		{
			DEBUG_ADD("[Collide] exist Creature");

			if (pCreature->GetCreatureType()!=pPacket->getMonsterType())
			{
				pCreature->SetCreatureType(pPacket->getMonsterType());
				pCreature->SetGroundCreature();
			}

			pCreature->MovePosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pCreature->SetDirection( pPacket->getDir() );
			pCreature->SetCurrentDirection( pPacket->getDir() );

			//pCreature->SetStatus( MODIFY_MAX_HP, 100 );
			pCreature->SetStatus( MODIFY_CURRENT_HP, 0 );

			//---------------------------------------------------------
			
			//---------------------------------------------------------
			if (!pCreature->IsDead())
			{
				pCreature->SetStop();
				pCreature->SetDead();		
			}

			pCreature->SetItemCount( pPacket->getTreasureCount() );

			
			pCreature->SetGuildNumber( 1 );
			pCreature->SetDrainCreatureID( pPacket->getLastKiller() );

			
			if(pPacket->getLastKiller() == g_pPlayer->GetID() && !IsIKilled)
			{
				ExecuteHelpEvent(HELP_EVENT_KILL);
				IsIKilled = true;
			}
			// 2004, 5, 7 sobeit add end
			
			if (!pPacket->gethasHead())
			{
				pCreature->RemoveHead();
			}
		}

	}

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_ITEM_APPEAR_CORPSE );	
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
