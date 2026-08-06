//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopBoughtHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopBought.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShop.h"
#include "MShopShelf.h"

void GCShopBoughtHandler::execute ( GCShopBought * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
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
			DEBUG_ADD_FORMAT("[Error] There is no such Creature id=%d", pPacket->getObjectID());
		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else if (pCreature->GetClassType()==MCreature::CLASS_NPC)
		{
			MNPC* pNPC = (MNPC*)pCreature;

			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				//------------------------------------------------------
				
				
				//------------------------------------------------------
			}
			else
			{
				//------------------------------------------------------
				
				//------------------------------------------------------
				MShopShelf* pShopShelf = pShop->GetShelf( pPacket->getShopType() );

				if (pShopShelf==NULL)
				{
					DEBUG_ADD_FORMAT("[Error] There is no Shelf type=%d", (int)pPacket->getShopType());
				}
				else
				{
					
					pShopShelf->SetVersion( pPacket->getShopVersion() );

					
					MItem* pItem = MItem::NewItem( (ITEM_CLASS)pPacket->getItemClass() );

					pItem->SetID( pPacket->getItemObjectID() );
					pItem->SetItemType( pPacket->getItemType() );
					pItem->SetItemOptionList( pPacket->getOptionType() );
					pItem->SetCurrentDurability( pPacket->getDurability() );
					pItem->SetSilver( pPacket->getSilver() );
					pItem->SetGrade( pPacket->getGrade() );
					pItem->SetEnchantLevel( pPacket->getEnchantLevel() );

					
					if (!pShopShelf->SetItem( pPacket->getShopIndex(), pItem ))
					{
						DEBUG_ADD("[Error] Cannot Add Item to ShopShelf");
						
						delete pItem;
					}
				}
			}

		}
		//------------------------------------------------------
		
		//------------------------------------------------------
		else
		{
			DEBUG_ADD_FORMAT("[Error] The Creature is Not NPC. id=%d", pPacket->getObjectID());
		}
	}
	


#endif

	__END_CATCH
}
