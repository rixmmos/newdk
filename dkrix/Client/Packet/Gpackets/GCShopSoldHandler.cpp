//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopSoldHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopSold.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShop.h"
#include "MShopShelf.h"
#include "UIFunction.h"


void GCShopSoldHandler::execute ( GCShopSold * pPacket , Player * pPlayer )
	 


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

		if (pCreature==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] There is no such Creature. id=%d", pPacket->getObjectID());
		}
		else if (pCreature->GetClassType()==MCreature::CLASS_NPC)
		{
			MNPC* pNPC = (MNPC*)pCreature;

			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				DEBUG_ADD_FORMAT("[Error] the Creature has NO Shop. creatureID=%d", pPacket->getObjectID());
			}
			else
			{
				MShopShelf* pShopShelf = pShop->GetShelf( (MShopShelf::SHELF_TYPE)pPacket->getShopType() );

				if (pShopShelf==NULL)
				{
					DEBUG_ADD_FORMAT("[Error] There is no such Shelf. type=%d", (int)pPacket->getShopType());
				}
				else
				{
					//------------------------------------------------------
					
					//------------------------------------------------------
					if (pPacket->getShopType()==SHOP_RACK_SPECIAL)
					{
						MItem* pRemovedItem = pShopShelf->RemoveItem( pPacket->getShopIndex() );

						if (pRemovedItem!=NULL)
						{
							
							UI_RemoveDescriptor( (void*)pRemovedItem );

							
							delete pRemovedItem;
						}
					}

					
					pShopShelf->SetVersion( pPacket->getShopVersion() );									
				}
			}
		}
		else
		{
			DEBUG_ADD_FORMAT("[Error] the Creature is not NPC. id=%d", pPacket->getObjectID());
		}
	}

#endif

	__END_CATCH
}
