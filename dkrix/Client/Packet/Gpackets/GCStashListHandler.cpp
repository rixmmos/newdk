////////////////////////////////////////////////////////////////////////////////
// Filename    : GCStashListHandler.cpp

// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCStashList.h"
#include "ClientDef.h"
#include "MStorage.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "UIFunction.h"

void GCStashListHandler::execute ( GCStashList * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	int stashNum = pPacket->getStashNum();

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (stashNum>0)
	{
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (g_pStorage!=NULL)
		{
			delete g_pStorage;
		}

		g_pStorage = new MStorage;

		
		g_pStorage->Init( stashNum ); 

		for (int rack=0; rack<stashNum; rack++)
		{
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			g_pStorage->SetCurrent( rack );

			for (int index=0; index<STASH_INDEX_MAX; index++)
			{
				if (pPacket->isExist(rack, index))
				{
					const STASHITEM&	item = pPacket->getStashItem(rack, index);

					//------------------------------------------------------------
					
					//------------------------------------------------------------
					MItem* pItem = MItem::NewItem( (ITEM_CLASS)item.itemClass );

					pItem->SetID( item.objectID );

					pItem->SetItemType( item.itemType );
					pItem->SetItemOptionList( item.optionType );
					pItem->SetCurrentDurability( item.durability );

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
					
						
						pMagazine->SetNumber( item.num );

						//------------------------------------
						
						//------------------------------------
						MGunItem* pGunItem = (MGunItem*)pItem;
						pGunItem->SetMagazine( pMagazine );
					}		
					//------------------------------------------
					
					//------------------------------------------
					else
					{
						pItem->SetNumber( item.num );
						pItem->SetSilver( item.silver );
					}

					pItem->SetGrade( item.grade );
					pItem->SetEnchantLevel( item.enchantLevel );

					//------------------------------------------------------------
					
					//------------------------------------------------------------
					int subNum = pPacket->getSubItemCount(rack, index);
					if (subNum!=0)
					{
						//------------------------------------------------------------
						
						//------------------------------------------------------------
						if (pItem->GetItemClass()==ITEM_CLASS_BELT)
						{
							MBelt* pBelt = (MBelt*)pItem;

							std::list<SubItemInfo*>& listSubItem = pPacket->getSubItems(rack, index);

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
									//pSubItem->SetItemOption( 0 );
									//pSubItem->SetItemDurability( item.durability );
									pSubItem->SetNumber( pItemInfo->getItemNum() );

									if (!pBelt->AddItem( pSubItem, pItemInfo->getSlotID() ))
									{
										delete pSubItem;

										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
									}
								}

								iItem++;
							}
						} else if (pItem->GetItemClass()==ITEM_CLASS_OUSTERS_ARMSBAND)
						{
							MOustersArmsBand* pBelt = (MOustersArmsBand*)pItem;

							std::list<SubItemInfo*>& listSubItem = pPacket->getSubItems(rack, index);

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
									//pSubItem->SetItemOption( 0 );
									//pSubItem->SetItemDurability( item.durability );
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
					if (!g_pStorage->SetItem( index, pItem ))
					{
						
						delete pItem;

						DEBUG_ADD_FORMAT("[Error] Can't Add Item to Storage. rack=%d, slot=%d", rack, index);
					}
				}
			}
		}
		
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		g_pStorage->GetMoneyManager()->SetMoney( pPacket->getStashGold() );

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		UI_RunStorage();
		UI_SetStorage(g_pStorage);
	}
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	else
	{
		g_pUIDialog->PopupFreeMessageDlg((*g_pGameStringTable)[STRING_MESSAGE_NO_STORAGE].GetString());
	}
		
#endif

	__END_CATCH
}
