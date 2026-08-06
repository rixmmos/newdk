//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNPCSayDynamicHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCNPCSayDynamic.h"
#include "ClientDef.h"
#include "MChatManager.h"
#include "TextSystem/TextSanitizer.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCNPCSayDynamicHandler::execute ( GCNPCSayDynamic * pPacket , Player * pPlayer )
	 
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
		//---------------------------------------------------------------
		
		//---------------------------------------------------------------
		MCreature*	pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		if (pCreature==NULL)
		{
			
		}
		else
		{		
			//-------------------------------------------------
			
			//-------------------------------------------------
			if (g_pChatManager->IsAcceptID( pCreature->GetName() ))
			{
				
				char str[256];
				std::string safeText = TextSystem::NormalizeLatinOrFallback(pPacket->getMessage().c_str(), "......");
				strncpy(str, safeText.c_str(), sizeof(str) - 1);
				str[sizeof(str) - 1] = '\0';
				
				pCreature->SetChatString( str );
			}
			//-------------------------------------------------
			
			//-------------------------------------------------
			else
			{
				pCreature->SetChatString( "......" );
			}

			
			//char temp[128];
			//sprintf(temp, "%s> %s", pCreature->GetName(), str);
			//UI_AddChatToHistory( temp );
		}
	}

#endif

	__END_CATCH
}
