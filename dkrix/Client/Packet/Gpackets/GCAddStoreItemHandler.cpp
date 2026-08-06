//////////////////////////////////////////////////////////////////////
//
// Filename    : GCAddStoreItemHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCAddStoreItem.h"
#include "MStorage.h"
#include "MPlayer.h"
#include "../PCItemInfo.h"
#include "DebugInfo.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "VS_UI.h"


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCAddStoreItemHandler::execute ( GCAddStoreItem * pPacket , Player * pPlayer )

throw ( ProtocolException , Error )
{
	__BEGIN_TRY 
		//__BEGIN_DEBUG_EX
		
#ifdef __GAME_CLIENT__
		
		if(g_pStorage2 != NULL && g_pStorage2->GetCuropenid() !=NULL)
		{
			if( pPacket->getOwnerObjectID() == g_pStorage2->GetCuropenid())
			{
				g_pStorage2->SetCurrent( 0 );
				
				int current_storage = g_pStorage2->GetCurrent();
				const int storage_size = g_pStorage2->GetSize();
				const int storage_max = STORAGE_SLOT;
				
				for(int j = 0; j < storage_size; j++)
				{
					g_pStorage2->SetCurrent(0);
					
					for(int i = 0; i < storage_max; i++)
					{
						const MItem *p_slot_item = g_pStorage2->GetItem(i);
						
						
						if(p_slot_item == NULL)
						{
							//------------------------------------------------------------
							
							//------------------------------------------------------------
							MItem* pItem = MItem::NewItem( (ITEM_CLASS)pPacket->getItem().getItemClass() );
							
							pItem->SetID( pPacket->getItem().getObjectID() );
							
							pItem->SetItemType( pPacket->getItem().getItemType() );
							pItem->SetItemOptionList( pPacket->getItem().getOptionType() );
							pItem->SetCurrentDurability( pPacket->getItem().getDurability() );
							pItem->SetPersnalPrice(pPacket->getItem().getPrice());
							
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
								pItem->SetSilver( pPacket->getItem().getSilver() );
							}
							
							pItem->SetGrade( pPacket->getItem().getGrade() );
							pItem->SetEnchantLevel( pPacket->getItem().getEnchantLevel() );
							
							//------------------------------------------------------------
							
							//------------------------------------------------------------
							
							int subNum =pPacket->getItem().getSubItems().size();
							
							if (subNum!=0)
							{
								//------------------------------------------------------------
								
								//------------------------------------------------------------
								if (pItem->GetItemClass()==ITEM_CLASS_BELT)
								{
									MBelt* pBelt = (MBelt*)pItem;
									std::list<SubItemInfo*>& listSubItem = pPacket->getItem().getSubItems();
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
												
												//										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
											}
										}
										
										iItem++;
									}
								}
								else if (pItem->GetItemClass()==ITEM_CLASS_OUSTERS_ARMSBAND)
								{
									MOustersArmsBand* pBelt = (MOustersArmsBand*)pItem;
									
									std::list<SubItemInfo*>& listSubItem = pPacket->getItem().getSubItems();
									
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
												
												//										DEBUG_ADD_FORMAT("[Error] Can't Add Item to Belt. rack=%d, slot=%d, class=%d, belt-slot=%d", rack, index, (int)pSubItem->GetItemClass(), (int)pItemInfo->getSlotID());
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
							
							if (!g_pStorage2->SetItem( i, pItem ))
							{
								delete pItem;
								
								//DEBUG_ADD_FORMAT("[Error] Can't Add Item to Storage. rack=%d, slot=%d", rack, index);
							}
							break;
			}
		}
		current_storage ++;
		if(current_storage >= storage_size)
			current_storage = 0;
	}
	
		}
		
		
}	

#endif

//__END_DEBUG_EX 
__END_CATCH
}
