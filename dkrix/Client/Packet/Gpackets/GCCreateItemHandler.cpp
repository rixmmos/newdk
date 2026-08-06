//--------------------------------------------------------------------------------
//
// Filename    : GCCreateItemHandler.cpp
// Written By  : elca
//
//--------------------------------------------------------------------------------

// include files
#include "Client_PCH.h"
#include "GCCreateItem.h"
#include "ClientDef.h"
#include "MInventory.h"
#include "UIFunction.h"
#include "VS_UI_GameCommon.h"
extern int g_C2G;

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void GCCreateItemHandler::execute ( GCCreateItem * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	MItem* pItem;

	
	//---------------------------------------------------------------------
	
	//---------------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferPickupSomeFromInventory())
	{
		MItem* pItem = g_pPlayer->GetItemCheckBuffer();
		g_pPlayer->ClearItemCheckBuffer();		

		if (pItem!=NULL)
		{
			
			
			int numPickup = pItem->GetNumber() - pPacket->getItemNum();

			
			MItem* pRemovedItem = g_pInventory->RemoveItem( pItem->GetGridX(), pItem->GetGridY() );

			if (pRemovedItem==pItem)
			{
				
				pItem->SetNumber( numPickup );

				
				UI_PickUpItem( pItem );
			}
			else
			{
				DEBUG_ADD("[Error] PickupPart item Wrong");
			}
		}
		else
		{
			DEBUG_ADD("[Error] PickupPart item NULL");
		}
	}

	//---------------------------------------------------------------------
	
	//---------------------------------------------------------------------
	pItem = MItem::NewItem( (enum ITEM_CLASS)pPacket->getItemClass() );
	pItem->SetItemType( pPacket->getItemType() );
	pItem->SetItemOptionList( pPacket->getOptionType() );

	pItem->SetID( pPacket->getObjectID() );		

	pItem->SetCurrentDurability( pPacket->getDurability() );

	//pItem->SetNumber( pPacket->getItemNum() );

	pItem->SetGrade( pPacket->getGrade() );
	pItem->SetEnchantLevel( pPacket->getEnchantLevel() );

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
	
		
		pMagazine->SetNumber( pPacket->getItemNum() );

		//------------------------------------
		
		//------------------------------------
		MGunItem* pGunItem = (MGunItem*)pItem;
		pGunItem->SetMagazine( pMagazine );
	}		
	//------------------------------------------
	
	//------------------------------------------
	else
	{
		pItem->SetNumber( pPacket->getItemNum() );
		//pItem->SetSilver( item.silver );
	}

	
	if(pItem->GetItemClass() == ITEM_CLASS_EVENT_TREE && (pItem->GetItemType() == 12
		|| pItem->GetItemType() == 25 || pItem->GetItemType() == 41))
	{
		if(g_pPlayer->GetItemCheckBufferStatus() == MPlayer::ITEM_CHECK_BUFFER_TREE_MERGE)
		{
			for(int y = 0; y < 4; y++)
			{
				for(int x = 0; x < 3; x++)
				{
					MItem *pPartOfTree = g_pInventory->GetItem(x+pPacket->getInvenX(), y+pPacket->getInvenY());
					if(pPartOfTree != NULL && 
						pPartOfTree->GetItemClass() == ITEM_CLASS_EVENT_TREE &&
						( pPartOfTree->GetItemType() == y*3+x ||
						pPartOfTree->GetItemType() == y*3+x + 13 ||
						pPartOfTree->GetItemType() == y*3+x + 29))
					{
						g_pInventory->RemoveItem(x+pPacket->getInvenX(), y+pPacket->getInvenY());
						delete pPartOfTree;
					}
				}
			}
			g_pPlayer->ClearItemCheckBuffer();
		}
		else
		{
			DEBUG_ADD("[GCCreateItemHandler] ItemCheckBuffer != ITEM_CHECK_BUFFER_TREE_MERGE");
		}
	}

	//---------------------------------------------------------------------
	
	//---------------------------------------------------------------------
	if (g_pInventory->AddItem( pItem, pPacket->getInvenX(), pPacket->getInvenY() ))
	{
		if( g_C2G > 0 )
		{
			C_VS_UI_INVENTORY::AutoMove( pPacket->getInvenX(), pPacket->getInvenY() );
			gpC_base->DispatchMessage();
			gpC_base->DispatchMessage();
			gpC_base->DispatchMessage();
			gpC_base->DispatchMessage();
			gpC_base->DispatchMessage();
			gpC_base->DispatchMessage();
			g_C2G--;
		}
		
	}
	else
	{
		DEBUG_ADD_FORMAT("[Error] Cannot Create Item to inventory: id=%d, tp=%d, xy=(%d,%d)", (int)pItem->GetID(), (int)pItem->GetItemType(), (int)pPacket->getInvenX(), (int)pPacket->getInvenY());
		
		delete pItem;
	}


#endif

	__END_CATCH
}
