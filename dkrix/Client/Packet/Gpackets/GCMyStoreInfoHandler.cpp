//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMyStoreInfoHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMyStoreInfo.h"
#include "MStorage.h"
#include "MPlayer.h"
#include "../PCItemInfo.h"
#include "DebugInfo.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "VS_UI.h"
#include "UserOption.h"
#include "ClientConfig.h"
//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCMyStoreInfoHandler::execute ( GCMyStoreInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	
		__BEGIN_TRY 

		if(g_pPlayer->IsTraceCreature() == TRUE)
			g_pPlayer->TraceNULL();

		if(g_pPlayer->IsNPC() == false)
			g_pPlayer->SetPersnalShop(pPacket->getStoreInfo()->isOpen());

		if(pPacket->getStoreInfo()->isOpen()) 
		{
			if(!pPacket->getStoreInfo()->getSign().empty()) 
				g_pPlayer->SetPersnalString((char*)pPacket->getStoreInfo()->getSign().c_str(),g_pUserOption->ChattingColor);
			else
				g_pPlayer->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
		}
 
		if(gC_vs_ui.GetPersnalShoptoUI() == 3) 
		{
			g_pPlayer->ClearChatString();
			gC_vs_ui.SetPersnalShoptoUI(0);
			gC_vs_ui.ClosePersnalShop(); 

			if (g_pStorage2!=NULL)
			{
				delete g_pStorage2;
				g_pStorage2 = NULL;
			}
			gC_vs_ui.SetPersnalShop( NULL );
			
			return;
		}
		if(gC_vs_ui.GetPersnalShoptoUI() == 2) 
		{ 
			gC_vs_ui.SetPersnalShoptoUI(0);
			gC_vs_ui.ClosePersnalShop(); 
			return;
		}

 		if(g_pStorage2!=NULL) 
		{
			delete g_pStorage2;
			g_pStorage2 = NULL;
		}
	
		g_pStorage2 = new MStorage;
		g_pStorage2->Init( 1 ); 
		g_pStorage2->SetCurrent( 0 );

		if(g_pPlayer->GetID() != NULL)
			g_pStorage2->SetCuropenid(g_pPlayer->GetID());


			
		MItem * p_olditem = NULL;
		MItem * p_Newitem = NULL;
		  
		if(strlen(pPacket->getStoreInfo()->getSign().c_str())>0)
			g_pPlayer->SetPersnalShopMessage(pPacket->getStoreInfo()->getSign().c_str());
//			set->SetPersnalShopMessage(pPacket->getStoreInfo()->getSign().c_str());
		for (int rack=0; rack<1; rack++)
		{	 
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			//int numitem = pPacket->getStoreInfo().getItems().size();
			int numitem = pPacket->getStoreInfo()->getItems().size();
			for (int index=0; index<numitem; index++)
			{
// 
				StoreItemInfo&	item = pPacket->getStoreInfo()->getStoreItemInfo(index);
				//if(item.isItemExist() == 45)
				if(item.isItemExist() != 0)
				{
					 
					//------------------------------------------------------------
					
					//------------------------------------------------------------
					MItem* pItem = MItem::NewItem( (ITEM_CLASS)item.getItemClass() );

					pItem->SetID( item.getObjectID() );

					pItem->SetItemType( item.getItemType() );
					pItem->SetItemOptionList( item.getOptionType() );
					pItem->SetCurrentDurability( item.getDurability() );
					pItem->SetPersnalPrice(item.getPrice());
					pItem->SetNumber(pPacket->getStoreInfo()->getStoreItemInfo(index).getItemNum());
					
					g_pInventory->SetBegin();
					while (g_pInventory->IsNotEnd())
					{
						MItem * p_item = g_pInventory->Get();
						MItem * p_newitem = g_pInventory->Get();
						
						if(p_item != NULL)
						{
							if(item.getObjectID() == p_item->GetID())
							{
								p_olditem = g_pInventory->GetItem(p_item->GetGridX(),p_item->GetGridY());
								p_Newitem = p_olditem;
								p_Newitem->SetPersnal(true);
								g_pInventory->ReplaceItem(p_Newitem,p_item->GetGridX(),p_item->GetGridY(),p_olditem);
								break;
							}
						}
						g_pInventory->Next();
					}
					//------------------------------------------
					
					//------------------------------------------
					
					//------------------------------------------
					if (pItem->IsGunItem())
					{
						MMagazine* pMagazine = (MMagazine*)MItem::NewItem( (ITEM_CLASS)ITEM_CLASS_MAGAZINE );

						
						pMagazine->SetID( 0 );

						
						for (int j=0; j<(*g_pItemTable)[ITEM_CLASS_MAGAZINE].GetSize(); j++)			
						{
							pMagazine->SetItemType(	j );

							if (pMagazine->IsInsertToItem( pItem ))
							{
								break;
							}
						}

						
						pMagazine->ClearItemOption();
					

						//------------------------------------
						
						//------------------------------------
						MGunItem* pGunItem = (MGunItem*)pItem;
						pGunItem->SetMagazine( pMagazine );
					}		
					//------------------------------------------
					
					//------------------------------------------
					else
					{
						pItem->SetSilver( item.getSilver() );
					}

					pItem->SetGrade( item.getGrade() );
					pItem->SetEnchantLevel( item.getEnchantLevel() );

					//------------------------------------------------------------
					
					//------------------------------------------------------------

					int subNum =item.getListNum();
	
					if (subNum!=0)
					{
						//------------------------------------------------------------
						
						//------------------------------------------------------------
						if (pItem->GetItemClass()==ITEM_CLASS_BELT)
						{
							MBelt* pBelt = (MBelt*)pItem;
							std::list<SubItemInfo*>& listSubItem = item.getSubItems();
							std::list<SubItemInfo*>::const_iterator iItem = listSubItem.begin();

							//------------------------------------------------------------
							
							//------------------------------------------------------------
							while (iItem != listSubItem.end())
							{
								SubItemInfo* pItemInfo = *iItem;

								if (pItemInfo!=NULL)
								{
									//------------------------------------------------------------
									
									//------------------------------------------------------------
									MItem* pSubItem = MItem::NewItem( (ITEM_CLASS)pItemInfo->getItemClass() );

									pSubItem->SetID( pItemInfo->getObjectID() );

									pSubItem->SetItemType( pItemInfo->getItemType() );
									pSubItem->SetNumber( pItemInfo->getItemNum() );

									if (!pBelt->AddItem( pSubItem, pItemInfo->getSlotID() ))
									{
										delete pSubItem;

										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
									}
								}

								iItem++;
							}
						}
						else if (pItem->GetItemClass()==ITEM_CLASS_OUSTERS_ARMSBAND)
						{
							MOustersArmsBand* pBelt = (MOustersArmsBand*)pItem;

							std::list<SubItemInfo*>& listSubItem = item.getSubItems();

							std::list<SubItemInfo*>::const_iterator iItem = listSubItem.begin();

							//------------------------------------------------------------
							
							//------------------------------------------------------------
							while (iItem != listSubItem.end())
							{
								SubItemInfo* pItemInfo = *iItem;

								if (pItemInfo!=NULL)
								{
									//------------------------------------------------------------
									
									//------------------------------------------------------------
									MItem* pSubItem = MItem::NewItem( (ITEM_CLASS)pItemInfo->getItemClass() );

									pSubItem->SetID( pItemInfo->getObjectID() );

									pSubItem->SetItemType( pItemInfo->getItemType() );
									pSubItem->SetNumber( pItemInfo->getItemNum() );

									if (!pBelt->AddItem( pSubItem, pItemInfo->getSlotID() ))
									{
										delete pSubItem;

										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
									}
								}

								iItem++;
							}
						}
						else
						{
							DEBUG_ADD_FORMAT("[Error] This item can't have subitems. itemClass=%d", (int)pItem->GetItemClass());
						}
					}
					 
					//------------------------------------------------------------
					
					//------------------------------------------------------------
					if (!g_pStorage2->SetItem( index, pItem ))
					{
						
						delete pItem;
						
						DEBUG_ADD_FORMAT("[Error] Can't Add Item to Storage. rack=%d, slot=%d", rack, index);
					}

			}
			}
		}
		 
		if(gC_vs_ui.GetPersnalShoptoUI() == 1)
		{
			gC_vs_ui.RunPersnalShop();
			gC_vs_ui.SetPersnalShop( g_pStorage2 ); 		
			return; 
		}
		if(gC_vs_ui.IsRunningPersnalShop()== false && g_pPlayer->CurPernalShop() == false && pPacket->getOpenUI()!=0)
		{
			gC_vs_ui.RunPersnalShop();
			gC_vs_ui.SetPersnalShop( g_pStorage2 ); 		
		}
	__END_CATCH
}

  