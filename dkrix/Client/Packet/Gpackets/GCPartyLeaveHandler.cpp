//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPartyLeaveHandler.cpp

// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Client_PCH.h"
#include "GCPartyLeave.h"
#include "MGameStringTable.h"
#include "MParty.h"
#include "UIDialog.h"
#include "UserInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"

extern CMessageArray*		g_pGameMessage;

void GCPartyLeaveHandler::execute (GCPartyLeave * pPacket , Player * pPlayer)
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	if (g_pZone==NULL
		|| g_pPlayer==NULL
		|| g_pUserInformation==NULL
		|| g_pParty==NULL
		|| g_pGameMessage==NULL)
	{
		DEBUG_ADD("GCPartyLeaveHandler Failed");
		return;
	}

	
	const char* pExpeller = pPacket->getExpeller().c_str();	
	const char* pExpellee = pPacket->getExpellee().c_str(); 

	char str[256];
			
	bool bRemovePartyCheck = false;
	
	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_PARTY_LEAVE)
	{
		g_pPlayer->SetWaitVerifyNULL();
	}

	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	if (pExpeller==NULL || pExpeller[0]==NULL)
	{
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		if (g_pUserInformation->CharacterID==pExpellee)
		{
			g_pParty->UnSetPlayerParty();
			g_pParty->Release();	

			g_pGameMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_REMOVE_PARTY_MYSELF].GetString() );
			//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_REMOVE_PARTY_MYSELF].GetString() );

			UI_CloseParty();
		}
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		else if (g_pParty->RemoveMember( pExpellee ))
		{
			sprintf(str, 
					(*g_pGameStringTable)[STRING_MESSAGE_REMOVE_PARTY_HIMSELF].GetString(), 
					pExpellee);		
			
			//g_pUIDialog->PopupFreeMessageDlg( str );
			g_pGameMessage->Add( str );

			bRemovePartyCheck = true;
		}
	}
	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	else
	{
		//-------------------------------------------------------------------
		
		//-------------------------------------------------------------------
		if (g_pUserInformation->CharacterID==pExpeller)
		{		
			if (g_pParty->RemoveMember( pExpellee ))
			{
				sprintf(str, 
						(*g_pGameStringTable)[STRING_MESSAGE_KICK_PARTY_MEMBER_OK].GetString(), 
						pExpellee);

				//g_pUIDialog->PopupFreeMessageDlg( str );
				g_pGameMessage->Add( str );
			}

			bRemovePartyCheck = true;
		}
		//-------------------------------------------------------------------
		
		//-------------------------------------------------------------------
		else if (g_pUserInformation->CharacterID==pExpellee)
		{
			g_pParty->UnSetPlayerParty();
			g_pParty->Release();	

			sprintf(str, 
						(*g_pGameStringTable)[STRING_MESSAGE_KICKED_FROM_PARTY].GetString(), 
						pExpeller);		
			
			//g_pUIDialog->PopupFreeMessageDlg( str );
			g_pGameMessage->Add( str );

			bRemovePartyCheck = true;
		}
		//-------------------------------------------------------------------
		
		//-------------------------------------------------------------------
		else if (g_pParty->RemoveMember( pExpellee ))
		{
			sprintf(str, 
					(*g_pGameStringTable)[STRING_MESSAGE_KICK_PARTY_MEMBER].GetString(), 
					pExpeller, pExpellee);

			//g_pUIDialog->PopupFreeMessageDlg( str );
			g_pGameMessage->Add( str );
		}
	}

	
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (bRemovePartyCheck && g_pParty->GetSize()==0)
	{		
		UI_CloseParty();

		
		//g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_REMOVE_PARTY].GetString() );		
		g_pGameMessage->Add( (*g_pGameStringTable)[STRING_MESSAGE_REMOVE_PARTY].GetString() );
	}

#endif

	__END_CATCH
}
