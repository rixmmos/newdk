//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddSlayerHandler.cc
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddSlayer.h"
#include "ClientDef.h"
#include "AddonDef.h"
#include "MCreatureTable.h"
#include "MTestDef.h"
#include "MGameStringTable.h"

extern void SetPetInfo(PetInfo* pPetInfo, TYPE_OBJECTID objectID);

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddSlayerHandler::execute ( GCAddSlayer * pPacket , Player * pPlayer )
	 
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
			
		DEBUG_ADD_FORMAT("[AddSlayer] id=%d, xy=(%d,%d), dir=%d", si.getObjectID(), si.getX(), si.getY(), si.getDir());				
		
		MCreature* pCreature = g_pZone->GetCreature(si.getObjectID());

		//--------------------------------------------------
		
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			MCreatureWear* pCreatureWear = new MCreatureWear;

			pCreatureWear->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, si.getAdvancementLevel() );
			pCreatureWear->SetZone( g_pZone );

			pCreature = (MCreature*)pCreatureWear;

			if (si.getCompetence()==0)
			{
				pCreatureWear->SetCompetence( 0 );
				pCreatureWear->SetCreatureType( CREATURETYPE_SLAYER_OPERATOR );
				pCreatureWear->SetMale( si.getSex()==MALE );
			}
			else
			{
				pCreatureWear->SetCreatureType( (si.getSex()==MALE)? CREATURETYPE_SLAYER_MALE : CREATURETYPE_SLAYER_FEMALE );
			}

			pCreatureWear->SetGuildNumber( si.getGuildID() );
			pCreatureWear->SetUnionGuildID( si.getUnionID() );

			pCreatureWear->SetGroundCreature();
			pCreatureWear->SetID(si.getObjectID());
			//pCreatureWear->SetAction(ACTION_MOVE);
			pCreatureWear->SetPosition( si.getX(), si.getY() );
			pCreatureWear->SetServerPosition( si.getX(), si.getY() );
			pCreatureWear->SetDirection( si.getDir() );
			pCreatureWear->SetCurrentDirection( si.getDir() );
			pCreatureWear->SetAction( ACTION_STAND );

			
			pCreatureWear->SetBodyColor1( si.getSkinColor() );

			pCreature->SetMasterEffectType(si.getMasterEffectColor());

			pCreatureWear->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
			pCreatureWear->SetStatus( MODIFY_CURRENT_HP, si.getCurrentHP() );
			pCreatureWear->SetStatus( MODIFY_ALIGNMENT, si.getAlignment() );
			pCreatureWear->SetStatus( MODIFY_RANK, si.getRank() );

			
			pCreatureWear->SetName( si.getName().c_str() );

			

			
			//----------------------------------------	
			
			//----------------------------------------	
			SetAddonToSlayer( pCreatureWear, &si );
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());

			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsSlayer()== true && pCreature->IsNPC() == false)
			{
				if(!pPacket->getStoreOutlook().getSign().empty())
					pCreature->SetPersnalString((char*)pPacket->getStoreOutlook().getSign().c_str(),g_pUserOption->ChattingColor);
				else
					pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);

			}

			if (!g_pZone->AddCreature( pCreatureWear ))
			{
				delete pCreatureWear;
				pCreatureWear = NULL;
				pCreature = NULL;
			}
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else
		{
			pCreature->SetStatus( MODIFY_ADVANCEMENT_CLASS_LEVEL, si.getAdvancementLevel() );
			pCreature->SetGuildNumber( si.getGuildID() );
			pCreature->SetUnionGuildID( si.getUnionID() );

			pCreature->SetGroundCreature();
			
			//pCreature->SetAction(ACTION_MOVE);
			pCreature->MovePosition( si.getX(), si.getY() );
			pCreature->SetServerPosition( si.getX(), si.getY() );
			pCreature->SetDirection( si.getDir() );
			pCreature->SetCurrentDirection( si.getDir() );
			pCreature->SetAction( ACTION_STAND );

			
			pCreature->SetBodyColor1( si.getSkinColor() );
			pCreature->SetMasterEffectType(si.getMasterEffectColor());

			pCreature->SetStatus( MODIFY_MAX_HP, si.getMaxHP() );
			pCreature->SetStatus( MODIFY_CURRENT_HP, si.getCurrentHP() );
			pCreature->SetStatus( MODIFY_ALIGNMENT, si.getAlignment() );
			pCreature->SetStatus( MODIFY_RANK, si.getRank() );
			
			if(pCreature->IsNPC() == false)
				pCreature->SetPersnalShop(pPacket->getStoreOutlook().isOpen());
			
			if(pPacket->getStoreOutlook().isOpen()&& g_pPlayer->IsSlayer()== true && pCreature->IsNPC() == false)
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
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_SLAYER );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
