//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopListHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopList.h"
#include "ClientDef.h"
#include "MShopShelf.h"
#include "MNPC.h"
#include "MPriceManager.h"
#include "UIFunction.h"

void GCShopListHandler::execute ( GCShopList * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY

	DEBUG_ADD("[GCShopListHandler::execute] run in execute function OK [0].");	
		
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
		MCreature* pCreature = g_pZone->GetCreature( pPacket->getObjectID() );

		//------------------------------------------------------
		
		//------------------------------------------------------
		if (pCreature==NULL)
		{
			DEBUG_ADD("[Error] OK 111");
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else if (pCreature->GetClassType()==MCreature::CLASS_NPC)
		{

			DEBUG_ADD("[GCShopListHandler::execute] OK [1]\n");


			MNPC* pNPC = (MNPC*)pCreature;

			//------------------------------------------------------
			
			//------------------------------------------------------\

			ShopRackType_t shopType = pPacket->getShopType();
			DEBUG_ADD_FORMAT("[GCShopListHandler::execute] OK [1.0]  %d\n", shopType);
			if (shopType >= MShopShelf::MAX_SHELF) {
				DEBUG_ADD("[GCShopListHandler::execute] SHELF_TYPE Wrong!");
			}

			MShopShelf* pShelf = MShopShelf::NewShelf( (MShopShelf::SHELF_TYPE) shopType);


			DEBUG_ADD("[GCShopListHandler::execute] OK [1.1]\n");

			pShelf->SetVersion( pPacket->getShopVersion() );
			pShelf->SetEnable();

			DEBUG_ADD("[GCShopListHandler::execute] OK [2]\n");
			//------------------------------------------------------
			
			//------------------------------------------------------
			for (int i=0; i<SHOP_RACK_INDEX_MAX; i++)
			{

				DEBUG_ADD_FORMAT("[GCShopListHandler::execute] shop item %d befre", i);

				const SHOPLISTITEM& item = pPacket->getShopItem( i );

				DEBUG_ADD_FORMAT("[GCShopListHandler::execute] shop item %d after", i);

				if (item.bExist)
				{
					
					MItem* pItem = MItem::NewItem( (ITEM_CLASS)item.itemClass );

					pItem->SetID( item.objectID );
					pItem->SetItemType( item.itemType );
					pItem->SetItemOptionList( item.optionType );
					pItem->SetCurrentDurability( item.durability );
					pItem->SetSilver( item.silver );
					pItem->SetGrade( item.grade );
					pItem->SetEnchantLevel( item.enchantLevel );

					
					pShelf->SetItem( i, pItem );
				}
			}

			DEBUG_ADD("[GCShopListHandler::execute] OK [3]");

			//------------------------------------------------------
			//
			
			//
			//------------------------------------------------------
			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				
				pShop = new MShop;
				pShop->Init( MShopShelf::MAX_SHELF );

				
				pNPC->SetShop( pShop );

				
				pNPC->CreateFixedShelf();
				pNPC->CreateFixedShelf(true);	// mysterious -_-;
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			pShop->SetShopType( (MShop::SHOP_TYPE)pPacket->getNPCShopType() );

			//------------------------------------------------------
			
			//------------------------------------------------------
			if (pShop->GetShopType()==MShop::SHOP_EVENT_STAR)
			{
				pShop->SetCurrent( 1 );
			}
			else
			{			
				pShop->SetCurrent( 0 );
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			g_pPriceManager->SetMarketCondBuy( pPacket->getMarketCondBuy() );
			g_pPriceManager->SetMarketCondSell( pPacket->getMarketCondSell() );			
			
			//------------------------------------------------------
			
			//------------------------------------------------------
			pShop->SetShelf( pShelf->GetShelfType(), pShelf );

			//------------------------------------------------------
			
			
			//------------------------------------------------------
			UI_SetShop( pShop );		
			UI_RunShop();
			UI_SetShop( pShop );		
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else
		{
			DEBUG_ADD_FORMAT("[Error] The Creature is Not NPC. id=%d", pPacket->getObjectID());
		}
	}
//	__BEGIN_HELP_EVENT
		ExecuteHelpEvent( HELP_EVENT_USE_SHOP );
//	__END_HELP_EVENT
#endif

	__END_CATCH
}
