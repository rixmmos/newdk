//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNPCAskHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCNPCAsk.h"
#include "ClientDef.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "MNPCScriptTable.h"
#include "TempInformation.h"
#include "SystemAvailabilities.h"
#include "TextSystem/TextSanitizer.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCNPCAskHandler::execute ( GCNPCAsk * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	if (g_pPlayer==NULL
		|| g_pZone==NULL
		|| g_pNPCScriptTable==NULL
		|| g_pUIDialog==NULL
		|| g_pPCTalkBox==NULL)
	{
		DEBUG_ADD("[Error] Some Object is NULL");
		return;
	}


	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK)
	{
		g_pPlayer->SetWaitVerifyNULL();
		
		// NPC
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );
		
//		pPacket->

		if (pCreature!=NULL || pPacket->getNPCID() >= 634 && pPacket->getNPCID() <= 637 )
		{		
			unsigned int CreatureType;

			if(pPacket->getNPCID() >= 634 && pPacket->getNPCID() <= 637 )
			{
				CreatureType = pPacket->getNPCID();
			} else
			{
				CreatureType= pCreature->GetCreatureType();
			}
			
			//---------------------------------------------------
			
			//---------------------------------------------------
			
			g_pPCTalkBox->Release();

			DEBUG_ADD("TalkBoxRel");

			//---------------------------------------------------
			// normal
			//---------------------------------------------------
			g_pPCTalkBox->SetType( PCTalkBox::NORMAL );

			
			int scriptID = pPacket->getScriptID();

			//---------------------------------------------------
			
			//---------------------------------------------------
			
			std::string subject = TextSystem::NormalizeNpcSubjectOrFallback(
				scriptID,
				g_pNPCScriptTable->GetSubject(scriptID, 0),
				"What can I help you with?");
			g_pPCTalkBox->SetContent( subject.c_str() );
			g_pPCTalkBox->SetNPCID( pPacket->getObjectID() );
			g_pPCTalkBox->SetCreatureType( CreatureType );
			g_pPCTalkBox->SetScriptID( scriptID );

			DEBUG_ADD("SetScript");

			//---------------------------------------------------
			
			//---------------------------------------------------
			int contentSize = g_pNPCScriptTable->GetContentSize( scriptID );

			g_pPCTalkBox->m_AnswerIDMap.clear();

			int idnum=0;
			
			for (int i=0; i<contentSize; i++)
			{
				
				if( g_pSystemAvailableManager->ScriptFiltering( scriptID, i ) )
				{
					char fallback[32];
					sprintf(fallback, "Option %d", idnum + 1);
					std::string answer = TextSystem::NormalizeNpcContentOrFallback(
						scriptID,
						i,
						g_pNPCScriptTable->GetContent( scriptID, i ),
						fallback);
					g_pPCTalkBox->AddString( answer.c_str() );
					g_pPCTalkBox->m_AnswerIDMap.push_back( i );
					idnum++;
				}				
			}

			DEBUG_ADD("SetContent");
			
			g_pUIDialog->PopupPCTalkDlg();
		}

		//---------------------------------------------------
		
		//---------------------------------------------------
		//POINT point = ConvertPositionMapToScreen(pCreature->GetX(), pCreature->GetY());

		//point.y -= pCreature->GetHeight();

		//g_pUIDialog->PopupPCTalkDlg(point.x, point.y - size*20-100);							
	}
	else
	{
		DEBUG_ADD("[Error] WaitVerifyMode is Not WAIT_VERIFY_NPC_ASK");
	}

	
#endif

	__END_CATCH
}
