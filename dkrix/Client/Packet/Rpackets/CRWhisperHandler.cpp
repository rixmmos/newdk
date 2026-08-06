//////////////////////////////////////////////////////////////////////
//
// Filename    : CRWhisperHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "CRWhisper.h"
#include "ClientDef.h"
#include "UserInformation.h"
#include "MChatManager.h"
#include "../RequestServerPlayer.h"
#include "RequestUserManager.h"
#include "RCRequestVerify.h"
#include "UIFunction.h"
#include "MGameStringTable.h"

extern CMessageArray*		g_pNoticeMessage;

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void CRWhisperHandler::execute ( CRWhisper * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
	
	RequestServerPlayer* pRequestServerPlayer = dynamic_cast<RequestServerPlayer*>( pPlayer );
		
	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	if (g_Mode!=MODE_GAME						
		&& g_Mode!=MODE_WAIT_UPDATEINFO			
		&& g_Mode!=MODE_WAIT_SETPOSITION		
		|| g_pPlayer==NULL
		|| g_pChatManager==NULL
		|| g_pUserInformation==NULL 
		|| g_pUserInformation->CharacterID.GetString()==NULL
		|| pPacket->getTargetName() != g_pUserInformation->CharacterID.GetString()
		|| pPacket->getWorldID() != g_pUserInformation->WorldID)
	{		
		
		RCRequestVerify _RCRequestVerify;
		_RCRequestVerify.setCode( REQUEST_VERIFY_WHISPER_FAILED );
		pRequestServerPlayer->sendPacket( &_RCRequestVerify );

		
		try {
			pRequestServerPlayer->processOutput();
		} catch (Throwable& t)	{
			DEBUG_ADD_ERR(t.toString().c_str());
		}	
	}
	else
	{
		//-------------------------------------------------------------------
		
		//-------------------------------------------------------------------
		

		if (!g_pRequestUserManager->HasRequestUser(pPacket->getName().c_str()))
		{
			const char* pIP = pPlayer->getSocket()->getHost().c_str();

			g_pRequestUserManager->AddRequestUser( pPacket->getName().c_str(), pIP );
		}

		
		//RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo( pPacket->getName().c_str() );

		//if (pUserInfo!=NULL)
		{
		//	Socket* pSocket = pPlayer->getSocket();
		//	pUserInfo->IP = pSocket->getHost().c_str();
			//pUserInfo->TCPPort = pSocket->getPort();
		}


		int numMessage = pPacket->getMessageSize();

		char str[128];
		char strName[128];	
		strcpy(strName, pPacket->getName().c_str());

		//bool bMasterWords = (strstr(strName, "GM")!=NULL);
		bool bMasterWords = strncmp( strName, (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetString(), (*g_pGameStringTable)[UI_STRING_MESSAGE_MASTER_NAME].GetLength() ) == 0;
		
		if (bMasterWords 
			|| g_pChatManager->IsAcceptID( strName ))
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			for (int i=0; i<numMessage; i++)
			{
				WHISPER_MESSAGE* pString = pPacket->popMessage();

				if (pString==NULL)
				{
					break;
				}

				strcpy(str, pString->msg.c_str());

				//--------------------------------------------------
				
				
				
				//--------------------------------------------------
				if (!bMasterWords && !g_pUserInformation->IsMaster 
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
							g_pChatManager->AddMask(str, percent);
						}
//						else if (g_pPlayer->IsVampire() && !bVampireSay)
//						{

//							int percent = min(75, 25+g_pPlayer->GetINT()*100/300);
//							g_pChatManager->AddMask(str, percent);
//						}
					#endif
				}

				//sprintf(str, "<%s> %s", pPacket->getName().c_str(), pPacket->getMessage().c_str());
				// whisper = 2
				UI_AddChatToHistory( str, strName, 2, pString->color );
				
				if ( bMasterWords )
				{
					
					g_pNoticeMessage->AddFormat("%s>%s",strName,str);
				}

				
				char strWhisperID[128];
				sprintf(strWhisperID, "%s ", pPacket->getName().c_str());
				g_pUserInformation->WhisperID = strWhisperID;

				
//				__BEGIN_HELP_EVENT
//					ExecuteHelpEvent( HE_CHAT_WHISPERED );	
//				__END_HELP_EVENT


				delete pString;
			}
		}
	}

	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	throw DisconnectException("Disconnect after Whisper");
	/*
	try {

		pRequestServerPlayer->disconnect( UNDISCONNECTED );

	} catch (Throwable& t)	{
		DEBUG_ADD(t.toString().c_str());
	}
	*/

#endif

	__END_CATCH
}
