//--------------------------------------------------------------------------------
//
// Filename    : GCAddVampireFromBurrowingHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddVampireFromBurrowing.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "MTestDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddVampireFromBurrowingHandler::execute ( GCAddVampireFromBurrowing * pPacket , Player * pPlayer )
	 
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
		const PCVampireInfo3 & vi = pPacket->getVampireInfo ();

		MCreature* pCreature = g_pZone->GetCreature(vi.getObjectID());

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreatureWear;
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );

			pCreature->SetZone( g_pZone );

	
			//pCreature->SetCreatureType( 0 );
			//--------------------------------------------------
			
			//--------------------------------------------------
			Shape_t shape	= vi.getShape();
			bool bMale		= vi.getSex()==MALE;
			ItemType_t coatType = vi.getCoatType();

			if (vi.getCompetence()==0)
			{
				pCreature->SetCompetence( 0 );
				pCreature->SetCreatureType( CREATURETYPE_VAMPIRE_OPERATOR );
				pCreature->SetMale( bMale );
			}
			else
			{
				pCreature->SetCreatureType( GetVampireCreatureType( shape, bMale, coatType ) );
			}

			pCreature->SetMale( bMale );

			if (pCreature->GetCreatureType()==CREATURETYPE_BAT)
			{
				pCreature->SetFlyingCreature();
			}
			else
			{
				pCreature->SetGroundCreature();
			}

			pCreature->SetName( vi.getName().c_str() );

			
			pCreature->SetGuildNumber( vi.getGuildID() );
			
			pCreature->SetID(vi.getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( vi.getX(), vi.getY() );
			pCreature->SetServerPosition( vi.getX(), vi.getY() );
			pCreature->SetDirection( vi.getDir() );
			pCreature->SetCurrentDirection( vi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			pCreature->SetStatus( MODIFY_MAX_HP, vi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, vi.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, vi.getAlignment() );

			
			pCreature->SetBodyColor1( vi.getSkinColor() );
			pCreature->SetBodyColor2( vi.getCoatColor() );

			// speed
			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );

			
			//--------------------------------------------------
			// [ TEST CODE ]
			//--------------------------------------------------
			
			//--------------------------------------------------
			/*
			if (pCreature->IsMale())
			{
				pCreature->SetBodyColor2( 91 );
			}
			else
			{
				pCreature->SetBodyColor2( 38 );
			}
			*/

			if (!g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
				pCreature = NULL;
			}

#ifdef __METROTECH_TEST__
			extern int	g_iSpeed;
			if(g_iSpeed < 0)
				g_iSpeed = -1;
#endif
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else
		{
			pCreature->SetZone( g_pZone );
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );

			pCreature->SetName( vi.getName().c_str() );

			
			pCreature->SetGuildNumber( vi.getGuildID() );

			//pCreature->SetCreatureType( 0 );
			//--------------------------------------------------
			
			//--------------------------------------------------
			Shape_t shape	= vi.getShape();
			bool bMale		= vi.getSex()==MALE;
			ItemType_t coatType = vi.getCoatType();

			if (vi.getCompetence()==0)
			{
				pCreature->SetCompetence( 0 );
				pCreature->SetCreatureType( CREATURETYPE_VAMPIRE_OPERATOR );
				pCreature->SetMale( bMale );
			}
			else
			{
				pCreature->SetCreatureType( GetVampireCreatureType( shape, bMale, coatType ) );
			}

			pCreature->SetMale( bMale );

			if (pCreature->GetCreatureType()==CREATURETYPE_BAT)
			{
				pCreature->SetFlyingCreature();
			}
			else
			{
				pCreature->SetGroundCreature();
			}

			pCreature->SetID(vi.getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( vi.getX(), vi.getY() );
			pCreature->SetServerPosition( vi.getX(), vi.getY() );			
			pCreature->SetDirection( vi.getDir() );
			//pCreature->SetCurrentDirection( vi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetBodyColor1( vi.getSkinColor() );
			pCreature->SetBodyColor2( vi.getCoatColor() );


			// speed
			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );

			//--------------------------------------------------
			// [ TEST CODE ]
			//--------------------------------------------------
			
			//--------------------------------------------------
			/*
			if (pCreature->IsMale())
			{
				pCreature->SetBodyColor2( 91 );
			}
			else
			{
				pCreature->SetBodyColor2( 38 );
			}
			*/

			pCreature->SetStatus( MODIFY_MAX_HP, vi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, vi.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, vi.getAlignment() );
		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );

			//--------------------------------------------------
			
			//--------------------------------------------------		
			ExecuteActionInfoFromMainNode(
				RESULT_MAGIC_HIDE,										
			
				pCreature->GetX(), pCreature->GetY(), 0,
				pCreature->GetDirection(),														
				
				OBJECTID_NULL,												
				pCreature->GetX(), pCreature->GetY(), 0, 
				
				0,													
				
				NULL,
				
				false);			
		}
	}

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_VAMPIRE );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
