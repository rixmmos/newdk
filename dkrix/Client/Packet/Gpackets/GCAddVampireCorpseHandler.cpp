//--------------------------------------------------------------------------------
//
// Filename    : GCAddVampireCorpseHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddVampireCorpse.h"
#include "ClientDef.h"
#include "MItem.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddVampireCorpseHandler::execute ( GCAddVampireCorpse * pPacket , Player * pPlayer )
	 
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
		const PCVampireInfo3 & vi = pPacket->getVampireInfo();

		//----------------------------------------	
		
		//----------------------------------------	
		MCreature* pCreature = g_pZone->GetCreatureOnly( vi.getObjectID() );
		
		//---------------------------------------------------------
		//
		
		//
		//---------------------------------------------------------
		if (pCreature==NULL)
		{
			
			MItem* pItem = g_pZone->GetItem( vi.getObjectID() );

			//---------------------------------------------------------
			//
			
			//
			//---------------------------------------------------------
			if (pItem==NULL)
			{
				//Color_t getHairColor () const throw () { return m_Colors[VAMPIRE_COLOR_HAIR]; }
				//Color_t getSkinColor () const throw () { return m_Colors[VAMPIRE_COLOR_SKIN]; }
				//Color_t getCoatColor ( ColorType colorType = MAIN_COLOR ) const throw () { return m_Colors[VAMPIRE_COLOR_COAT1 + (int)colorType]; }

				//----------------------------------------	
				//
				
				//
				//----------------------------------------	
				DEBUG_ADD("New VampireCorpse");
				
				MCreatureWear*	pDeadCreature = new MCreatureWear;
				pDeadCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );

				pDeadCreature->SetZone( g_pZone );
				
				
				//--------------------------------------------------
				
				//--------------------------------------------------
				Shape_t shape	= vi.getShape();
				bool bMale		= vi.getSex()==MALE;
				ItemType_t coatType = vi.getCoatType();

				if (vi.getCompetence()==0)
				{
					pDeadCreature->SetCompetence( 0 );
					pDeadCreature->SetCreatureType( CREATURETYPE_VAMPIRE_OPERATOR );
					pDeadCreature->SetMale( bMale );
				}
				else
				{
					pDeadCreature->SetCreatureType( GetVampireCreatureType( shape, bMale, coatType ) );
				}

				pDeadCreature->SetMale( bMale );				
				
				
				if (pDeadCreature->GetCreatureType()==CREATURETYPE_BAT)
				{
					pDeadCreature->SetFlyingCreature();
				}
				else
				{
					pDeadCreature->SetGroundCreature();
				}

				pDeadCreature->SetID( vi.getObjectID() );
				pDeadCreature->SetPosition( vi.getX(), vi.getY() );
				pDeadCreature->SetServerPosition( vi.getX(), vi.getY() );
				pDeadCreature->SetDirection( vi.getDir() );
				pDeadCreature->SetCurrentDirection( vi.getDir() );

				pDeadCreature->SetStatus( MODIFY_MAX_HP, vi.getMaxHP() );
				pDeadCreature->SetStatus( MODIFY_CURRENT_HP, 0 );
				
				
				pDeadCreature->SetCorpse();
				pDeadCreature->SetName( vi.getName().c_str() );

				
				pDeadCreature->SetGuildNumber( vi.getGuildID() );

				pDeadCreature->SetWeaponSpeed( vi.getAttackSpeed() );
				pDeadCreature->SetStatus( MODIFY_ALIGNMENT, vi.getAlignment() );

				
				pDeadCreature->SetBodyColor1( vi.getSkinColor() );
				pDeadCreature->SetBodyColor2( vi.getCoatColor() );
				

				//--------------------------------------------------
				// [ TEST CODE ]
				//--------------------------------------------------
				
				//--------------------------------------------------
				/*
				if (pDeadCreature->IsMale())
				{
					pDeadCreature->SetBodyColor2( 91 );
				}
				else
				{
					pDeadCreature->SetBodyColor2( 38 );
				}
				*/

				//----------------------------------------	
				//
				
				//
				//----------------------------------------	
				MCorpse* pCorpse = (MCorpse*)MItem::NewItem( ITEM_CLASS_CORPSE );
				pCorpse->SetID( vi.getObjectID() );
				pCorpse->SetItemType( 0 );
				pCorpse->SetCreature( pDeadCreature );
				pCorpse->SetPosition( vi.getX(), vi.getY() );		

				pCorpse->SetNumber( pPacket->getTreasureCount() );

				//----------------------------------------
				
				//----------------------------------------
				if (!g_pZone->AddItem( pCorpse ))
				{
					//---------------------------------------------------------
					
					
					//---------------------------------------------------------
					TYPE_OBJECTID oldItemID = g_pZone->GetItemID( vi.getX(), vi.getY() );

					DEBUG_ADD_FORMAT("Remove old Item id=%d", oldItemID);
						
					if (g_pZone->RemoveItem( oldItemID ))				
					{
						
						if (!g_pZone->AddItem( pCorpse ))
						{
							DEBUG_ADD_FORMAT("[Error] Can't add Corpse to Zone, too. id=%d, xy=(%d, %d)", vi.getObjectID(), vi.getX(), vi.getY());
							
							delete pCorpse;
						}
					}
					else
					{
						
						DEBUG_ADD_FORMAT("[Error] Can't remove old Item. id=%d, xy=(%d, %d)", oldItemID, vi.getX(), vi.getY());
						
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
			const PCVampireInfo3 & vi = pPacket->getVampireInfo();

			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );

			Shape_t shape	= vi.getShape();
			bool bMale		= vi.getSex()==MALE;
			ItemType_t coatType = vi.getCoatType();

			int creatureType = GetVampireCreatureType( shape, bMale, coatType );

			if (pCreature->GetCreatureType()!=creatureType)
			{
				if (vi.getCompetence()==0)
				{
					pCreature->SetCompetence( 0 );
					pCreature->SetCreatureType( CREATURETYPE_VAMPIRE_OPERATOR );
					pCreature->SetMale( bMale );
				}
				else
				{
					pCreature->SetCreatureType(creatureType);
				}

				pCreature->SetGroundCreature();
			}

			pCreature->MovePosition( vi.getX(), vi.getY() );
			pCreature->SetServerPosition( vi.getX(), vi.getY() );
			pCreature->SetDirection( vi.getDir() );
			pCreature->SetCurrentDirection( vi.getDir() );

		//	pCreature->SetStatus( MODIFY_MAX_HP, vi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, 0 );

			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, vi.getAlignment() );

			
			pCreature->SetGuildNumber( vi.getGuildID() );

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
