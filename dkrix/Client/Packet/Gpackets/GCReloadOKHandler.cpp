//////////////////////////////////////////////////////////////////////
//
// Filename    : GCReloadOKHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCReloadOK.h"
#include "ClientDef.h"
#include "MSlayerGear.h"
#include "MInventory.h"
#include "MQuickSlot.h"
#include "UIFunction.h"

#define SAFE_DELETE(x)		{ if(x!=NULL) delete x; x=NULL; }

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCReloadOKHandler::execute ( GCReloadOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	// message

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if (g_pPlayer->IsVampire())
	{
		DEBUG_ADD("[Error] Player is Vampire. No Effect!");

		return;
	}

	

	//------------------------------------------------------------------
	//
	
	//
	//------------------------------------------------------------------	
	MItem* pItem = g_pPlayer->GetItemCheckBuffer();

	//----------------------------------------------------
	
	//----------------------------------------------------
	if (pItem!=NULL)
	{
		PlaySound( pItem->GetUseSoundID() );

		MPlayer::ITEM_CHECK_BUFFER status =	g_pPlayer->GetItemCheckBufferStatus();

		//----------------------------------------------------
		
		//----------------------------------------------------
		if (status==MPlayer::ITEM_CHECK_BUFFER_USE_FROM_INVENTORY)			
		{
			
			g_pPlayer->ClearItemCheckBuffer();

			//----------------------------------------------------
			// MAGAZINE
			//----------------------------------------------------
			if (pItem->GetItemClass()==ITEM_CLASS_MAGAZINE)
			{
				MMagazine* pMagazineItem;

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pItem->GetNumber()>1)
				{
					
					
					pItem->SetNumber( pItem->GetNumber() - 1 );

					//-------------------------------------------------
					
					//-------------------------------------------------
					pMagazineItem = (MMagazine*)MItem::NewItem( ITEM_CLASS_MAGAZINE );

					pMagazineItem->SetID( 0 );	
					pMagazineItem->SetItemType( pItem->GetItemType() );
					pMagazineItem->SetItemOptionList( pItem->GetItemOptionList() );
				}
				//-------------------------------------------------
				
				//-------------------------------------------------
				else
				{
					
					g_pInventory->RemoveItem( pItem->GetID() );

					
					//delete pItem;	
					
					pMagazineItem = (MMagazine*)pItem;
				}

				
				//----------------------------------------------------
				
				//----------------------------------------------------
				// Magazine.GetNumber()
				//
				
				
				//----------------------------------------------------
				pMagazineItem->SetNumber( pPacket->getBulletNum() );

				
				const MItem* pRightHandItem = g_pSlayerGear->GetItem( (MSlayerGear::GEAR_SLAYER)MSlayerGear::GEAR_SLAYER_RIGHTHAND );

				//----------------------------------------------------
				
				//----------------------------------------------------
				if (pRightHandItem!=NULL)
				{
					if (pRightHandItem->IsGunItem())
					{
						MGunItem* pGunItem = (MGunItem*)pRightHandItem;

						
						MMagazine* pOldMagazine = pGunItem->SetMagazine( pMagazineItem );

						
						g_pCurrentMagazine = pMagazineItem;

						
						if (pOldMagazine!=NULL)
						{
							DEBUG_ADD("Delete Old Magazine");
							
							delete pOldMagazine;
						}
					}									
				}
				//----------------------------------------------------
				
				//----------------------------------------------------
				else
				{
					DEBUG_ADD_FORMAT("[Error] Player not Use GUN. the ItemClass=%d", (int)pRightHandItem->GetItemClass());
					
					
					g_pInventory->RemoveItem( pItem->GetID() );

					
					UI_RemoveDescriptor( (void*)pItem );

					delete pItem;
				}
			}
			//----------------------------------------------------
			
			//----------------------------------------------------
			else
			{
				DEBUG_ADD_FORMAT("[Error] Item is Not Magazine: the ItemClass=%d", (int)pItem->GetItemClass());
			}
		}
		//----------------------------------------------------
		
		//----------------------------------------------------
		else if (status==MPlayer::ITEM_CHECK_BUFFER_USE_FROM_QUICKSLOT)
		{
			
			g_pPlayer->ClearItemCheckBuffer();

			//----------------------------------------------------
			// MAGAZINE
			//----------------------------------------------------
			if (pItem->GetItemClass()==ITEM_CLASS_MAGAZINE)
			{
				MMagazine* pMagazineItem;

				//-------------------------------------------------
				
				//-------------------------------------------------
				if (pItem->GetNumber()>1)
				{
					
					
					pItem->SetNumber( pItem->GetNumber() - 1 );

					//-------------------------------------------------
					
					//-------------------------------------------------
					pMagazineItem = (MMagazine*)MItem::NewItem( ITEM_CLASS_MAGAZINE );

					pMagazineItem->SetID( 0 );	
					pMagazineItem->SetItemType( pItem->GetItemType() );
					pMagazineItem->SetItemOptionList( pItem->GetItemOptionList() );					
				}
				//-------------------------------------------------
				
				//-------------------------------------------------
				else
				{
					
					g_pQuickSlot->RemoveItem( pItem->GetID() );

					
					//delete pItem;	
					
					pMagazineItem = (MMagazine*)pItem;
				}

				//----------------------------------------------------
				
				//----------------------------------------------------
				pMagazineItem->SetNumber( pPacket->getBulletNum() );

				
				const MItem* pRightHandItem = g_pSlayerGear->GetItem( (MSlayerGear::GEAR_SLAYER)MSlayerGear::GEAR_SLAYER_RIGHTHAND );

				//----------------------------------------------------
				
				//----------------------------------------------------
				if (pRightHandItem!=NULL)
				{
					if (pRightHandItem->IsGunItem())
					{
						MGunItem* pGunItem = (MGunItem*)pRightHandItem;

						
						MMagazine* pOldMagazine = pGunItem->SetMagazine( pMagazineItem );

						
						g_pCurrentMagazine = pMagazineItem;

						
						if (pOldMagazine!=NULL)
						{
							DEBUG_ADD("Delete Old Magazine");
							
							delete pOldMagazine;
						}
					}									
				}
				//----------------------------------------------------
				
				//----------------------------------------------------
				else
				{
					DEBUG_ADD_FORMAT("[Error] Player not Use GUN. the ItemClass=%d", (int)pRightHandItem->GetItemClass());
					
					
					g_pQuickSlot->RemoveItem( pItem->GetID() );

					SAFE_DELETE( pItem );					
				}
			}
			//----------------------------------------------------
			
			//----------------------------------------------------
			else
			{
				DEBUG_ADD_FORMAT("[Error] Item is Not Magazine: the ItemClass=%d", (int)pItem->GetItemClass());
			}
		}
		//----------------------------------------------------
		
		//----------------------------------------------------
		else
		{
			DEBUG_ADD_FORMAT("[Error] ItemCheck Buffer is not Use Status: status=%d", (int)status);
			
			return;
		}

		
		g_pPlayer->CalculateStatus();

	}
	//----------------------------------------------------
	
	//----------------------------------------------------
	else
	{
		DEBUG_ADD("[Error] No Item in CheckBuffer");
		
		return;
	}


#endif

	__END_CATCH
}
