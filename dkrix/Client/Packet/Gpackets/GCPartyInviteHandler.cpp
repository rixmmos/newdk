//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPartyInviteHandler.cpp

// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCPartyInvite.h"
#include "CGPartyInvite.h"
#include "../ClientPlayer.h"
#include "MGameStringTable.h"
#include "UIDialog.h"
#include "TempInformation.h"
#include "MParty.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "MHelpDef.h"
extern CMessageArray*		g_pGameMessage;

void GCPartyInviteHandler::execute (GCPartyInvite * pPacket , Player * pPlayer)
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	if (g_pZone==NULL
		|| g_pTempInformation==NULL
		|| g_pUIDialog==NULL
		|| g_pPlayer==NULL
		|| g_pParty==NULL
		|| g_pGameMessage==NULL)
	{
		DEBUG_ADD("GCPartyInviteHandler Failed");
		return;
	}

	int targetID = pPacket->getTargetObjectID();

	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);
			
	
	bool bCancelAll = false;
	bool bMessage = false;
	int cancelMessage = 0;
	bool bJoinedSomeone = false;

	switch (pPacket->getCode())
	{
		//--------------------------------------------------------------------
		//
		
		//
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_REQUEST :
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_PARTY
				&& g_pTempInformation->GetMode()==TempInformation::MODE_PARTY_REQUEST)
			{
				int code;

				//----------------------------------------------------------------------
				
				//----------------------------------------------------------------------
				if (g_pTempInformation->Value1==targetID)
				{
					code = CG_PARTY_INVITE_ACCEPT;

					g_pTempInformation->SetMode(TempInformation::MODE_NULL);

					//-------------------------------------------
					
					//-------------------------------------------
					// UI_RunParty();
					
					UI_ClosePartyCancel();

					bJoinedSomeone = true;
					
					//2004, 5, 6 sobeit add start
					ExecuteHelpEvent(HELP_EVENT_PARTY);
					//2004, 5, 6 sobeit add end
				}
				//----------------------------------------------------------------------
				
				//----------------------------------------------------------------------
				else
				{
					code = CG_PARTY_INVITE_BUSY;
				}

				CGPartyInvite _CGPartyInvite;
				_CGPartyInvite.setTargetObjectID( targetID );
				_CGPartyInvite.setCode( code );				

				pClientPlayer->sendPacket( &_CGPartyInvite );

			}
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			else
			{
				//-------------------------------------------
				
				//-------------------------------------------
				if (g_pPlayer->IsWaitVerify() && g_pPlayer->GetWaitVerify()!=MPlayer::WAIT_VERIFY_PARTY			
					
					|| g_pUIDialog->IsLockInput()		
					|| g_pPlayer->GetCreatureType()==CREATURETYPE_BAT	
					|| g_pPlayer->GetCreatureType()==CREATURETYPE_WOLF	
					|| !g_pParty->IsAcceptInvite()
					)
				{
					
					CGPartyInvite _CGPartyInvite;
					_CGPartyInvite.setTargetObjectID( targetID );
					_CGPartyInvite.setCode( CG_PARTY_INVITE_REJECT );

					pClientPlayer->sendPacket( &_CGPartyInvite );
				}
				else
				{
					UI_RunPartyRequest( targetID );
				}
			}
		break;

		//--------------------------------------------------------------------
		//
		
		//
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_CANCEL :
			if (g_pTempInformation->GetMode() == TempInformation::MODE_PARTY_REQUEST)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}		

			UI_ClosePartyRequest();
			UI_ClosePartyAsk();			
		break;

		//--------------------------------------------------------------------
		//
		
		//
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_ACCEPT :
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_PARTY
				&& g_pTempInformation->GetMode()==TempInformation::MODE_PARTY_REQUEST)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
			
			bJoinedSomeone = true;

			UI_ClosePartyCancel();			

			//2004, 5, 6 sobeit add start
			ExecuteHelpEvent(HELP_EVENT_PARTY);
			//2004, 5, 6 sobeit add end	
		break;

		//--------------------------------------------------------------------
		//
		
		//
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_REJECT :
			bCancelAll = true;
			bMessage = true;
			cancelMessage = STRING_MESSAGE_PARTY_REJECTED;			
		break;

		//--------------------------------------------------------------------
		//
		
		//
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_BUSY :
			bCancelAll = true;
			bMessage = true;
			cancelMessage = STRING_MESSAGE_PARTY_BUSY;
		break;

		//--------------------------------------------------------------------
		//
		
		//
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_ANOTHER_PARTY :
		{
			MCreature* pCreature = g_pZone->GetCreature( pPacket->getTargetObjectID() );
			
			if ( pCreature!=NULL )
			{
				
				if (pCreature->IsPlayerParty())
				{
					bCancelAll = true;
					bMessage = false;
				}
				else
				{
					bCancelAll = true;
					bMessage = true;
					cancelMessage = STRING_MESSAGE_IN_ANOTHER_PARTY;					
				}
			}
		}
		break;

		//--------------------------------------------------------------------
		//
		
		// 
		//--------------------------------------------------------------------
		case GC_PARTY_INVITE_MEMBER_FULL :
			bCancelAll = true;
			bMessage = true;
			cancelMessage = STRING_MESSAGE_PARTY_FULL;
		break;
	}

	//-------------------------------------------
	
	//-------------------------------------------
	if (bJoinedSomeone)
	{
		MCreature* pCreature = g_pZone->GetCreature( g_pTempInformation->PartyInviter );

		if (pCreature!=NULL)
		{
			g_pGameMessage->AddFormat((*g_pGameStringTable)[STRING_MESSAGE_SOMEONE_JOINED_PARTY].GetString(), pCreature->GetName());
		}
	}

	//--------------------------------------------------------------------
	
	//--------------------------------------------------------------------
	if (bCancelAll)
	{
		if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_PARTY
			&& g_pTempInformation->GetMode()==TempInformation::MODE_PARTY_REQUEST)
		{			
			
			g_pPlayer->SetWaitVerifyNULL();
			g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		}

		
		UI_ClosePartyCancel();

		UI_ClosePartyAsk();
		UI_ClosePartyRequest();

		if (bMessage)
		{
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[cancelMessage].GetString());		
		}
	}

#endif

	__END_CATCH
}
