//----------------------------------------------------------------------
// MObjectSelector.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MObjectSelector.h"
#include "MPlayer.h"
#include "MJusticeAttackManager.h"
#include "InputService.h"
#include "Platform/InputManager.h"
#include "MEventManager.h"
#include "MWarManager.H"
#include "MZone.h"
#include "Packet/GuildWarInfo.h"
#include "Packet/RaceWarInfo.h"
#include "MZoneTable.h"
#include "MGuildType.h"
#include "UserInformation.h"
//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MObjectSelector* g_pObjectSelector = NULL;

//----------------------------------------------------------------------
//
// constructor / destructor
//
//----------------------------------------------------------------------
MObjectSelector::MObjectSelector()
{
	m_SelectTarget	= SELECT_ALL;
	m_SelectBy		= SELECT_BY_RACE;
}

MObjectSelector::~MObjectSelector()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------
//----------------------------------------------------------------------
// Can Select (MCreature*)
//----------------------------------------------------------------------
BOOL
MObjectSelector::CanSelect(MCreature* pCreature)
{
	
	
	
	
	
	
	
	
	
	
	WORD CurZoneID = g_pZone->GetID();
	// 2004, 9, 15, sobeit add start
	bool	IsAvailablePK = true;
//	if( g_pUserInformation->IsNonPK && NULL != pCreature )
//	{
//		int spriteType = (*g_pCreatureTable)[pCreature->GetCreatureType()].SpriteType;
//		if((*g_pCreatureSpriteTable)[spriteType].IsPlayerOnlySprite() && !pCreature->IsNPC())
//		{
//			if( g_pPlayer->GetCreatureTribe() != pCreature->GetCreatureTribe() || g_pSDLInput->KeyDown(DIK_LSHIFT))
//				IsAvailablePK = false;
//		}
//	}
	// 2004, 9, 15, sobeit add end
	return	(g_pSDLInput->KeyDown(DIK_LSHIFT)

			|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION)

			|| m_SelectTarget==SELECT_ALL

			|| (m_SelectBy==SELECT_BY_RACE 
					&& (m_SelectTarget==SELECT_ENEMY && g_pPlayer->CanAttackTribe( pCreature )
						|| m_SelectTarget==SELECT_FRIEND && !g_pPlayer->CanAttackTribe( pCreature )
						)
				)

			|| (m_SelectBy==SELECT_BY_GUILD
					&& (m_SelectTarget==SELECT_ENEMY && g_pPlayer->CanAttackGuild( pCreature )
						|| m_SelectTarget==SELECT_FRIEND && !g_pPlayer->CanAttackGuild( pCreature )
						)
				)

			|| g_pJusticeAttackManager->HasCreature( pCreature->GetName() )

			|| pCreature->IsNPC()
			|| IsWarEnemy( pCreature )
		//	|| g_pZone->GetPKType() == PK_TYPE_ALL
			|| (g_pPlayer->GetCreatureType() == CREATURETYPE_SLAYER_OPERATOR || g_pPlayer->GetCreatureType() == CREATURETYPE_VAMPIRE_OPERATOR || g_pPlayer->GetCreatureType() == CREATURETYPE_OUSTERS_OPERATOR )
			)
			&& IsAvailablePK
			
			;

}

//----------------------------------------------------------------------
// Can Attack (MCreature*)
//----------------------------------------------------------------------

//----------------------------------------------------------------------
BOOL	
MObjectSelector::CanAttack(MCreature* pCreature)
{
	
	
	
	
	
	
	WORD CurZoneID = g_pZone->GetID();

//	bool bFreePKZone = g_pZoneTable->Get( g_pZone->GetID() )->FreePK;
	// 2004, 9, 15, sobeit add start

	bool	IsAvailablePK = true;

	if(NULL == pCreature)return FALSE;
	if( g_pUserInformation->IsNonPK && NULL != pCreature )
	{
		int spriteType = (*g_pCreatureTable)[pCreature->GetCreatureType()].SpriteTypes[0];
		if((*g_pCreatureSpriteTable)[spriteType].IsPlayerOnlySprite()&& !pCreature->IsNPC())
			IsAvailablePK = false;
	}
	// 2004, 9, 15, sobeit add end
	
	return (
				g_pSDLInput->KeyDown(DIK_LSHIFT)
				|| g_pPlayer->HasEffectStatus(EFFECTSTATUS_HALLUCINATION)
				|| m_SelectBy==SELECT_BY_RACE 
						&& g_pPlayer->CanAttackTribe(pCreature)
				|| m_SelectBy==SELECT_BY_GUILD
						&& g_pPlayer->CanAttackGuild(pCreature)
				|| g_pJusticeAttackManager->HasCreature( pCreature->GetName() )
				|| IsWarEnemy( pCreature )
			)

			&& IsAvailablePK;
		//|| g_pZone->GetPKType() == PK_TYPE_ALL;
	
}
//BOOL	
//MObjectSelector::IsAblePKbyZone(MCreature* pCreature)
//{
//	if(pCreature == NULL || g_pZone == NULL)
//		return FALSE;
//	DWORD CreatureType = pCreature->GetCreatureType();

//	{
//		BYTE PkType = g_pZone->GetPKType();

//		{

//				return FALSE;

//				return TRUE;

//			{
//				int myGuildID = g_pPlayer->GetGuildNumber();	
//				int OtherGuildID = pCreature->GetGuildNumber();	
//				if( myGuildID		== GUILDID_VAMPIRE_DEFAULT ||
//					myGuildID		== GUILDID_SLAYER_DEFAULT ||
//					myGuildID		== GUILDID_OUSTERS_DEFAULT ||
//					OtherGuildID	== GUILDID_VAMPIRE_DEFAULT ||
//					OtherGuildID	== GUILDID_SLAYER_DEFAULT ||


//
//				if(myGuildID == OtherGuildID)

//				else
//					return TRUE;
//			}
//		}
//
//	}
//	return TRUE;
//}
//--------------------------------------------------------------

//--------------------------------------------------------------
BOOL	
MObjectSelector::IsWarEnemy(MCreature* pCreature)
{
	BOOL bWarSelect = FALSE;

	if(pCreature == NULL || g_pZone == NULL)
		return bWarSelect;

	
	WORD CurrentZoneID = g_pZone->GetID();
	if(g_pZone!=NULL && pCreature != NULL)
	{
		if(g_pZone->GetPKType() == PK_TYPE_ALL)
			return TRUE;
		else if(g_pZone->GetPKType() == PK_TYPE_SIEGE)
		{
			
			if(pCreature->GetObjectType() != MObject::TYPE_CREATURE || !pCreature->IsAvailableEffectStatus())
				return FALSE;
			// 2004, 11, 8, sobeit add end

			
			if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_DEFENDERL)||g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_REINFORCE))
			{
				if(!pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_DEFENDERL)&&!pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_REINFORCE))
					return TRUE;
			}
			if(g_pPlayer->GetUnionGuildID() > 0 && (g_pPlayer->GetUnionGuildID() == pCreature->GetUnionGuildID()))
			{
				return FALSE;
			}
			if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1) && !pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_1))
				return TRUE;
			else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2) && !pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_2))
				return TRUE;
			else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3) && !pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_3))
				return TRUE;
			else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4) && !pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_4))
				return TRUE;
			else if(g_pPlayer->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5) && !pCreature->HasEffectStatus(EFFECTSTATUS_SIEGE_ATTACKER_5))
				return TRUE;
		}
	}
//	// 2004, 5, 14 sobeit add start
//	DWORD CreatureType = pCreature->GetCreatureType();

//	{
//		BYTE PkType = g_pZone->GetPKType();

//		{

//				return FALSE;

//				return TRUE;

//			{
//				int myGuildID = g_pPlayer->GetGuildNumber();	
//				int OtherGuildID = pCreature->GetGuildNumber();	
//				if( myGuildID		== GUILDID_VAMPIRE_DEFAULT ||
//					myGuildID		== GUILDID_SLAYER_DEFAULT ||
//					myGuildID		== GUILDID_OUSTERS_DEFAULT ||
//					OtherGuildID	== GUILDID_VAMPIRE_DEFAULT ||
//					OtherGuildID	== GUILDID_SLAYER_DEFAULT ||


//
//				if(myGuildID == OtherGuildID)

//				else
//					return TRUE;
//			}
//		}
//	}
//	// 2004, 5, 14 sobeit add end

	if( g_pWarManager->IsExist( CurrentZoneID ) && g_pWarManager->getSize() > 0)		
	{


//			const WarInfoMap&			info = g_pWarManager->getWarInfoList();
//			WarInfoMap::const_iterator	itr = info.begin();

//
//			while(itr != info.end() && bWarSelect == FALSE)
//			{
//				WarInfo *info = (*itr).second;

//				{
//					GuildWarInfo *pInfo = (GuildWarInfo *)info;
//
//					int myGuildID = g_pPlayer->GetGuildNumber();			
//					GuildWarInfo::GuildIDList joinGuild = pInfo->getJoinGuilds();
//					list<GuildID_t>::const_iterator	itr = joinGuild.Begin();					
//					

//					{				
//						if( *itr == myGuildID )
//						{
//							if( pCreature->GetGuildNumber() != myGuildID )
//								bWarSelect = TRUE;
//							break;
//						}

//							creatureGuildCheck = true;
//					}					
//				}
//				itr++;
//			}

//				bWarSelect = TRUE;
//		} else
		{								
			WarInfo *info = g_pWarManager->GetWarInfo( CurrentZoneID );
			
			if(info == NULL)
				return FALSE;

			GuildWarInfo *pInfo = (GuildWarInfo *)info;

			if(info->getWarType() == WAR_GUILD)
			{																	
				int myGuildID = g_pPlayer->GetGuildNumber();			
				GuildWarInfo::GuildIDList joinGuild = pInfo->getJoinGuilds();
				std::list<GuildID_t>::const_iterator	itr = joinGuild.Begin();
				bool	creatureGuildCheck = false;								
				
				for(int i=0;i<joinGuild.getSize();i++,itr++)					
				{				
					if( *itr == myGuildID )
					{
						if( pCreature->GetGuildNumber() != myGuildID )
							bWarSelect = TRUE;
						break;
					}
					if( *itr == pCreature->GetGuildNumber() )
						creatureGuildCheck = true;
				}
				if(bWarSelect == FALSE && creatureGuildCheck == true)				
					bWarSelect = TRUE;
			}			
		}		
	}
	
	return bWarSelect;
}
