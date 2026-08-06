//--------------------------------------------------------------------------------
//
// Filename    : GCAddSlayerCorpseHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddSlayerCorpse.h"
#include "ClientDef.h"
#include "MItem.h"
#include "AddonDef.h"
#include "DrawTypeDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddSlayerCorpseHandler::execute ( GCAddSlayerCorpse * pPacket , Player * pPlayer )
	 
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
		const PCSlayerInfo3 & si = pPacket->getSlayerInfo();	
			
		//----------------------------------------	
		
		//----------------------------------------	
		MCreature* pCreature = g_pZone->GetCreatureOnly( si.getObjectID() );
		
		//---------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			
			MItem* pItem = g_pZone->GetItem( si.getObjectID() );

			//---------------------------------------------------------
			//
			
			//
			//---------------------------------------------------------
			if (pItem==NULL)
			{
				DEBUG_ADD("New SlayerCorpse");
				
				//----------------------------------------	
				//
				
				//
				//----------------------------------------	
				MCreatureWear*	pDeadCreature = new MCreatureWear;
				
				pDeadCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, si.getAdvancementLevel() );

				pDeadCreature->SetZone( g_pZone );
				
				//----------------------------------------	
				
				//----------------------------------------					
				if (si.getCompetence()==0)
				{
					pDeadCreature->SetCompetence( 0 );
					pDeadCreature->SetCreatureType( CREATURETYPE_SLAYER_OPERATOR );
					pDeadCreature->SetMale( si.getSex()==MALE );
				}
				else
				{
					pDeadCreature->SetCreatureType( (si.getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );
				}

				pDeadCreature->SetGuildNumber( si.getGuildID() );

				
				pDeadCreature->SetGroundCreature();
				pDeadCreature->SetID( si.getObjectID() );
				pDeadCreature->SetPosition( si.getX(), si.getY() );
				pDeadCreature->SetServerPosition( si.getX(), si.getY() );
				pDeadCreature->SetDirection( si.getDir() );
				pDeadCreature->SetCurrentDirection( si.getDir() );

				
				pDeadCreature->SetBodyColor1( si.getSkinColor() );

				pDeadCreature->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
				pDeadCreature->SetStatus( MODIFY_CURRENT_HP, 0 );			
				
				SetAddonToSlayer( pDeadCreature, &si );	

				
				pDeadCreature->SetCorpse();
				pDeadCreature->SetName( si.getName().c_str() );				

				//----------------------------------------	
				//
				
				//
				//----------------------------------------	
				MCorpse* pCorpse = (MCorpse*)MItem::NewItem( ITEM_CLASS_CORPSE );
				pCorpse->SetID( si.getObjectID() );
				pCorpse->SetItemType( 0 );
				pCorpse->SetCreature( pDeadCreature );
				pCorpse->SetPosition( si.getX(), si.getY() );	
				
				pCorpse->SetNumber( pPacket->getTreasureCount() );

				//----------------------------------------
				
				//----------------------------------------
				if (!g_pZone->AddItem( pCorpse ))
				{
					DEBUG_ADD("[Error] Can't Add to Zone.");
					
					//---------------------------------------------------------
					
					
					//---------------------------------------------------------
					TYPE_OBJECTID oldItemID = g_pZone->GetItemID( si.getX(), si.getY() );

					DEBUG_ADD_FORMAT("Remove old Item id=%d", oldItemID);
						
					if (g_pZone->RemoveItem( oldItemID ))				
					{
						
						if (!g_pZone->AddItem( pCorpse ))
						{
							DEBUG_ADD_FORMAT("[Error] Can't add Corpse to Zone, too. id=%d, xy=(%d, %d)", si.getObjectID(), si.getX(), si.getY());
							
							delete pCorpse;
						}
					}
					else
					{
						
						DEBUG_ADD_FORMAT("[Error] Can't remove old Item. id=%d, xy=(%d, %d)", oldItemID, si.getX(), si.getY());
						
						delete pCorpse;
					}	
				}				
			}
			//---------------------------------------------------------
			//
			
			//
			//---------------------------------------------------------
			else
			{
				DEBUG_ADD_FORMAT("[Error] Already Exist Another Item : class=%d type=%d", (int)pItem->GetItemClass(), (int)pItem->GetItemType());
				
				if (pItem->GetItemClass()==ITEM_CLASS_CORPSE)
				{
					//---------------------------------------------------------					
					
					
					//---------------------------------------------------------
					pItem->SetNumber( pPacket->getTreasureCount() );
				}
				else
				{
					DEBUG_ADD("The Item is Not Corpse");
				}
			}
		}
		//---------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------		
		else
		{
			DEBUG_ADD_FORMAT("Already Exist Creature. Set Dead id=%d", si.getObjectID());

			const PCSlayerInfo3 & si = pPacket->getSlayerInfo();

			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, si.getAdvancementLevel() );

			int creatureType = ((si.getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE);

			if (pCreature->GetCreatureType()!=creatureType)
			{
				if (si.getCompetence()==0)
				{
					pCreature->SetCompetence( 0 );
					pCreature->SetCreatureType( CREATURETYPE_SLAYER_OPERATOR );
					pCreature->SetMale( si.getSex()==MALE );
				}
				else
				{
					pCreature->SetCreatureType(creatureType);
				}

				pCreature->SetGroundCreature();				
			}

			pCreature->SetGuildNumber( si.getGuildID() );
			
			pCreature->MovePosition( si.getX(), si.getY() );
			pCreature->SetServerPosition( si.getX(), si.getY() );
			pCreature->SetDirection( si.getDir() );
			pCreature->SetCurrentDirection( si.getDir() );			
			
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
		}
	}

#endif

	__END_CATCH
}
