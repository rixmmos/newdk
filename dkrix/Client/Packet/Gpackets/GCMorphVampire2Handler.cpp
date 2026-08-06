//--------------------------------------------------------------------------------
//
// Filename    : GCMorphVampire2Handler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCMorphVampire2.h"
#include "ClientDef.h"

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCMorphVampire2Handler::execute ( GCMorphVampire2 * pPacket , Player * pPlayer )
	 
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
		const PCVampireInfo3& vi = pPacket->getSlayerInfo();

		MCreature* pCreature = g_pZone->GetCreature(vi.getObjectID());

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreatureWear;

			pCreature->SetZone( g_pZone );			

			pCreature->SetName( vi.getName().c_str() );

			//pCreature->SetCreatureType( 0 );
			Shape_t shape	= vi.getShape();
			bool bMale		= vi.getSex()==MALE;
			ItemType_t coatType = vi.getCoatType();
			pCreature->SetCreatureType( GetVampireCreatureType( shape, bMale, coatType ) );

			pCreature->SetMale( bMale );

			pCreature->SetGroundCreature();
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

			//si.getName()
			
			
			pCreature->SetGuildNumber( 2 );

			if (!g_pZone->AddCreature( pCreature ))
			{
				delete pCreature;
				pCreature = NULL;
			}
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else
		{
			pCreature->SetName( vi.getName().c_str() );

			
			pCreature->SetGuildNumber( 2 );

			//pCreature->SetCreatureType( 0 );
			Shape_t shape	= vi.getShape();
			bool bMale		= vi.getSex()==MALE;
			ItemType_t coatType = vi.getCoatType();
			pCreature->SetCreatureType( GetVampireCreatureType( shape, bMale, coatType ) );

			pCreature->SetMale( bMale );

			pCreature->SetGroundCreature();
			//pCreature->SetID(vi.getObjectID());
			
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
	}

#endif

	__END_CATCH
}
