//--------------------------------------------------------------------------------
//
// Filename    : LCReconnectHandler.cpp
// Written By  : Reiot
// Description : 
//
//--------------------------------------------------------------------------------

// include files

#include "Client_PCH.h"
#include "LCReconnect.h"
#include "ClientDef.h"
#include "ServerInfoFileParser.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
	#include "../Cpackets/CGConnect.h"
	#include "../Cpackets/CGPortCheck.h"
	#include "UserInformation.h"
	#include "Packet/Properties.h"
	#include "../ClientCommunicationManager.h"
	//add by viva
	#include "../Cpackets/CGConnectSetKey.h"
	//end
#endif

#ifdef OUTPUT_DEBUG
	#include <time.h>

	struct tempStruct
	{
		SYSTEMTIME st;
		DWORD reconnectTickCount;
		DWORD sendCGConnectTickCount;
	};
#endif

// ACProtect include removed (SDL2) - Copy protection no longer needed

extern int g_Dimension;
extern bool		UpdateSocketOutput();
extern BYTE g_macAddress[6];

static void TraceGameEntryFlow(const char* step)
{
	(void)step;
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
void LCReconnectHandler::execute ( LCReconnect * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

		 
#ifdef OUTPUT_DEBUG
	FILE *fp = NULL;
	DWORD tickCount = 0;


	std::list<struct tempStruct> tempStructList;
	struct tempStruct currentTempStruct;

	fp = fopen("Log\\ConnectTime.txt", "rt");
	if(fp != NULL)
	{
		char readTemp[512];
		fgets(readTemp, 512, fp);
		while(fgets(readTemp, 512, fp) != NULL)
		{
			if(readTemp[0] == 'r')break;
			
			struct tempStruct ts;

			DWORD reconnectTickCount = 0, sendCGConnectTickCount = 0;
			sscanf(readTemp, "%4d/%2d/%2d %2d:%2d:%2d\t%8d\t%8d\n", 
				&ts.st.wYear, &ts.st.wMonth, &ts.st.wDay, &ts.st.wHour, &ts.st.wMinute, &ts.st.wSecond,
				&ts.reconnectTickCount,
				&ts.sendCGConnectTickCount);

			tempStructList.push_back(ts);
		}

		fclose(fp);
	}

// 	fp = fopen("Log\\ConnectTime.txt", "wt");
//	if(fp != NULL)
//	{
// 		fprintf(fp, "CurrentTime\tReconnect TickCount\tSend cgConnect TickCount\n");

		SYSTEMTIME st;
		GetLocalTime(&st);

		currentTempStruct.st = st;
//		fprintf(fp, "%4d/%2d/%2d %2d:%2d:%2d\t", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
//		fclose(fp);
// 	}
#endif

	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

	
	
	
	pClientPlayer->disconnect();

	
	DEBUG_ADD_FORMAT("Reconnecting to %s:%d", 
										pPacket->getGameServerIP().c_str(), 
										pPacket->getGameServerPort());
	
	try {
#ifdef OUTPUT_DEBUG
		tickCount = timeGetTime();//GetTickCount();
#endif
		pClientPlayer->getSocket()->reconnect( pPacket->getGameServerIP() , pPacket->getGameServerPort() );
		TraceGameEntryFlow("LCReconnectHandler reconnect OK");
		pClientPlayer->resetStreams();
		TraceGameEntryFlow("LCReconnectHandler reset streams");
#ifdef OUTPUT_DEBUG
		currentTempStruct.reconnectTickCount = timeGetTime()-tickCount;
#endif
		
		
		pClientPlayer->getSocket()->setNonBlocking();
		pClientPlayer->getSocket()->setLinger(0);

	} catch ( ConnectException & ce ) {
		throw Error(ce.toString());
	}

	
	
	DEBUG_ADD_FORMAT("Sending CGConnect with Key(%ld)", 
												pPacket->getKey());

	//add by viva	
	CGConnectSetKey cgConnectSetKey;
	cgConnectSetKey.setEncryptKey(rand());
	cgConnectSetKey.setHashKey(rand());
	if (pPacket->getGameServerIP() != "127.0.0.1")
	{
		pClientPlayer->delKey();
		pClientPlayer->resetStreams();
		pClientPlayer->sendPacket(&cgConnectSetKey);
		UpdateSocketOutput();
		TraceGameEntryFlow("LCReconnectHandler sent CGConnectSetKey");
		Sleep(500);
		cgConnectSetKey.execute(pClientPlayer);
	}
	else
	{
		TraceGameEntryFlow("LCReconnectHandler skipped CGConnectSetKey for localhost");
	}
	//end

	
	CGConnect cgConnect;
	cgConnect.setKey( pPacket->getKey() );
	cgConnect.setPCType( pClientPlayer->getPCType() );
	cgConnect.setPCName( pClientPlayer->getPCName() );
	cgConnect.setMacAddress( g_macAddress );
	{
		char traceBuffer[256];
		sprintf(traceBuffer, "LCReconnectHandler CGConnect name=%s pcType=%d", pClientPlayer->getPCName().c_str(), pClientPlayer->getPCType());
		TraceGameEntryFlow(traceBuffer);
	}
	
#ifdef OUTPUT_DEBUG
	tickCount = timeGetTime();
#endif
	pClientPlayer->sendPacket( &cgConnect );
	pClientPlayer->setPlayerStatus( CPS_AFTER_SENDING_CG_CONNECT );	
	TraceGameEntryFlow("LCReconnectHandler sent CGConnect");

	
//	EMBEDDED_BEGIN;
	UpdateSocketOutput();
	TraceGameEntryFlow("LCReconnectHandler flushed CGConnect");
//	EMBEDDED_END;
	
#ifdef OUTPUT_DEBUG
	currentTempStruct.sendCGConnectTickCount = timeGetTime()-tickCount;
#endif
	
	
	
	CGPortCheck cgPortCheck;
	cgPortCheck.setPCName( g_pUserInformation->CharacterID.GetString() );

	std::string ServerAddress;
	uint ServerPort;

	if( g_pUserInformation->bKorean )
	{
		ServerAddress = g_pConfigKorean->getProperty("LoginServerAddress");
		ServerPort = g_pConfigKorean->getPropertyInt("LoginServerCheckPort");
		
	}
	else
	{
		ServerAddress = g_pConfigForeign->getProperty( g_Dimension, "LoginServerAddress" );	
		ServerPort = g_pConfigForeign->getPropertyInt(g_Dimension, "LoginServerCheckPort");
	}
	
			
	DEBUG_ADD("[ ClientPacket] Send CGPortCheck ");
	g_pClientCommunicationManager->sendPacket( ServerAddress,
		ServerPort,
		&cgPortCheck );

	DEBUG_ADD("[ MODE ] SKIP SETMODE MODE_WAIT_UPDATEINFO");
	TraceGameEntryFlow("LCReconnectHandler skipped MODE_WAIT_UPDATEINFO");

#ifdef OUTPUT_DEBUG
 	fp = fopen("Log\\ConnectTime.txt", "wt");
	if(fp != NULL)
	{
		tempStructList.push_back(currentTempStruct);

 		fprintf(fp, "CurrentTime\tReconnect TickCount\tSend cgConnect TickCount\n");
		
		DWORD minReconnectTickCount = 0xffffffff, minCGTickCount = 0xffffffff, maxReconnectTickCount = 0, maxCGTickCount = 0, reconnectSum = 0, CGSum = 0;
		int size = tempStructList.size();

		while(!tempStructList.empty())
		{
			tempStruct ts;
			ts = tempStructList.front();
			tempStructList.pop_front();

			fprintf(fp, "%4d/%2d/%2d %2d:%2d:%2d\t%8d\t%8d\n", 
				ts.st.wYear, ts.st.wMonth, ts.st.wDay, ts.st.wHour, ts.st.wMinute, ts.st.wSecond,
				ts.reconnectTickCount,
				ts.sendCGConnectTickCount);

			if(ts.reconnectTickCount < minReconnectTickCount)minReconnectTickCount = ts.reconnectTickCount;
			if(ts.reconnectTickCount > maxReconnectTickCount)maxReconnectTickCount = ts.reconnectTickCount;
			if(ts.sendCGConnectTickCount < minCGTickCount)minCGTickCount = ts.sendCGConnectTickCount;
			if(ts.sendCGConnectTickCount > maxCGTickCount)maxCGTickCount = ts.sendCGConnectTickCount;

			reconnectSum += ts.reconnectTickCount;
			CGSum += ts.sendCGConnectTickCount;

		}

		fprintf(fp, "reconnect TickCount Min : %d Max : %d Avr : %d\n", minReconnectTickCount, maxReconnectTickCount, reconnectSum/size);
		fprintf(fp, "send cgConnect TickCount Min : %d Max : %d Avr : %d\n", minCGTickCount, maxCGTickCount, CGSum/size);
		fclose(fp);
	}
#endif
	__END_CATCH
}
