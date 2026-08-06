//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddVampireHandler.cc
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddVampire.h"
#include "ClientDef.h"
#include "MCreatureTable.h"
#include "MTestDef.h"
#include "MGameStringTable.h"
extern void SetPetInfo(PetInfo* pPetInfo, TYPE_OBJECTID objectID);
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddVampireHandler::execute ( GCAddVampire * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		

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

			pCreature->SetName( vi.getName().c_str() );

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
 
			
			
			if( pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF )
			{
				pCreature->SetBodyColor1( vi.getCoatColor() );
				pCreature->SetBodyColor2( vi.getCoatColor() );
			} else
			{
				pCreature->SetBodyColor1( vi.getSkinColor() );
				pCreature->SetBodyColor2( vi.getCoatColor() );
			}
			// add by sonic 2006.10.29
			pCreature->SetMasterEffectType( vi.getMasterEffectColor() );
			// 2006.10.29 sonic add end
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
			pCreature->SetStatus( MODIFY_RANK, vi.getRank() );

			//vi.getName()
			

			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );
			
			
			pCreature->SetGuildNumber( vi.getGuildID() );
			pCreature->SetUnionGuildID( vi.getUnionID() );
			if( vi.getBatColor() != 0 )
				pCreature->SetBatColor( vi.getBatColor() );
			else
				pCreature->SetBatColor( 0xFFFF );
			
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsVampire()== true && pCreature->IsNPC() == false)
			{
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString((char*)pPacket->getStoreOutlook().getSign().c_str(),g_pUserOption->ChattingColor);
				else
					pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
			}
			

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
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, vi.getAdvancementLevel() );
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
			//pCreature->SetGroundCreature();

			
			pCreature->SetGuildNumber( vi.getGuildID() );
			pCreature->SetUnionGuildID( vi.getUnionID() );
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( vi.getX(), vi.getY() );
			pCreature->SetServerPosition( vi.getX(), vi.getY() );			
			pCreature->SetDirection( vi.getDir() );
			pCreature->SetCurrentDirection( vi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			if( pCreature->GetCreatureType() == CREATURETYPE_WER_WOLF )
			{
				pCreature->SetBodyColor1( vi.getCoatColor() );
				pCreature->SetBodyColor2( vi.getCoatColor() );
			} else
			{
				pCreature->SetBodyColor1( vi.getSkinColor() );
				pCreature->SetBodyColor2( vi.getCoatColor() );
			}

			// add by sonic 2006.10.29
			pCreature->SetMasterEffectType( vi.getMasterEffectColor() );
			// end
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
			pCreature->SetStatus( MODIFY_RANK, vi.getRank() );
			
			pCreature->SetWeaponSpeed( vi.getAttackSpeed() );
			if( vi.getBatColor() != 0 )
				pCreature->SetBatColor( vi.getBatColor() );
			else
				pCreature->SetBatColor( 0xFFFF );
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			
			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsVampire()== true && pCreature->IsNPC() == false)
			{
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString((char*)pPacket->getStoreOutlook().getSign().c_str(),g_pUserOption->ChattingColor);
				else
					pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
			}

			
		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );
			
			//--------------------------------------------------
			
			//--------------------------------------------------
			if (pPacket->getFromFlag()==1)
			{
				ComeFromPortal( pCreature );
			}
			
			
			if(pPacket->getPetInfo() != NULL)
				SetPetInfo(pPacket->getPetInfo(), pCreature->GetID());
			NicknameInfo* _tempNick = pPacket->getNicknameInfo();
			if(_tempNick != NULL)
			{
				
				if(_tempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM_FORCED ||
				   _tempNick->getNicknameType() == NicknameInfo::NICK_CUSTOM)
				{
					pCreature->SetNickName(_tempNick->getNicknameType(), (char*)_tempNick->getNickname().c_str());
					
				}
				else 
				{
					int TempIndex = _tempNick->getNicknameIndex();
					if(g_pNickNameStringTable == NULL || g_pNickNameStringTable->GetSize() == 0)
					{
						pCreature->SetNickName(_tempNick->getNicknameType(), (char*)"No Title");
						return;
					}
					if(TempIndex >= g_pNickNameStringTable->GetSize())
						TempIndex = 0;
					pCreature->SetNickName(_tempNick->getNicknameType(), (char*)(*g_pNickNameStringTable)[TempIndex].GetString());
				}
			}
		}
	}

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_VAMPIRE );
//	__END_HELP_EVENT

	__END_CATCH
}
