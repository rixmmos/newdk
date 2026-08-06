//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopBuyOKHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopBuyOK.h"
#include "ClientDef.h"
#include "MShop.h"
#include "MShopShelf.h"
#include "MInventory.h"
#include "TempInformation.h"
#include "MMoneyManager.h"
#include "MPriceManager.h"
#include "MItemFinder.h"
#include "UIFunction.h"

#define SAFE_DELETE(x)		{ if(x!=NULL) delete x; x=NULL; }


void	CheckItemForSkillIcon(const MItem* pItem);

void GCShopBuyOKHandler::execute ( GCShopBuyOK * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	//--------------------------------------------------------------
	
	//--------------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_SHOP_BUY)
	{
		MShop* pShop = (MShop*)g_pTempInformation->pValue;		
		int ShelfType	= g_pTempInformation->Value1;
		int index		= g_pTempInformation->Value2;
		int x			= g_pTempInformation->Value3;
		int y			= g_pTempInformation->Value4;

		DEBUG_ADD_FORMAT("[BuyOK] TempInfo. ShelfType=%d, index=%d, xy=(%d, %d)", ShelfType, index, x, y);
		
		
		MShopShelf* pShopShelf = pShop->GetShelf( ShelfType );

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		if (pShopShelf!=NULL)
		{
			//------------------------------------------------------
			
			//------------------------------------------------------
			if (ShelfType==SHOP_RACK_SPECIAL)
			{
				pShopShelf->DeleteItem( index );
			}

			
			pShopShelf->SetVersion( pPacket->getShopVersion() );
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] No Shelf.. type=%d", ShelfType);
		}

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------		
		MItem* pItem = MItem::NewItem( (ITEM_CLASS)pPacket->getItemClass() );

		pItem->SetID( pPacket->getItemObjectID() );
		pItem->SetItemType( pPacket->getItemType() );
		pItem->SetItemOptionList( pPacket->getOptionType() );
		pItem->SetNumber( pPacket->getItemNum() );	
		pItem->SetCurrentDurability( pPacket->getDurability() );
		pItem->SetSilver( pPacket->getSilver() );
		pItem->SetGrade( pPacket->getGrade() );
		pItem->SetEnchantLevel( pPacket->getEnchantLevel() );

		const MItem* pOldItem = g_pInventory->GetItem( x, y );

		//--------------------------------------------------------------		
		
		//--------------------------------------------------------------				
		if (pOldItem==NULL)
		{				
		}
		//--------------------------------------------------------------		
		
		//--------------------------------------------------------------				
		else
		{
			if (pItem->IsInsertToItem( pOldItem ))
			{
				int total = //pOldItem->GetNumber() + 
							pItem->GetNumber();
				
				//------------------------------------------------
				
				//------------------------------------------------
				if ( total > pItem->GetMaxNumber() )
				{
					DEBUG_ADD_FORMAT("[Error] Cannot Add. MaxNum exceed=%d", total);
				}
				//------------------------------------------------
				
				//------------------------------------------------
				else
				{
					//pItem->SetNumber( total );

					
					MItem* pRemovedItem = g_pInventory->RemoveItem( x, y );
					
					SAFE_DELETE( pRemovedItem );

//					if (pRemovedItem!=NULL)
//					{
//						delete pRemovedItem;
//					}
				}
			}
			else
			{
				DEBUG_ADD("[Error] Cannot Add. No match item");
			}
		}

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		if (g_pInventory->AddItem( pItem, x, y ))
		{
			
			PlaySound( pItem->GetTileSoundID() );

			
			CheckItemForSkillIcon( pItem );
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] Cannot Add to Inventory(%d,%d)", x,y);
			
			
			delete pItem;
		}
	
		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		switch (pShop->GetShopType())
		{
			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			case MShop::SHOP_NORMAL :
				//--------------------------------------------------------------
				
				//--------------------------------------------------------------
				if (!g_pMoneyManager->SetMoney( pPacket->getPrice() ))
				{
					DEBUG_ADD_FORMAT("[Error] Can't Set Money=%d, Price=%d", g_pMoneyManager->GetMoney(), pPacket->getPrice());
				}
			break;

			//--------------------------------------------------------------
			
			//--------------------------------------------------------------
			case MShop::SHOP_EVENT_STAR :
				
				if (pItem!=NULL)
				{
					STAR_ITEM_PRICE starPrice;

					g_pPriceManager->GetItemPrice( pItem, starPrice );

					int remainNum = starPrice.number;

					if (starPrice.type!=-1 && starPrice.number!=0)
					{
						
						MItemClassTypeFinder starFinder(ITEM_CLASS_EVENT_STAR, starPrice.type);

						
						while (remainNum > 0)
						{
							MItem* pStarItem = g_pInventory->FindItemGridOrder( starFinder );

							
							if (pStarItem==NULL)
							{
								DEBUG_ADD("[Error] Not Enough Star -_-");
								break;
							}

							int itemNum = pStarItem->GetNumber();

							if (itemNum > remainNum)
							{
								pStarItem->SetNumber( itemNum - remainNum );
								remainNum = 0;
							}
							else
							{
								
								remainNum -= itemNum;

								MItem* pRemovedItem = g_pInventory->RemoveItem( 
																pStarItem->GetGridX(), 
																pStarItem->GetGridY() );

								if (pRemovedItem!=NULL)
								{
									delete pRemovedItem;
								}
								else
								{
									DEBUG_ADD_FORMAT("[Error] Can't remove star(%d, %d)", pStarItem->GetGridX(), pStarItem->GetGridY());
								}
							}
						}
					}
				}
			break;
		}

		
		
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);

		
		UI_UnlockItemTrade();

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
//		__BEGIN_HELP_EVENT
//			if (pItem->GetItemClass()==ITEM_CLASS_KEY 
//				&& pItem->GetItemType()==2)
//			{

////				ExecuteHelpEvent( HE_ITEM_BUY_MOTORCYCLE );
//			}
//		__END_HELP_EVENT
	}
	else
	{
		DEBUG_ADD("[Error] Buy? -_-; No Temp Information!");
	}

#endif

	__END_CATCH
}
