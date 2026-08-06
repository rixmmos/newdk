//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCannotUseHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCannotUse.h"
#include "ClientDef.h"
#include "MSlayerGear.h"
#include "MVampireGear.h"
#include "MOustersGear.h"
#include "MGameStringTable.h"
#include "MItemFinder.h"
#include "UIDialog.h"
#include "TempInformation.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCCannotUseHandler::execute ( GCCannotUse * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	
	if(g_pPlayer->GetWaitVerify() == MPlayer::WAIT_VERIFY_LOVE_CHAIN)
	{
		TYPE_OBJECTID packetObjectID = pPacket->getObjectID();
		ITEM_CLASS		ItemClass;

		switch(g_pPlayer->GetRace() )
		{
		case RACE_SLAYER :
			ItemClass = ITEM_CLASS_COUPLE_RING;
			break;
		case RACE_VAMPIRE :
			ItemClass = ITEM_CLASS_VAMPIRE_COUPLE_RING;
			break;
		default :
			ItemClass = ITEM_CLASS_COUPLE_RING;
			break;
		}

		MPlayerGear* pGear = g_pPlayer->GetGear();
		MItemClassFinder itemFinder( ItemClass );

//		const MItem *pItem = pGear->GetItem(packetObjectID);
		MItem *pItem = pGear->FindItem( itemFinder );
		if(pItem != NULL && pItem->GetID() == packetObjectID)
		{
			g_pPlayer->ClearItemCheckBuffer();			
			g_pPlayer->SetWaitVerifyNULL();
			g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_LOVE_CHAIN );

//			g_pSystemMessage->AddFormat((*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_TRACE].GetString(), (*g_pGameStringTable)[STRING_MESSAGE_COUPLE].GetString());
			g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_CAN_NOT_FIND].GetString());
		}
		return;
	}
	//----------------------------------------------------
	
	//----------------------------------------------------
	MItem* pItem = g_pPlayer->GetItemCheckBuffer();

	if (pItem!=NULL)
	{
		MPlayer::ITEM_CHECK_BUFFER status =	g_pPlayer->GetItemCheckBufferStatus();

		//----------------------------------------------------
		
		//----------------------------------------------------
		if( status == MPlayer::ITEM_CHECK_BUFFER_ITEM_TO_ITEM )
		{			
			g_pPlayer->ClearItemCheckBuffer();
			g_pTempInformation->SetMode( TempInformation::MODE_NULL );
			
			if( pItem->GetItemClass() != ITEM_CLASS_CODE_SHEET )
				UI_PopupMessage( STRING_MESSAGE_FAILED_REMOVE_OPTION );
		} else
		if (status==MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY)
		{
			MItem *pItem = g_pPlayer->GetItemCheckBuffer();
			if(pItem != NULL && pItem->GetItemClass() == ITEM_CLASS_EVENT_TREE &&
				pItem->GetItemType() >= 26 && pItem->GetItemType() <= 28)
				UI_PopupMessage( STRING_MESSAGE_USE_GUILD_MEMBER_ONLY );

			if(pItem->GetItemClass() == ITEM_CLASS_MIXING_ITEM)
			{
				UI_CloseMixingForge();
				UI_PopupMessage(UI_STRING_MESSAGE_FAIL_MERGE_ITEM);				
			}

			if(pItem->GetItemClass() == ITEM_CLASS_DYE_POTION && pItem->GetItemType() == 48)
			{
				
				switch( pPacket->getObjectID() )
				{
				case 1 :
					UI_PopupMessage(STRING_MESSAGE_CANNOT_CHANGE_SEX_BY_WEAR);
					break;
				case 2 :
					UI_PopupMessage(STRING_MESSAGE_CANNOT_CHANGE_SEX_BY_COUPLE);
					break;
				default :
					UI_PopupMessage(STRING_ERROR_ETC_ERROR);
					break;
				}
			}

			
			g_pPlayer->ClearItemCheckBuffer();			
		}
		else if (status==MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT)
		{
			
			g_pPlayer->ClearItemCheckBuffer();

			//----------------------------------------------------
			
			//----------------------------------------------------
			UI_UnlockGear();
		}
		//----------------------------------------------------
		
		//----------------------------------------------------
		else if(status == MPlayer::ITEM_CHECK_BUFFER_USE_FROM_GEAR)
		{
			g_pPlayer->ClearItemCheckBuffer();			

			
			
			
			if( pItem->GetItemClass() == ITEM_CLASS_COUPLE_RING ||
				pItem->GetItemClass() == ITEM_CLASS_VAMPIRE_COUPLE_RING)
			{
				g_pSystemMessage->Add((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_CAN_NOT_FIND].GetString());
//				g_pPlayer->SetWaitVerifyNULL();
//				g_pPlayer->RemoveEffectStatus( EFFECTSTATUS_LOVE_CHAIN );
			}
		}
		
		else if(status == MPlayer::ITEM_CHECK_BUFFER_USE_FROM_GQUEST_INVENTORY)
		{
			g_pPlayer->ClearItemCheckBuffer();
		}
		
		
		else if(status == MPlayer::ITEM_CHECK_BUFFER_DROP_TO_CREATURE)
		{
			g_pPlayer->ClearItemCheckBuffer();
		}
		// 2004, 12, 13, sobeit add end
		else
		{
			DEBUG_ADD_FORMAT("[Error] ItemCheck Buffer is not Use Status: status=%d", (int)status);
		}

	}
	//----------------------------------------------------
	
	//----------------------------------------------------
	else
	{
		DEBUG_ADD("[Error] No Item in CheckBuffer");
	}
	
#endif

	__END_CATCH
}
