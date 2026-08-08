//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddNPCHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddNPC.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MGuildType.h"
#include "EffectSpriteTypeDef.h"
#include "MinTr.h"

extern int					g_nZoneLarge;
extern int					g_nZoneSmall;
extern bool					g_bZonePlayerInLarge;

static int ResolveNPCTypeFromServerSprite(int npcID, int serverSpriteType, const char* serverName)
{
	if (g_pCreatureTable == NULL)
		return serverSpriteType;

	int creatureCount = g_pCreatureTable->GetSize();

	if (npcID >= 0 && npcID < creatureCount)
	{
		CREATURETABLE_INFO& info = (*g_pCreatureTable)[npcID];
		if (info.SpriteTypes.GetSize() > 0 && info.SpriteTypes[0] == serverSpriteType)
			return npcID;
	}

	if (serverName != NULL && serverName[0] != '\0')
	{
		for (int i = 0; i < creatureCount; ++i)
		{
			CREATURETABLE_INFO& info = (*g_pCreatureTable)[i];
			if (info.SpriteTypes.GetSize() > 0
				&& info.SpriteTypes[0] == serverSpriteType
				&& _stricmp(info.Name.GetString(), serverName) == 0)
			{
				return i;
			}
		}
	}

	if (serverSpriteType >= 0 && serverSpriteType < creatureCount)
		return serverSpriteType;

	for (int i = 0; i < creatureCount; ++i)
	{
		CREATURETABLE_INFO& info = (*g_pCreatureTable)[i];
		if (info.SpriteTypes.GetSize() > 0 && info.SpriteTypes[0] == serverSpriteType)
			return i;
	}

	return serverSpriteType;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCAddNPCHandler::execute ( GCAddNPC * pPacket , Player * pPlayer )
	 
	throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	// message

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
			int npcID = pPacket->getNPCID();

			MNPC* pNPC = new MNPC;
			
			pNPC->SetZone( g_pZone );

			int zoneID	= (g_bZonePlayerInLarge?g_nZoneLarge : g_nZoneSmall);

			
			int creatureType = ResolveNPCTypeFromServerSprite(
				npcID,
				pPacket->getSpriteType(),
				pPacket->getName().c_str());

			if (creatureType < 0
				|| creatureType >= g_pCreatureTable->GetSize()
				|| (*g_pCreatureTable)[creatureType].SpriteTypes.GetSize() == 0)
			{
				DEBUG_ADD_FORMAT("[Error] Invalid NPC creature type. npcID=%d serverSprite=%d resolved=%d",
					npcID,
					pPacket->getSpriteType(),
					creatureType);
				delete pNPC;
				return;
			}

			int spriteType = (*g_pCreatureTable)[creatureType].SpriteTypes[0];			
			
			CREATURESPRITETABLE_INFO& spriteInfo = (*g_pCreatureSpriteTable)[spriteType];

			if (spriteInfo.IsNPCSprite()
				&& spriteInfo.IsSlayerSprite()
				|| spriteInfo.IsSlayerSprite())
			{
				// slayer NPC
				pNPC->SetGuildNumber( GUILDID_EVE );
			} else
			if( spriteInfo.IsOustersSprite() )
			{
				pNPC->SetGuildNumber( GUILDID_OUSTERS );
			}
			else if (zoneID==1007 || zoneID==1114 || zoneID==1115)
			{
				
				pNPC->SetGuildNumber( GUILDID_TEPEZ );
			}
			else
			{
				
				pNPC->SetGuildNumber( GUILDID_BATHORY );
			}

			//pNPC->SetCreatureType( 0 );
			pNPC->SetCreatureType( creatureType );
			pNPC->SetGroundCreature();
			pNPC->SetID(pPacket->getObjectID());
			//pNPC->SetAction(ACTION_MOVE);
			pNPC->SetPosition( pPacket->getX(), pPacket->getY() );
			pNPC->SetServerPosition( pPacket->getX(), pPacket->getY() );
			pNPC->SetDirection( pPacket->getDir() );
			pNPC->SetCurrentDirection( pPacket->getDir() );
			pNPC->SetAction( ACTION_STAND );

			pNPC->SetName( pPacket->getName().c_str() );

			pNPC->SetBodyColor1( pPacket->getMainColor() );
			pNPC->SetBodyColor2( pPacket->getSubColor() );

			// NPC ID
			pNPC->SetNPCID( pPacket->getNPCID() );
			
			//
			pNPC->SetStatus( MODIFY_MAX_HP, 100 );

//			_MinTrace("AddNPC : %s Dir:%d Pos(%d,%d) CreatureType : %d\n",pNPC->GetName(), pPacket->getDir(), pPacket->getX(), pPacket->getY(), pPacket->getNPCID() );
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (npcID==217)
			{
				pNPC->SetStatus( MODIFY_CURRENT_HP, 10 );
				pNPC->SetStatus( MODIFY_ALIGNMENT, -10000 );	
			}
			else
			{
				pNPC->SetStatus( MODIFY_CURRENT_HP, 100 );
			}

			if (!g_pZone->AddCreature( pNPC ))
			{
				delete pNPC;
				pNPC = NULL;
			}
			else
			{			
				//------------------------------------------------------------
				
				//------------------------------------------------------------
				SetAddonToSlayer( (MNPC*)pNPC, creatureType );

				//------------------------------------------------------------
				
				//------------------------------------------------------------
//				LoadCreatureType( pPacket->getNPCID() );

				MAttachEffect* pEffect = NULL;

				if( pNPC->GetCreatureType() == 638 || pNPC->GetCreatureType() == 657)					
					pEffect = pNPC->CreateAttachEffect( EFFECTSPRITETYPE_AMATA, 0xffff,0);
				else
				if( pNPC->GetCreatureType() == 639 || pNPC->GetCreatureType() == 654)
					pEffect = pNPC->CreateAttachEffect( EFFECTSPRITETYPE_RIPATY, 0xffff,0);

				if(pEffect != NULL)
				{
					pEffect->SetDirection( pNPC->GetDirection() );					
					pEffect->SetLink( ACTIONINFO_NULL, NULL );
					pEffect->SetAttachCreatureID( pNPC->GetID() );
				}
			}
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else
		{
			if (pCreature->GetClassType()==MCreature::CLASS_NPC)
			{
				
				pCreature->SetGroundCreature();
				
				//pCreature->SetAction(ACTION_MOVE);
				pCreature->SetServerPosition( pPacket->getX(), pPacket->getY() );				
				pCreature->MovePosition( pPacket->getX(), pPacket->getY() );
				pCreature->SetDirection( pPacket->getDir() );
				pCreature->SetCurrentDirection( pPacket->getDir() );
				pCreature->SetAction( ACTION_STAND );

				pCreature->SetBodyColor1( pPacket->getMainColor() );
				pCreature->SetBodyColor2( pPacket->getSubColor() );

				//
				//pCreature->SetStatus( MODIFY_MAX_HP, 100 );
				//pCreature->SetStatus( MODIFY_CURRENT_HP, 100 );
			}
			else
			{
				DEBUG_ADD_FORMAT("[Error] The Creature is Not NPC. id=%d", pPacket->getObjectID());
			}
		}
	}

	
//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_CREATURE_APPEAR_NPC );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
