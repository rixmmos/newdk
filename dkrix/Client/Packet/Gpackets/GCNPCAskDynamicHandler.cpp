//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNPCAskDynamicHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCNPCAskDynamic.h"
#include "ClientDef.h"
#include "TalkBox.h"
#include "UIDialog.h"
#include "SystemAvailabilities.h"
#include "TextSystem/TextSanitizer.h"


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCNPCAskDynamicHandler::execute ( GCNPCAskDynamic * pPacket , Player * pPlayer )
	 
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
		if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK)
		{
			g_pPlayer->SetWaitVerifyNULL();

			int size = pPacket->getContentsCount();

			if (size==0)
			{
				DEBUG_ADD_FORMAT("[Error] No Answer String.. ScriptID=%d", (int)pPacket->getScriptID());			
			}
			else
			{
				// NPC
				MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

				if (pCreature==NULL)
				{
					
				}
				else
				{
					//---------------------------------------------------
					
					//---------------------------------------------------
					
					g_pPCTalkBox->Release();

					//---------------------------------------------------
					// normal
					//---------------------------------------------------
					g_pPCTalkBox->SetType( PCTalkBox::NORMAL );

					//---------------------------------------------------
					
					//---------------------------------------------------
					std::string subject = TextSystem::NormalizeNpcSubjectOrFallback(
						pPacket->getScriptID(),
						pPacket->getSubject().c_str(),
						"What can I help you with?");
					g_pPCTalkBox->SetContent( subject.c_str() );
					g_pPCTalkBox->SetNPCID( pPacket->getObjectID() );
					g_pPCTalkBox->SetCreatureType( pCreature->GetCreatureType() );
					g_pPCTalkBox->SetScriptID( pPacket->getScriptID() );

					//---------------------------------------------------
					
					//---------------------------------------------------

					g_pPCTalkBox->m_AnswerIDMap.clear();
					int visibleAnswer = 0;
					for (int i=0; i<size; i++)
					{
						std::string rawContent = pPacket->popContent();

						
						if( g_pSystemAvailableManager->ScriptFiltering( pPacket->getScriptID(), i ) )
						{
							char fallback[32];
							sprintf(fallback, "Option %d", visibleAnswer + 1);
							std::string content = TextSystem::NormalizeNpcContentOrFallback(
								pPacket->getScriptID(),
								i,
								rawContent.c_str(),
								fallback);
							g_pPCTalkBox->AddString( content.c_str() );
							g_pPCTalkBox->m_AnswerIDMap.push_back( i );
							visibleAnswer++;
						}
					}
					
					g_pUIDialog->PopupPCTalkDlg();
				}

				//---------------------------------------------------
				
				//---------------------------------------------------
				//POINT point = ConvertPositionMapToScreen(pCreature->GetX(), pCreature->GetY());

				//point.y -= pCreature->GetHeight();

				//g_pUIDialog->PopupPCTalkDlg(point.x, point.y - size*20-100);				
			}
		}
		else
		{
			DEBUG_ADD("[Error] WaitVerifyMode is Not WAIT_VERIFY_NPC_ASK");
		}
	}

#endif

	__END_CATCH
}
