//////////////////////////////////////////////////////////////////////
//
// Filename    : GCWhisperHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCWhisper.h"
#include "ClientDef.h"
#include "UserInformation.h"
#include "MChatManager.h"
#include "UIFunction.h"
#include "MGameStringTable.h"

extern CMessageArray*		g_pNoticeMessage;

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCWhisperHandler::execute ( GCWhisper * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__


	
	// 256: GCWhisper's read() cap of 128 matches the server exactly, so the
	// only defect was the missing room for the NUL. Enlarge rather than tighten
	// the cap, which would silently drop the longest legal whisper.
	char str[256];
	char strName[256];
	strcpy(str, pPacket->getMessage().c_str());
	strcpy(strName, pPacket->getName().c_str());

//	bool bMasterWords = (strstr(strName, "GM")!=NULL);
	bool bMasterWords = strncmp( strName, (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetString(), (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetLength() ) == 0;

	if (bMasterWords 
		|| g_pChatManager->IsAcceptID( strName ))
	{
		//--------------------------------------------------
		
		
		
		//--------------------------------------------------
		if (!bMasterWords && !g_pUserInformation->IsMaster &&
			g_pPlayer->GetCreatureType() != CREATURETYPE_SLAYER_OPERATOR &&
			g_pPlayer->GetCreatureType() != CREATURETYPE_VAMPIRE_OPERATOR &&
			g_pPlayer->GetCreatureType() != CREATURETYPE_OUSTERS_OPERATOR 
			&& !g_pPlayer->HasEffectStatus( EFFECTSTATUS_GHOST )
#ifdef __METROTECH_TEST__
				&& !g_bLight
#endif
			)
		{
			g_pChatManager->RemoveCurse( str, true );

			#ifndef _DEBUG
				//--------------------------------------------------
				
				//--------------------------------------------------
				Race race = (Race)pPacket->getRace();
				if (g_pPlayer->GetRace() != race)
				{
					
					int percent = min(75, 25+g_pPlayer->GetINT()*100/(min(2, g_pPlayer->GetRace()+1)*150));
//					if(g_pPlayer->GetRace() == RACE_OUSTERS || race == RACE_OUSTERS)
//						percent = 70;
					
					g_pChatManager->AddMask(str, percent);
				}
//				else if (g_pPlayer->IsVampire() && !bVampireSay)
//				{

//					int percent = min(75, 25+g_pPlayer->GetINT()*100/300);
//					g_pChatManager->AddMask(str, percent);
//				}
			#endif
		}

		//sprintf(str, "<%s> %s", pPacket->getName().c_str(), pPacket->getMessage().c_str());
		// whisper = 2
		UI_AddChatToHistory( str, strName, 2, pPacket->getColor() );
		
		if ( bMasterWords )
		{
			
			g_pNoticeMessage->AddFormat("%s>%s",strName,str);
		}

		
		char strWhisperID[128];
		sprintf(strWhisperID, "%s ", pPacket->getName().c_str());
		g_pUserInformation->WhisperID = strWhisperID;

		
//		__BEGIN_HELP_EVENT
////			ExecuteHelpEvent( HE_CHAT_WHISPERED );	
//		__END_HELP_EVENT
	}


#endif

	__END_CATCH
}
