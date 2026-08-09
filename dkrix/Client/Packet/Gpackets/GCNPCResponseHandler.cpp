//////////////////////////////////////////////////////////////////////
//
// Filename    : GCNPCResponseHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files

#include "Client_PCH.h"
#include "GCNPCResponse.h"
#include "ClientDef.h"
#include "MStorage.h"
#include "MInventory.h"
#include "MSlayerGear.h"
#include "MVampireGear.h"
#include "MOustersGear.h"
#include "MMoneyManager.h"
#include "MGameStringTable.h"
#include "TempInformation.h"
#include "MSkillManager.h"
#include "UIDialog.h"
#include "SoundDef.h"
#include "MItemFinder.h"
#include "UserInformation.h"
#include "MSkillManager.h"
#include "MEventManager.h"
#include "UIFunction.h"

#ifdef __GAME_CLIENT__
	#include "../ClientPlayer.h"
	#include "CGConnect.h"
	#include "../Cpackets/CGStashList.h"
	#include "VS_UI.h"
	#include "VS_UI_filepath.h"
#endif


void GCNPCResponseHandler::execute ( GCNPCResponse * pPacket , Player * pPlayer )
	 


throw ( ProtocolException , Error )
{
	__BEGIN_TRY

#ifdef __GAME_CLIENT__

	char sz_temp[512];
	//------------------------------------------------------
	
	//------------------------------------------------------
	if ( g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK )
	{
		g_pPlayer->SetWaitVerifyNULL();

		DEBUG_ADD("[Verified] NPC Ask Answer OK");
	}

	int value = pPacket->getParameter();

	switch (pPacket->getCode())
	{		
		//------------------------------------------------------
		//
		//				Quit Dialog
		//
		//------------------------------------------------------
		case NPC_RESPONSE_QUIT_DIALOGUE :
			//if (g_pPlayer->GetWaitVerify()==MPlayer::WAIT_VERIFY_NPC_ASK)
			{
				g_pUIDialog->UnSetLockInputPCTalk();

				g_pPlayer->SetWaitVerifyNULL();

				
				g_pUIDialog->ClosePCTalkDlg();
			}
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------			
		case NPC_RESPONSE_INTERFACE_REPAIR :
			UI_OpenInventoryToRepair();
		break;


		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------			
		case NPC_RESPONSE_INTERFACE_HELICOPTER :
			UI_RunSelectWayPoint();
		break;		

		//------------------------------------------------------			
		//
		
		//
		//------------------------------------------------------			
		case NPC_RESPONSE_INTERFACE_STASHOPEN :
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			if (g_pStorage==NULL)
			{	
				ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

				CGStashList	_CGStashList;

				_CGStashList.setObjectID( g_pPlayer->GetID() );
				
				pClientPlayer->sendPacket( &_CGStashList );
			}
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			else
			{
				UI_RunStorage();
				UI_SetStorage(g_pStorage);
			}
		break;
	
		
		//--------------------------------------------------------------
		//
		//  Repair OK
		//
		//--------------------------------------------------------------
		case NPC_RESPONSE_REPAIR_OK :
			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			if (g_pTempInformation->GetMode() == TempInformation::MODE_SHOP_REPAIR)
			{
				//MItem* pCheckItem = (MItem*)g_pTempInformation->pValue;		
				
				MItem* pModifyItem = (MItem*)g_pTempInformation->pValue;		//NULL;

				BOOL	bInInventory	= g_pTempInformation->Value1;
				BOOL	bInGear			= g_pTempInformation->Value2;
				
				//--------------------------------------------------------------
				
				//--------------------------------------------------------------		
				 

				//--------------------------------------------------------------
				
				//--------------------------------------------------------------
				if (pModifyItem==NULL && !bInInventory && bInGear)
				{
					MPlayerGear* pGear = g_pPlayer->GetGear();

					
					pGear->SetBegin();

					MItem* pLastItem = NULL;

					while (pGear->IsNotEnd())
					{
						MItem* pItem = pGear->Get();

						if (pItem!=NULL)
						{
							pItem->SetCurrentDurability( pItem->GetMaxDurability() );

							pLastItem = pItem;
						}

						pGear->Next();
					}

					
					pGear->CheckItemStatusAll();

					//--------------------------------------------------------------
					
					//--------------------------------------------------------------
					if (pLastItem!=NULL)
					{	
						PlaySound( pLastItem->GetInventorySoundID() );				
					}


					//--------------------------------------------------------------
					
					//--------------------------------------------------------------
					if (!g_pMoneyManager->SetMoney( value ))
					{
						DEBUG_ADD_FORMAT("[Error] Can't Set Money=%d, Price=%d", g_pMoneyManager->GetMoney(), value);
					}

					DEBUG_ADD("RepairAll OK");
				}
				//--------------------------------------------------------------
				
				//--------------------------------------------------------------
				else if (//pCheckItem!=NULL && 
						pModifyItem!=NULL)
				{
					
					if (1)//pCheckItem->GetID()==itemID && pModifyItem->GetID()==itemID)
					{
						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						if (pModifyItem->IsChargeItem())
						{
							pModifyItem->SetNumber( pModifyItem->GetMaxNumber() );

							if (g_pSkillAvailable!=NULL)
							{
								g_pSkillAvailable->SetAvailableSkills();		
							}	
						}
						else
						{
							pModifyItem->SetCurrentDurability( pModifyItem->GetMaxDurability() );
						}

						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						if (bInGear)
						{
							MPlayerGear *pGear = g_pPlayer->GetGear();
							
							pGear->CheckItemStatusAll();				
						}

						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						PlaySound( pModifyItem->GetInventorySoundID() );				

						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						if (!g_pMoneyManager->SetMoney( value ))
						{
							DEBUG_ADD_FORMAT("[Error] Can't Set Money=%d, Price=%d", g_pMoneyManager->GetMoney(), value);
						}
					}
					//else
					//{
					//	DEBUG_ADD_FORMAT("[Error] Different ID. Packet(%d)!=ClientTemp(%d)", itemID, pModifyItem->GetID());
					//}
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] Item is NULL");
				}

				
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				
				UI_UnlockItemTrade();
			}
			else
			{
				DEBUG_ADD("[Error] Repair? -_-; No Temp Information!");
			}
		break;

		//------------------------------------------------------			
		//
		
		//
		//------------------------------------------------------			
		case NPC_RESPONSE_REPAIR_FAIL_ITEM_NOT_EXIST :
		case NPC_RESPONSE_REPAIR_FAIL_ITEM_TYPE :
		case NPC_RESPONSE_REPAIR_FAIL_MONEY :
			
			g_pTempInformation->SetMode(TempInformation::MODE_NULL);

			
			UI_UnlockItemTrade();

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_REPAIR].GetString() );

		break;

		//------------------------------------------------------			
		//
		
		//
		//------------------------------------------------------			
		case NPC_RESPONSE_STASH_SELL_OK :
			if (g_pTempInformation->GetMode() == TempInformation::MODE_STORAGE_BUY_WAIT)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				
				g_pUIDialog->ShowPCTalkDlg();

				
				
				if (g_pStorage!=NULL)
				{
					delete g_pStorage;
					g_pStorage = NULL;
				}

				
				g_pMoneyManager->UseMoney( g_pTempInformation->Value1 );

				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_STORAGE_BUY].GetString());
			}
		break;

		//------------------------------------------------------			
		//
		
		//
		//------------------------------------------------------			
		case NPC_RESPONSE_STASH_SELL_FAIL_MAX :
		case NPC_RESPONSE_STASH_SELL_FAIL_MONEY :
			if (g_pTempInformation->GetMode() == TempInformation::MODE_STORAGE_BUY_WAIT)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				
				g_pUIDialog->ShowPCTalkDlg();

				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_CANNOT_BUY_MORE].GetString());
			}
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_SILVER_COATING_OK :
			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			if (g_pTempInformation->GetMode() == TempInformation::MODE_SHOP_SILVERING)
			{
				//MItem* pCheckItem = (MItem*)g_pTempInformation->pValue;		
				
				MItem* pModifyItem = (MItem*)g_pTempInformation->pValue;		//NULL;

				BOOL	bInInventory	= g_pTempInformation->Value1;
				BOOL	bInGear			= g_pTempInformation->Value2;	

				if (//pCheckItem!=NULL && 
					pModifyItem!=NULL)
				{
					
					if (1)//pCheckItem->GetID()==itemID && pModifyItem->GetID()==itemID)
					{
						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						pModifyItem->SetSilver( pModifyItem->GetSilverMax() );
						const MItem* pWeapon = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );
						if(pModifyItem->IsAffectStatus() && pModifyItem == pWeapon)
						{
							UI_SetCharInfoSilverDamage(pWeapon->GetMinDamage() / 10, pWeapon->GetMaxDamage() / 10);
						}

						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						PlaySound( pModifyItem->GetInventorySoundID() );				

						//--------------------------------------------------------------
						
						//--------------------------------------------------------------
						if (!g_pMoneyManager->SetMoney( value ))
						{
							DEBUG_ADD_FORMAT("[Error] Can't Set Money=%d, Price=%d", g_pMoneyManager->GetMoney(), value);
						}
					}
					//else
					//{
					//	DEBUG_ADD_FORMAT("[Error] Different ID. Packet(%d)!=ClientTemp(%d)", itemID, pModifyItem->GetID());
					//}
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] Item is NULL");
				}

				
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				
				UI_UnlockItemTrade();
			}
			else
			{
				DEBUG_ADD("[Error] Silvering? -_-; No Temp Information!");
			}
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_SILVER_COATING_FAIL_ITEM_NOT_EXIST :
		case NPC_RESPONSE_SILVER_COATING_FAIL_ITEM_TYPE :
		case NPC_RESPONSE_SILVER_COATING_FAIL_MONEY :
			
			g_pTempInformation->SetMode(TempInformation::MODE_NULL);

			
			UI_UnlockItemTrade();

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_SILVERING].GetString() );
		break;

		
		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_INTERFACE_SILVER_COATING :
			UI_OpenInventoryToSilvering();
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_INTERFACE_CREATE_GUILD :
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_INTERFACE_DESTROY_GUILD :
		break;
	
		//------------------------------------------------------
		//
		//				Newbie Item
		//
		//-----------------------------------------------------
		case NPC_RESPONSE_INTERFACE_NEWBIE_ITEM :
			UI_RunTutorialExit();
		break;

		//------------------------------------------------------
		//
		//				Tutorial Computer
		//
		//------------------------------------------------------
		case NPC_RESPONSE_INTERFACE_TUTORIAL_COMPUTER :
			UI_RunComputer();
		break;

		//------------------------------------------------------
		//
		//				Tutorial Briefing
		//
		//------------------------------------------------------
		case NPC_RESPONSE_INTERFACE_TUTORIAL_BRIEFING :
			UI_RunBriefing();
		break;

		//------------------------------------------------------
		//
		//				Tutorial Bookcase
		//
		//------------------------------------------------------
		case NPC_RESPONSE_INTERFACE_TUTORIAL_BOOKCASE :
			UI_RunBookcase();
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_HEAL :
			if( g_pPlayer->IsSlayer() )
				PlaySound( SOUND_ITEM_USE_HPOTION );
			else if (g_pPlayer->IsOusters() )
				PlaySound( SOUND_OUSTERS_PUPA );
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_DONATION_OK :
			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			if (!g_pMoneyManager->SetMoney( value ))
			{
				DEBUG_ADD_FORMAT("[Error] Can't Set Money=%d, Price=%d", g_pMoneyManager->GetMoney(), value);
			}

			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_DONATION_OK].GetString() );
		break;

		//------------------------------------------------------
		//
		
		//
		//------------------------------------------------------
		case NPC_RESPONSE_DONATION_FAIL_MONEY :
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_DONATION_FAIL].GetString() );
		break;

		/////////////////////////////////////////////////////////
		
		/////////////////////////////////////////////////////////
		case NPC_RESPONSE_DECREASE_BALL:
		
		{
			int remainNum = pPacket->getParameter();
				
			
			MItemClassTypeFinder ballFinder(ITEM_CLASS_EVENT_STAR, 6);	
				
			
			while (remainNum > 0)
			{
				MItem* pBallItem = g_pInventory->FindItemGridOrder( ballFinder );
				
				
				if (pBallItem==NULL)
				{
					DEBUG_ADD("[Error] Not Enough Ball -_-");
					break;
				}
				
				int itemNum = pBallItem->GetNumber();
				
				if (itemNum > remainNum)
				{
					pBallItem->SetNumber( itemNum - remainNum );
					remainNum = 0;
				}
				else
				{
					
					remainNum -= itemNum;
					
					MItem* pRemovedItem = g_pInventory->RemoveItem( 
						pBallItem->GetGridX(), 
						pBallItem->GetGridY() );
					
					if (pRemovedItem!=NULL)
					{
						delete pRemovedItem;
					}
					else
					{
						DEBUG_ADD_FORMAT("[Error] Can't remove ball(%d, %d)", pBallItem->GetGridX(), pBallItem->GetGridY());
					}
				}
			}
		}
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_ALREADY_JOIN:			
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_ALREADY_JOIN].GetString(), g_pUserInformation->GuildName.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_QUIT_TIMEOUT:			
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_QUIT_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_CANCEL_TIMEOUT:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_CANCEL_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_LEVEL:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_LEVEL].GetString() );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_MONEY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_MONEY].GetString() );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_FAME:					
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_FAME].GetString(), g_pUserInformation->CharacterID.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_NAME:					
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_NAME].GetString() );
		break;

		case NPC_RESPONSE_TEAM_REGIST_FAIL_DENY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_REGIST_FAIL_DENY].GetString() );
		break;


		case NPC_RESPONSE_TEAM_STARTING_FAIL_ALREADY_JOIN:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_ALREADY_JOIN].GetString() );
		break;

		case NPC_RESPONSE_TEAM_STARTING_FAIL_QUIT_TIMEOUT:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_QUIT_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_TEAM_STARTING_FAIL_CANCEL_TIMEOUT:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_CANCEL_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_TEAM_STARTING_FAIL_LEVEL:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_LEVEL].GetString() );
		break;

		case NPC_RESPONSE_TEAM_STARTING_FAIL_MONEY:				
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_MONEY].GetString(), g_pUserInformation->CharacterID.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_TEAM_STARTING_FAIL_FAME:				
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_FAME].GetString(), g_pUserInformation->CharacterID.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_TEAM_STARTING_FAIL_DENY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TEAM_STARTING_FAIL_DENY].GetString() );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_ALREADY_JOIN:			
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_ALREADY_JOIN].GetString(), g_pUserInformation->GuildName.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_QUIT_TIMEOUT:			
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_QUIT_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_CANCEL_TIMEOUT:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_CANCEL_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_LEVEL:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_LEVEL].GetString() );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_MONEY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_MONEY].GetString() );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_FAME:					
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_FAME].GetString(), g_pUserInformation->CharacterID.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_NAME:					
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_NAME].GetString() );
		break;

		case NPC_RESPONSE_CLAN_REGIST_FAIL_DENY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_REGIST_FAIL_DENY].GetString() );
		break;


		case NPC_RESPONSE_CLAN_STARTING_FAIL_ALREADY_JOIN:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_ALREADY_JOIN].GetString() );
		break;

		case NPC_RESPONSE_CLAN_STARTING_FAIL_QUIT_TIMEOUT:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_QUIT_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_CLAN_STARTING_FAIL_CANCEL_TIMEOUT:		
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_CANCEL_TIMEOUT].GetString() );
		break;

		case NPC_RESPONSE_CLAN_STARTING_FAIL_LEVEL:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_LEVEL].GetString() );
		break;

		case NPC_RESPONSE_CLAN_STARTING_FAIL_MONEY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_MONEY].GetString() );
		break;

		case NPC_RESPONSE_CLAN_STARTING_FAIL_FAME:				
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_FAME].GetString(), g_pUserInformation->CharacterID.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
		break;

		case NPC_RESPONSE_CLAN_STARTING_FAIL_DENY:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLAN_STARTING_FAIL_DENY].GetString() );
		break;


		case NPC_RESPONSE_GUILD_SHOW_REGIST:             
			UI_ShowGuildRegist(pPacket->getParameter());
		break;
	
		case NPC_RESPONSE_GUILD_SHOW_STARTING_JOIN:      
//			UI_ShowGuildStartJoin(pPacket->getParameter());
		break;

		case NPC_RESPONSE_GUILD_SHOW_JOIN:               
//			UI_ShowGuildJoin(pPacket->getParameter());
		break;

		case NPC_RESPONSE_GUILD_SHOW_QUIT:               
//			UI_ShowGuildQuit();
		break;

		case NPC_RESPONSE_GUILD_ERROR:                   
		break;

		case NPC_RESPONSE_TRADE_GIFT_BOX_OK:             
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_GIFT_BOX_OK].GetString() );
		break;

		case NPC_RESPONSE_TRADE_GIFT_BOX_NO_ITEM:        
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_GIFT_BOX_NO_ITEM].GetString() );
		break;

		case NPC_RESPONSE_TRADE_GIFT_BOX_ALREADY_TRADE:  
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_TRADE_GIFT_BOX_ALREADY_TRADE].GetString() );
		break;

		case NPC_RESPONSE_TRADE_GIFT_BOX_ERROR:          
		break;

		case NPC_RESPONSE_REWARD_OK:					
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_REWARD_OK].GetString() );
		break;

		case NPC_RESPONSE_REWARD_FAIL:					
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_REWARD_FAIL].GetString() );
		break;

		case NPC_RESPONSE_NO_EMPTY_SLOT:				
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_NO_EMPTY_SLOT].GetString() );
		break;

		case NPC_RESPONSE_SHOW_TAX_BALANCE :			
			UI_OpenBringFee(pPacket->getParameter());
			break;

		case NPC_RESPONSE_WITHDRAW_TAX_OK :				
			UI_SetTotalFee(pPacket->getParameter());
			if(g_pTempInformation->GetMode() == TempInformation::MODE_WAIT_BRING_FEE)
			{
				
				//g_pMoneyManager->AddMoney(g_pTempInformation->Value1);
				
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_SUCCESS_BRING_FEE].GetString());
			break;

		case NPC_RESPONSE_WITHDRAW_TAX_FAIL :			
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_FAIL_BRING_FEE].GetString());
			
			
			if(g_pTempInformation->GetMode() == TempInformation::MODE_WAIT_BRING_FEE)
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

			break;			
		
		case NPC_RESPONSE_NO_GUILD :					
			if(g_pPlayer->IsSlayer())			
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_NO_TEAM].GetString());
			else
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_NO_CLAN].GetString());
			break;
		
		case NPC_RESPONSE_NOT_GUILD_MASTER:				
			if(g_pPlayer->IsSlayer())			
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_NOT_TEAM_MASTER].GetString());
			else
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_NOT_CLAN_MASTER].GetString());
			break;
		
		case NPC_RESPONSE_HAS_NO_CASTLE:			    
			if(g_pPlayer->IsSlayer())			
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_TEAM_HAS_NO_CASTLE].GetString());
			else
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_CLAN_HAS_NO_CASTLE].GetString());
			break;
		
		case NPC_RESPONSE_NOT_YOUR_CASTLE:		        
			if(g_pPlayer->IsSlayer())			
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_TEAM_NOT_YOUR_CASTLE].GetString());
			else
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_CLAN_NOT_YOUR_CASTLE].GetString());
			break;
		
		case NPC_RESPONSE_NOT_ENOUGH_MONEY :			
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_WAR_NOT_ENOUGH_MONEY].GetString());
			break;
		
		case NPC_RESPONSE_WAR_SCHEDULE_FULL :			
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_WAR_SCHEDULE_FULL].GetString());
			break;
		
		case NPC_RESPONSE_WAR_ALREADY_REGISTERED :        
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_WAR_ALREADY_REGISTERED].GetString());
			break;
		
		case NPC_RESPONSE_WAR_REGISTRATION_OK :			
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_WAR_REGISTRATION_OK].GetString());
			break;
		
		case NPC_RESPONSE_ALREADY_HAS_CASTLE :            
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_ALREADY_HAS_CASTLE].GetString());			
			break;
		
		case NPC_RESPONSE_WAR_UNAVAILABLE :               
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_WAR_UNAVAILABLE].GetString());
			break;

		case NPC_RESPONSE_RACE_WAR_JOIN_FAILED :               
			sprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_RACE_WAR_JOIN_FAILED].GetString(), g_pUserInformation->CharacterID.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
			break;

		case NPC_RESPONSE_RACE_WAR_JOIN_OK :               
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_RACE_WAR_JOIN_OK].GetString());
			break;

		case NPC_RESPONSE_RACE_WAR_GO_FIRST_SERVER :               
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_RACE_WAR_GO_FIRST_SERVER].GetString());
			break;

		case NPC_RESPONSE_GIVE_EVENT_ITEM_FAIL_NOW:      
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_GIVE_EVENT_ITEM_FAIL_NOW].GetString());
		break;
    		
		case NPC_RESPONSE_GIVE_EVENT_ITEM_FAIL:          
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_GIVE_EVENT_ITEM_FAIL].GetString());
		break;

		case NPC_RESPONSE_GIVE_EVENT_ITEM_OK:            
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_GIVE_EVENT_ITEM_OK].GetString());
		break;

		case NPC_RESPONSE_GIVE_PREMIUM_USER_ONLY:        
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_GIVE_PREMIUM_USER_ONLY].GetString());
		break;

		
		case NPC_RESPONSE_WAIT_FOR_MEET_COUPLE:			
			UI_RunInputNameWindow(0);
		break;

		case NPC_RESPONSE_COUPLE_MEET_SUCCESS:			
			UI_CloseInputNameWindow();
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_MEET_SUCCESS].GetString());
			
		break;

		case NPC_RESPONSE_COUPLE_CANNOT_MEET:			
			UI_CloseInputNameWindow();
			if(pPacket->getParameter() != 0 && pPacket->getParameter() < COUPLE_MESSAGE_MAX)
				
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_NOT_EVENT_TERM+pPacket->getParameter()-1].GetString());
			else
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_CANNOT_MEET].GetString());
			
		break;

		case NPC_RESPONSE_MEET_WAIT_TIME_EXPIRED:		
			UI_CloseInputNameWindow();
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_MEET_WAIT_TIME_EXPIRED].GetString());			
		break;

		case NPC_RESPONSE_WAIT_FOR_APART_COUPLE:		
			UI_RunInputNameWindow(1);
		break;

		case NPC_RESPONSE_COUPLE_APART_SUCCESS:			
			UI_CloseInputNameWindow();
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_APART_SUCCESS].GetString());
			g_pSkillAvailable->SetAvailableSkills();			
		break;

		case NPC_RESPONSE_NOT_COUPLE:					
			UI_CloseInputNameWindow();
			if(pPacket->getParameter() != 0 && pPacket->getParameter() < COUPLE_MESSAGE_MAX)
				
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_COUPLE_NOT_EVENT_TERM+pPacket->getParameter()-1].GetString());
			else
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_NOT_COUPLE].GetString());
		break;

		case NPC_RESPONSE_APART_WAIT_TIME_EXPIRED:		
			UI_CloseInputNameWindow();
			g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_MEET_WAIT_TIME_EXPIRED].GetString());
		break;

		case NPC_RESPONSE_APART_COUPLE_FORCE :            
			UI_RunInputNameWindow(2);
			break;
		case NPC_RESPONSE_QUEST :
			switch( (QuestMessage) pPacket->getParameter() )
			{
			case START_SUCCESS :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_START_MONSTER_KILL_QUEST].GetString() );
				break;
			case START_FAIL_QUEST_NUM_EXCEEDED :				
			case START_FAIL_DUPLICATED_QUEST_ID :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_FAIL_ALREADY_START_MONSTER_KILL_QUEST].GetString() );
				break;
			case START_FAIL_PC :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_FAIL_MONSTER_KILL_QUEST_BY_STATUS].GetString() );
				break;
			case COMPLETE_SUCCESS :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_SUCCESS_MONSTER_KILL_QUEST].GetString() );
				//g_pEventManager->RemoveEvent( EVENTID_MONSTER_KILL_QUEST );
				UI_InitQuestStatus();				
				break;
			case COMPLETE_FAIL_NOT_COMPLETE :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_NOT_YET_COMPLETE_MONSTER_KILL_QUEST].GetString() );
				break;
			case COMPLETE_FAIL_NO_INVENTORY_SPACE :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_INVENTORY_FULL_MONSTER_KILL_QUEST].GetString() );
				break;
			case COMPLETE_FAIL_NOT_IN_QUEST :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_NOT_IN_QUEST].GetString() );
				break;
			case COMPLETE_FAIL_TIME_EXPIRED :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_FAIL_QUEST_EXPIRED_TIME].GetString() );
				break;
			case START_FAIL_CANNOT_APPLY_QUEST :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_CANNOT_APPLY_QUEST].GetString() );
				break;
			case CANCEL_SUCCESS :
				//				g_pEventManager->RemoveEvent( EVENTID_MONSTER_KILL_QUEST );				
				UI_InitQuestStatus();
				
				g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_NPC_ASK );
				break;
			case CANCEL_NOT_IN_QUEST :
				g_pPlayer->SetWaitVerify( MPlayer::WAIT_VERIFY_NPC_ASK );
				break;
			case COMPLETE_FAIL_INVALID_NPC :
				
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_FAIL_INVALID_NPC].GetString() );
				break;
			case FAIL_BUG :
				g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[UI_STRING_MESSAGE_FAIL_BUG].GetString() );
				break;
			}			
			break;
		case NPC_RESPONSE_LOTTERY :
			{
				int step = -1;
				if( pPacket->getParameter() >= 0 && pPacket->getParameter() <= 4 )
					step = pPacket->getParameter() + 1;
				
				if(step != -1 )
				{
					UI_RunLottery( step );
				}
			}
			break;
		case NPC_RESPONSE_CANNOT_BUY :			
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CANNOT_TAKE_OUT_ITEM_FROM_SHOP].GetString() );
			break;
		case NPC_RESPONSE_CLEAR_RANK_BONUS_OK :           
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_CLEAR_RANK_BONUS_OK].GetString() );
			break;
		case NPC_RESPONSE_NO_RANK_BONUS :                 
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_NO_RANK_BONUS].GetString() );
			break;
		case NPC_RESPONSE_ALREADY_CLEAR_RANK_BONUS :      
			g_pUIDialog->PopupFreeMessageDlg( (*g_pGameStringTable)[STRING_MESSAGE_ALREADY_CLEAR_RANK_BONUS].GetString() );
			break;
		case NPC_RESPONSE_GNOME_CONTRACT_OK :
			g_pUIDialog->ClosePCTalkDlg();
			UI_PopupMessage( STRING_MESSAGE_CONTRACT_GNOMES_HORN_OK );
			break;
		case NPC_RESPONSE_DOWN_SKILL :
			UI_DownSkill();
			break;
			
		case NPC_RESPONSE_GUILD_REGIST_FAIL_ALREADY_JOIN :		
			wsprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_GUILD_REGIST_FAIL_ALREADY_JOIN].GetString(), g_pUserInformation->GuildName.GetString());
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_QUIT_TIMEOUT :		
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_QUIT_TIMEOUT );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_CANCEL_TIMEOUT :		
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_CANCEL_TIMEOUT );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_LEVEL :				
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_LEVEL );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_MONEY :				
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_MONEY );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_FAME :				
			wsprintf(sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_GUILD_REGIST_FAIL_FAME].GetString(), g_pUserInformation->CharacterID.GetString() );
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_NAME :				
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_NAME );
			break;
		case NPC_RESPONSE_GUILD_REGIST_FAIL_DENY :				
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_DENY );
			break;			
		case NPC_RESPONSE_GUILD_STARTING_FAIL_ALREADY_JOIN :		
			UI_PopupMessage( STRING_MESSAGE_GUILD_STARTING_FAIL_ALREADY_JOIN );
			break;
		case NPC_RESPONSE_GUILD_STARTING_FAIL_QUIT_TIMEOUT :		
			UI_PopupMessage( STRING_MESSAGE_GUILD_STARTING_FAIL_QUIT_TIMEOUT );
			break;
		case NPC_RESPONSE_GUILD_STARTING_FAIL_CANCEL_TIMEOUT :	
			UI_PopupMessage( STRING_MESSAGE_GUILD_STARTING_FAIL_CANCEL_TIMEOUT );
			break;
		case NPC_RESPONSE_GUILD_STARTING_FAIL_LEVEL :				
			UI_PopupMessage( STRING_MESSAGE_GUILD_STARTING_FAIL_LEVEL );
			break;
		case NPC_RESPONSE_GUILD_STARTING_FAIL_MONEY :				
			wsprintf( sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_GUILD_STARTING_FAIL_MONEY].GetString(), g_pUserInformation->CharacterID.GetString() );
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
			break;
		case NPC_RESPONSE_GUILD_STARTING_FAIL_FAME :				
			wsprintf( sz_temp, (*g_pGameStringTable)[STRING_MESSAGE_GUILD_STARTING_FAIL_FAME].GetString(), g_pUserInformation->CharacterID.GetString() );
			g_pUIDialog->PopupFreeMessageDlg( sz_temp );
			break;
		case NPC_RESPONSE_GUILD_STARTING_FAIL_DENY :			
			UI_PopupMessage( STRING_MESSAGE_GUILD_REGIST_FAIL_DENY );
			break;
		case NPC_RESPONSE_TOO_MANY_GUILD_REGISTERED:             
			UI_PopupMessage( STRING_MESSAGE_TOO_MANY_GUILD_REGISTERED );
			break;
		case NPC_RESPONSE_REINFORCE_DENYED:                      
			UI_PopupMessage( STRING_MESSAGE_REINFORCE_DENYED );
			break;
		case NPC_RESPONSE_ALREADY_REINFORCE_ACCEPTED:            
			UI_PopupMessage( STRING_MESSAGE_ALREADY_REINFORCE_ACCEPTED );
			break;
		case NPC_RESPONSE_NO_WAR_REGISTERED:                     
			UI_PopupMessage( STRING_MESSAGE_NO_WAR_REGISTERED );
			break;
		case NPC_RESPONSE_CANNOT_ACCEPT:
			UI_PopupMessage( STRING_MESSAGE_CANNOT_ACCEPT );
			break;
		case NPC_RESPONSE_ACCEPT_OK:
			UI_PopupMessage( STRING_MESSAGE_ACCEPT_OK );
			break;
		case NPC_RESPONSE_CANNOT_DENY:
			UI_PopupMessage( STRING_MESSAGE_CANNOT_DENY );
			break;
		case NPC_RESPONSE_DENY_OK:
			UI_PopupMessage( STRING_MESSAGE_DENY_OK );
			break;
			
		case NPC_RESPONSE_SHOW_TAX_RATIO:						
			UI_RunModifyTax();
			break;
		case NPC_RESPONSE_MODIFY_TAX_RATIO_OK:					
			UI_PopupMessage( UI_STRING_MESSAGE_MODIFY_TAX_OK );
			break;
		case NPC_RESPONSE_MODIFY_TAX_RATIO_FAIL:				
			UI_PopupMessage( UI_STRING_MESSAGE_MODIFY_TAX_FAIL );
			break;
		case NPC_RESPONSE_SWAP_ADVANCEMENT_ITEM:
			UI_Show_Swap_Advancement_Item();
			break;
		case NPC_RESPONSE_NOT_ADVANCED:
			UI_PopupMessage( UI_STRING_MESSAGE_SWAP_ERROR );
			break;
		case NPC_RESPONSE_SHOW_DONATION_DAILOG:                  
		//	value
			UI_Run_Campaign_Help_Unfortunate_Neighbors(value);
			break;
		case NPC_RESPONSE_SHOW_DONATION_COMPLETE_DAILOG:         
			UI_PopupMessage( UI_STRING_MESSAGE_CAMPAIGN_HELP_THANKS );
			break;
			
		case NPC_RESPONSE_SHOW_CONFIRM_GET_EVENT_ITEM_DIALOG:    
			UI_Run_Confirm_GetEventItem(value);
			break;

		case NPC_RESPONSE_SHOW_COMMON_MESSAGE_DIALOG:           
			switch(value)
			{
			case YOU_CAN_GET_EVENT_200412_COMBACK_ITEM:
				gC_vs_ui.RunPopupMessage(TXT_TUTORIAL_COMEBACKEVENT_NOTICE, C_VS_UI_POPUP_MESSAGE::POPUP_WELCOME );
				//UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_COMEBACK_1 );
				break;
			case YOU_CAN_GET_EVENT_200412_COMBACK_PREMIUM_ITEM:
				gC_vs_ui.RunPopupMessage(TXT_TUTORIAL_COMEBACKEVENT_PAY, C_VS_UI_POPUP_MESSAGE::POPUP_WELCOME );
				//UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_COMEBACK_2 );
				break;
			case YOU_CAN_GET_EVENT_200412_COMBACK_RECOMMEND_ITEM:
				gC_vs_ui.RunPopupMessage(TXT_TUTORIAL_COMEBACKEVENT_RECOMMEND, C_VS_UI_POPUP_MESSAGE::POPUP_WELCOME );
				//UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_COMEBACK_3 );
				break;
			case YOU_GET_EVENT_ITEM:
				UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_RECEIVE_OK );
				break;
			case NOT_ENOUGH_INVENTORY_SPACE:
				UI_PopupMessage( UI_STRING_MESSAGE_INVENTORY_FULL_MONSTER_KILL_QUEST );
				break;
			case ALEADY_GET_EVENT_ITEM:
				UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_RECEIVE_ALREADY );
				break;
			case FAIL_GET_EVENT_ITEM:
				UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_RECEIVE_FAIL );
				break;
			case YOU_ARE_NOT_EVENT_USER:
				UI_PopupMessage( UI_STRING_MESSAGE_GET_EVENT_ITEM_NOT_EVENT_USER );
				break;

			}
			break;

	}
#endif

	__END_CATCH

}




























