//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTradeVerifyHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTradeVerify.h"
#include "ClientDef.h"
#include "TempInformation.h"
#include "MTradeManager.h"
#include "MMoneyManager.h"
#include "UIFunction.h"

#ifdef __GAME_CLIENT__

	#include "../ClientPlayer.h"
	#include "../Cpackets/CGTradeAddItem.h"
	#include "../Cpackets/CGTradeFinish.h"
	#include "../Cpackets/CGTradeRemoveItem.h"

#endif

void GCTradeVerifyHandler::execute ( GCTradeVerify * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	//------------------------------------------------------------------------
	
	//------------------------------------------------------------------------
	if (g_pTradeManager==NULL)
	{
		DEBUG_ADD( "[Error] TradeManager is NULL");
		
		return;
	}

	ClientPlayer * pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);

	switch (pPacket->getCode())
	{
		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_ADD_ITEM_OK :
			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_ADD_ITEM)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				MItem* pItem	= (MItem*)g_pTempInformation->pValue;
				
				pItem->SetTrade();

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();
			}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_ADD_ITEM_FAIL :
			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_ADD_ITEM)
			{
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_ADD_ITEM_WHEN_ACCEPT :
			
			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_ADD_ITEM)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				MItem* pItem	= (MItem*)g_pTempInformation->pValue;
				
				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				pItem->SetTrade();

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();				
			}
			else
			{
				DEBUG_ADD( "[Error] TempMode is Wrong..");
			}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_REMOVE_ITEM :
				
		//----------------------------------------------------------------------
		//
		//					mouse --> inventory OK
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_MOUSE_TO_INVENTORY_OK :	
		
		//----------------------------------------------------------------------
		//
		//					inventory --> mouse OK
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_INVENTORY_TO_MOUSE_OK :
		
			//----------------------------------------------------------------------
			// remove item from trade
			//----------------------------------------------------------------------
			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_REMOVE_ITEM)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				MItem* pItem	= (MItem*)g_pTempInformation->pValue;
				
				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				pItem->UnSetTrade();

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();				
			}
			//----------------------------------------------------------------------
			
			//----------------------------------------------------------------------
			else if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_PICKUP_FROM_INVENTORY)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				MItem* pItem	= (MItem*)g_pTempInformation->pValue;
				int gridX		= g_pTempInformation->Value2;
				int gridY		= g_pTempInformation->Value3;
				MItem* pMouseItem = UI_GetMouseItem();
			
				UI_PickUpItem( pItem );
				g_pTradeManager->GetMyInventory()->RemoveItem( gridX, gridY );

				//----------------------------------------------------------------
				
				//----------------------------------------------------------------				
				g_pTradeManager->RefuseMyTrade();
				g_pTradeManager->RefuseOtherTrade();

				//----------------------------------------------------------------
				
				
				//----------------------------------------------------------------
				if (!pItem->IsTrade())
				{
					CGTradeFinish _CGTradeFinish;
					_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

					pClientPlayer->sendPacket( &_CGTradeFinish );

				}			
			}
			//----------------------------------------------------------------
			
			//----------------------------------------------------------------
			else if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_DROP_TO_INVENTORY)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				
				MItem* pItem	= (MItem*)g_pTempInformation->pValue;
				int gridX		= g_pTempInformation->Value2;
				int gridY		= g_pTempInformation->Value3;
				MItem* pMouseItem = UI_GetMouseItem();

				
				//----------------------------------------------------------------
				
				//----------------------------------------------------------------
				if (pMouseItem!=NULL)
				{			
					MItem* pOldItem = NULL;

					//----------------------------------------------------------------
					
					//----------------------------------------------------------------
					if (g_pTradeManager->GetMyInventory()->ReplaceItem(
															pMouseItem,		
															gridX, gridY,	
															pOldItem))								
					{
						if (pOldItem != NULL) 
						{
							UI_PickUpItem( pOldItem );						
						}
						else
						{
							UI_DropItem();
						}
					}

					//---------------------------------------------------
					
					//---------------------------------------------------
					if (pMouseItem->IsTrade())
					{
						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pMouseItem->GetID() );

						pClientPlayer->sendPacket( &_CGTradeAddItem );

					}
				}
				else
				{
					
				}

				//----------------------------------------------------------------
				
				//----------------------------------------------------------------
				g_pTradeManager->RefuseMyTrade();
				g_pTradeManager->RefuseOtherTrade();

				//----------------------------------------------------------------
				
				
				//----------------------------------------------------------------
				if (pMouseItem!=NULL && !pMouseItem->IsTrade())
				{
					CGTradeFinish _CGTradeFinish;
					_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

					pClientPlayer->sendPacket( &_CGTradeFinish );

				}
			}
			//----------------------------------------------------------------
			
			//----------------------------------------------------------------
			else if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_INSERT_TO_INVENTORY)				
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				MItem* pItem	= (MItem*)g_pTempInformation->pValue;
				int gridX		= g_pTempInformation->Value2;
				int gridY		= g_pTempInformation->Value3;
				MItem* pMouseItem = UI_GetMouseItem();

				//----------------------------------------------------
				
				//----------------------------------------------------
				int total = pMouseItem->GetNumber() + pItem->GetNumber();
				if ( total > pItem->GetMaxNumber() )
				{
					
					pMouseItem->SetNumber( total - pItem->GetMaxNumber() );
					pItem->SetNumber( pItem->GetMaxNumber() );
				}
				else
				{
					
					pItem->SetNumber( total );
					UI_DropItem();

					delete pMouseItem;
				}

				//----------------------------------------------------
				
				//----------------------------------------------------
				// Remove and Add - -;
				if (pItem->IsTrade())
				{
						//----------------------------------------------------
						
						//----------------------------------------------------
						CGTradeRemoveItem _CGTradeRemoveItem;
						_CGTradeRemoveItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeRemoveItem.setItemObjectID( pItem->GetID() );

						pClientPlayer->sendPacket( &_CGTradeRemoveItem );

						#ifdef __DEBUG_OUTPUT__
							DEBUG_ADD_FORMAT( "[Send] %s", _CGTradeRemoveItem.toString().c_str() );							
						#endif
						
						//----------------------------------------------------
						
						//----------------------------------------------------
						CGTradeAddItem _CGTradeAddItem;
						_CGTradeAddItem.setTargetObjectID( g_pTradeManager->GetOtherID() );
						_CGTradeAddItem.setItemObjectID( pItem->GetID() );

						pClientPlayer->sendPacket( &_CGTradeAddItem );
						
				}
				else
				{
					//----------------------------------------------------------------
					
					
					//----------------------------------------------------------------					
					CGTradeFinish _CGTradeFinish;
					_CGTradeFinish.setTargetObjectID( g_pTradeManager->GetOtherID() );
					_CGTradeFinish.setCode( CG_TRADE_FINISH_RECONSIDER );

					pClientPlayer->sendPacket( &_CGTradeFinish );

				}

				PlaySound( pItem->GetInventorySoundID() );
			}
			//----------------------------------------------------------------
			
			//----------------------------------------------------------------
			else
			{
				DEBUG_ADD( "[Error] TempMode is Wrong..");
			}
		break;


		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_MONEY_INCREASE :

			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_ADD_MONEY)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				int money		= g_pTempInformation->Value1;
			
				//if (!g_pMoneyManager->GetOtherMoneyManager->CanAddMoney(money))
				//{
				//	money = g_pMoneyManager->GetOtherMoneyManager->GetMaxAddMoney();
				//}

				//g_pMoneyManager->UseMoney( money );
				//g_pTradeManager->GetMyMoneyManager()->AddMoney( money );

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();
			}
			else
			{
				DEBUG_ADD( "[Error] TempMode is Wrong..");
			}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_MONEY_DECREASE:

			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_REMOVE_MONEY)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				int money		= g_pTempInformation->Value1;
				
				//g_pTradeManager->GetMyMoneyManager()->UseMoney( money );
				//g_pMoneyManager->AddMoney( money );	

				//-----------------------------------------------------------
				
				//-----------------------------------------------------------
				g_pTradeManager->RefuseOtherTrade();
				g_pTradeManager->RefuseMyTrade();				
			}
			else
			{
				DEBUG_ADD( "[Error] TempMode is Wrong..");
			}
		break;

		//----------------------------------------------------------------------
		
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_FINISH_ACCEPT:
			
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_FINISH_REJECT:

			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_CLOSE)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				//----------------------------------------------------------------
				
				//----------------------------------------------------------------			
				if (g_pTradeManager!=NULL)
				{
					g_pTradeManager->CancelTrade();
				}

				UI_CloseExchange();
			}
		break;

		//----------------------------------------------------------------------
		//
		
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_FINISH_RECONSIDER:

			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_CANCEL)
			{
				g_pTempInformation->SetMode(TempInformation::MODE_NULL);

				//----------------------------------------------------------------
				
				//----------------------------------------------------------------			
				g_pTradeManager->RefuseMyTrade();
			}
		break;

		//----------------------------------------------------------------------
		//
		//					mouse --> inventory Fail
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_MOUSE_TO_INVENTORY_FAIL:
			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_DROP_TO_INVENTORY)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
			else
			{
				DEBUG_ADD( "[Error] TempMode is Not MODE_TRADE_VERIFY_DROP_TO_INVENTORY");
			}
		break;


		//----------------------------------------------------------------------
		//
		//					inventory --> mouse Fail
		//
		//----------------------------------------------------------------------
		case GC_TRADE_VERIFY_CODE_INVENTORY_TO_MOUSE_FAIL:
			if (g_pTempInformation->GetMode()==TempInformation::MODE_TRADE_VERIFY_PICKUP_FROM_INVENTORY)
			{
				
				g_pPlayer->ClearItemCheckBuffer();

				g_pTempInformation->SetMode(TempInformation::MODE_NULL);
			}
			else
			{
				DEBUG_ADD( "[Error] TempMode is Not MODE_TRADE_VERIFY_PICKUP_FROM_INVENTORY");
			}
		break;
	}


#endif

	__END_CATCH
}
