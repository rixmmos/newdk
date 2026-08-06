//--------------------------------------------------------------------------------
//
// Filename    : GCAddOustersHandler.cpp
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCAddOusters.h"
#include "ClientDef.h"
#include "MCreatureTable.h"
#include "MTestDef.h"
#include "MGameStringTable.h"

extern void SetPetInfo(PetInfo* pPetInfo, TYPE_OBJECTID objectID);
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCAddOustersHandler::execute ( GCAddOusters * pPacket , Player * pPlayer )
	 
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
		const PCOustersInfo3 & oi = pPacket->getOustersInfo ();

		MCreature* pCreature = g_pZone->GetCreature(oi.getObjectID());

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			pCreature = new MCreatureWear;
			
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, oi.getAdvancementLevel() );

			pCreature->SetZone( g_pZone );

			pCreature->SetName( oi.getName().c_str() );

			//--------------------------------------------------
			
			//--------------------------------------------------
			ItemType_t coatType = oi.getCoatType();

			if (oi.getCompetence()==0)
			{
				pCreature->SetCompetence( 0 );
				pCreature->SetCreatureType( CREATURETYPE_OUSTERS_OPERATOR );
			}
			else
			{
				pCreature->SetCreatureType( GetOustersCreatureType( coatType ) );
			}

			pCreature->SetID(oi.getObjectID());
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->SetPosition( oi.getX(), oi.getY() );
			pCreature->SetServerPosition( oi.getX(), oi.getY() );
			pCreature->SetDirection( oi.getDir() );
			pCreature->SetCurrentDirection( oi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetBodyColor1( oi.getHairColor() );
			pCreature->SetBodyColor2( oi.getCoatColor() );
	
			// add by sonic 2006.10.29
			pCreature->SetMasterEffectType( oi.getMasterEffectColor() );
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

			pCreature->SetStatus( MODIFY_MAX_HP, oi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, oi.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, oi.getAlignment() );
			pCreature->SetStatus( MODIFY_RANK, oi.getRank() );

			//oi.getName()
			

			pCreature->SetWeaponSpeed( oi.getAttackSpeed() );
			
			
			pCreature->SetGuildNumber( oi.getGuildID() );
			pCreature->SetUnionGuildID( oi.getUnionID() );
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsOusters()== true && pCreature->IsNPC() == false)
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
			//--------------------------------------------------
			
			//--------------------------------------------------
			ItemType_t coatType = oi.getCoatType();

			if (oi.getCompetence()==0)
			{
				pCreature->SetCompetence( 0 );
				pCreature->SetCreatureType( CREATURETYPE_OUSTERS_OPERATOR );
			}
			else
			{
				pCreature->SetCreatureType( GetOustersCreatureType( coatType ) );
			}

			
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, oi.getAdvancementLevel() );
			pCreature->SetGuildNumber( oi.getGuildID() );
			pCreature->SetUnionGuildID( oi.getUnionID() );
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( oi.getX(), oi.getY() );
			pCreature->SetServerPosition( oi.getX(), oi.getY() );			
			pCreature->SetDirection( oi.getDir() );
			pCreature->SetCurrentDirection( oi.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetBodyColor1( oi.getHairColor() );
			pCreature->SetBodyColor2( oi.getCoatColor() );
			pCreature->SetMasterEffectType( oi.getMasterEffectColor() );

	
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

			pCreature->SetStatus( MODIFY_MAX_HP, oi.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, oi.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, oi.getAlignment() );
			pCreature->SetStatus( MODIFY_RANK, oi.getRank() );

			pCreature->SetWeaponSpeed( oi.getAttackSpeed() );
			
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());

			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsOusters()== true && pCreature->IsNPC() == false)
			{
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString((char*)pPacket->getStoreOutlook().getSign().c_str(),g_pUserOption->ChattingColor);
				else
					pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
 
//				pCreature->SetPersnalString((char *)pPacket->getStoreOutlook().getSign().c_str());
//				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			}

			
		}

		if (pCreature!=NULL)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			SetEffectInfo( pCreature, pPacket->getEffectInfo() );
			
			SetAddonToOusters( (MCreatureWear*)pCreature, &oi );			

			
			if(pPacket->getPetInfo() != NULL)
				SetPetInfo(pPacket->getPetInfo(), pCreature->GetID());

			DEBUG_ADD_FORMAT("[GCADDOUSTERS] ID = %d", pCreature->GetName());	
			
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

#endif

	__END_CATCH
}
