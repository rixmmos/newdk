//--------------------------------------------------------------------------------
//
// Filename    : GCAddVampireFromTransformationHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddVampireFromTransformation.h"
#include "ClientDef.h"
#include "SkillDef.h"
#include "MCreatureTable.h"
#include "MTestDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddVampireFromTransformationHandler::execute ( GCAddVampireFromTransformation * pPacket , Player * pPlayer )
	 
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

			pCreature->SetID(vi.getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( vi.getX(), vi.getY() );
			pCreature->SetServerPosition( vi.getX(), vi.getY() );
			pCreature->SetDirection( vi.getDir() );
			pCreature->SetCurrentDirection( vi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetBodyColor1( vi.getSkinColor() );
			pCreature->SetBodyColor2( vi.getCoatColor() );
			
			// speed
			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );
			
			pCreature->SetName( vi.getName().c_str() );

			
			pCreature->SetGuildNumber( vi.getGuildID() );
			

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

			//vi.getName()
			

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
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );
			bool reAdd = false;

			
			if (pCreature->GetClassType()!=MCreature::CLASS_CREATUREWEAR)
			{
				g_pZone->RemoveCreature( pCreature->GetID() );

				pCreature = new MCreatureWear;

				pCreature->SetID( vi.getObjectID() );

				pCreature->SetZone( g_pZone );

				pCreature->SetName( vi.getName().c_str() );			

				pCreature->SetPosition( vi.getX(), vi.getY() );

				reAdd = true;
			}
			else
			{
				pCreature->MovePosition( vi.getX(), vi.getY() );
			}

		
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
			
			//pCreature->SetAction(ACTION_MOVE);			
			pCreature->SetServerPosition( vi.getX(), vi.getY() );
			pCreature->SetDirection( vi.getDir() );
			pCreature->SetCurrentDirection( vi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetBodyColor1( vi.getSkinColor() );
			pCreature->SetBodyColor2( vi.getCoatColor() );

			// speed
			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );

			
			pCreature->SetGuildNumber( vi.getGuildID() );
			
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

			if (reAdd && !g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
				pCreature = NULL;
			}
		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );

			//--------------------------------------------------
			
			//--------------------------------------------------		
			ExecuteActionInfoFromMainNode(

				RESULT_MAGIC_UN_TRANSFORM,
			
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
