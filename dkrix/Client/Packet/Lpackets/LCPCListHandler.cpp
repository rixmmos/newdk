//----------------------------------------------------------------------
//
// Filename    : LCPCListHandler.cpp
// Written By  : Reiot
// Description : 
//
//----------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "LCPCList.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "MStorage.h"
#include <stdio.h>

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
	#include "UserInformation.h"
	#include "../Cpackets/CLCreatePC.h"
	#include "CLDeletePC.h"
	#include "CLSelectPC.h"
#endif

static void TraceGameEntryFlow(const char* step)
{
	(void)step;
}

static void TracePCListName(int slot, const char* race, const char* name)
{
	char buffer[512];
	char hex[128];
	size_t hexPos = 0;
	const unsigned char* bytes = (const unsigned char*)(name != NULL ? name : "");
	size_t len = strlen((const char*)bytes);
	for (size_t i = 0; i < len && hexPos + 3 < sizeof(hex); ++i)
	{
		hexPos += sprintf(hex + hexPos, "%02X", bytes[i]);
	}
	hex[hexPos] = 0;
	sprintf(buffer, "LCPCList slot=%d race=%s name=%s len=%u hex=%s", slot, race, name != NULL ? name : "", (unsigned int)len, hex);
	TraceGameEntryFlow(buffer);
}

//----------------------------------------------------------------------



//----------------------------------------------------------------------
void LCPCListHandler::execute ( LCPCList * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__
	TraceGameEntryFlow("LCPCListHandler begin");

		 

	////
	 

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	UI_StartCharacterManager();				
			

	for (int i=0; i<SLOT_MAX; i++)
	{
		try {
			PCInfo * pInfo = pPacket->getPCInfo ( i==0? SLOT1 : i==1? SLOT2 : SLOT3 );

			switch ( pInfo->getPCType() ) 
			{	
				case PC_SLAYER :
				{
					PCSlayerInfo * pPCSlayerInfo = dynamic_cast<PCSlayerInfo*>(pInfo);							

					//char johapName[16];

					//------------------------------------------------------------
					
					//------------------------------------------------------------
					//UI_WansungToJohap( pPCSlayerInfo->getName().c_str(), johapName );

					//g_pUserInformation->CharacterW[i] = pPCSlayerInfo->getName().c_str();
					//g_pUserInformation->Character[i] = johapName;
					g_pUserInformation->Character[i] = pPCSlayerInfo->getName().c_str();
					TracePCListName(i, "SLAYER", pPCSlayerInfo->getName().c_str());
					g_pUserInformation->Race[i]	= RACE_SLAYER;
					g_pUserInformation->FaceStyleSlot[i] = pPCSlayerInfo->getHairStyle();

					UI_SetCharacter( i, pPCSlayerInfo );		
				}
				break;

				case PC_VAMPIRE :
				{
					PCVampireInfo * pPCVampireInfo = dynamic_cast<PCVampireInfo*>(pInfo);		
					
					//char johapName[16];

					//------------------------------------------------------------
					
					//------------------------------------------------------------
					//UI_WansungToJohap( pPCVampireInfo->getName().c_str(), johapName );

					//g_pUserInformation->CharacterW[i] = pPCVampireInfo->getName().c_str();
					//g_pUserInformation->Character[i] = johapName;
					g_pUserInformation->Character[i] = pPCVampireInfo->getName().c_str();
					TracePCListName(i, "VAMPIRE", pPCVampireInfo->getName().c_str());
					g_pUserInformation->Race[i]	= RACE_VAMPIRE;
					g_pUserInformation->FaceStyleSlot[i] = 0;//pPCVampireInfo->

					UI_SetCharacter( i, pPCVampireInfo );
				}
				break;

				case PC_OUSTERS :
				{
					PCOustersInfo * pPCOustersInfo = dynamic_cast<PCOustersInfo*>(pInfo);		
					
					//char johapName[16];
					
					//------------------------------------------------------------
					
					//------------------------------------------------------------
					//UI_WansungToJohap( pPCVampireInfo->getName().c_str(), johapName );
					
					//g_pUserInformation->CharacterW[i] = pPCVampireInfo->getName().c_str();
					//g_pUserInformation->Character[i] = johapName;
					g_pUserInformation->Character[i] = pPCOustersInfo->getName().c_str();
					TracePCListName(i, "OUSTERS", pPCOustersInfo->getName().c_str());
					g_pUserInformation->Race[i]	= RACE_OUSTERS;
					g_pUserInformation->FaceStyleSlot[i] = 0;//pPCVampireInfo->
					
					UI_SetCharacter( i, pPCOustersInfo );
				}
				break;
			}
			
		} catch (NoSuchElementException) {			
		}
	}

	//----------------------------------------------
	
	//----------------------------------------------
	if (g_pStorage!=NULL)
	{
		delete g_pStorage;
		g_pStorage = NULL;
	}

			

	
	SetMode( MODE_WAIT_SELECTPC );
	TraceGameEntryFlow("LCPCListHandler set MODE_WAIT_SELECTPC");
	

#endif
		
	__END_CATCH
}
