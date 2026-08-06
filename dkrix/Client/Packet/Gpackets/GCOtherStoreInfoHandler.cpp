//////////////////////////////////////////////////////////////////////
//
// Filename    : GCOtherStoreInfoHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCOtherStoreInfo.h"
#include "MStorage.h"
#include "MPlayer.h"
#include "../PCItemInfo.h"
#include "DebugInfo.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "VS_UI.h"
#include "UserOption.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCOtherStoreInfoHandler::execute ( GCOtherStoreInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	//__BEGIN_DEBUG_EX
	
#ifdef __GAME_CLIENT__



		MCreature* pCreature = g_pZone->GetCreature(pPacket->getObjectID());
		if(pCreature == NULL)
			return; 
		if(pCreature->IsNPC() == false)
			pCreature->SetPersnalShop(pPacket->getStoreInfo()->isOpen());
		
		if(pCreature->CurPernalShop() == 1 && pCreature->IsNPC() == false)
		{
			if(!pPacket->getStoreInfo()->getSign().empty())
				pCreature->SetPersnalString((char*)pPacket->getStoreInfo()->getSign().c_str(),g_pUserOption->ChattingColor);
			else
				pCreature->SetPersnalString((*g_pGameStringTable)[UI_STRING_MESSAGE_PERSNAL_DEFAULT_MESSGE].GetString(),g_pUserOption->ChattingColor);
		}

		if(g_pStorage2 != NULL && g_pStorage2->GetCuropenid() !=NULL)
		{
			if(false == pPacket->getStoreInfo()->isOpen() && g_pStorage2->GetCuropenid() == pPacket->getObjectID())
			{
				if(gC_vs_ui.IsRunningPersnalShop())
					gC_vs_ui.ClosePersnalShop();
				return;
			}
		}

		if(strlen(pPacket->getStoreInfo()->getSign().c_str())>0 && pCreature->IsNPC() == false)
			pCreature->SetPersnalShopMessage(pPacket->getStoreInfo()->getSign().c_str());

		if(false == pPacket->isRequested()) 
			return;


		
		if (g_pStorage2!=NULL) 
			delete g_pStorage2;

		g_pStorage2 = new MStorage;
		g_pStorage2->Init( 1 ); 
		g_pStorage2->SetCurrent( 0 );
		pPacket->getStoreInfo()->getSign();
		g_pStorage2->SetCuropenid(pPacket->getObjectID());

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
			//		}
				}
			}
		}
		
		gC_vs_ui.setinventory_mode(2);
		gC_vs_ui.RunPersnalShop();
		gC_vs_ui.setinventory_mode(2);
		gC_vs_ui.SetPersnalShop( g_pStorage2 ); 		
			 

#endif

	//__END_DEBUG_EX 
	__END_CATCH
}
