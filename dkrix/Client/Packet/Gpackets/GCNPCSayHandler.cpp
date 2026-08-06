//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNPCSayHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCNPCSay.h"
#include "ClientDef.h"
#include "MChatManager.h"
#include "MNPCScriptTable.h"
#include "TextSystem/TextSanitizer.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCNPCSayHandler::execute ( GCNPCSay * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
		// Debug Message

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
				int scriptID = pPacket->getScriptID();
				int subjectID = pPacket->getSubjectID();

				
				char str[256];
				const char* pString = g_pNPCScriptTable->GetSubject( scriptID, subjectID );

				if (pString!=NULL)
				{
					std::string safeText = TextSystem::NormalizeLatinOrFallback(pString, "......");
					strcpy(str, safeText.c_str());
				
					pCreature->SetChatString( str );
				}
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
