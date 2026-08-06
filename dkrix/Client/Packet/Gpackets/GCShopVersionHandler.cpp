//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopVersionHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopVersion.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShopShelf.h"
#include "ServerInfo.h"
#include "UIFunction.h"
#include "MPriceManager.h"
void GCShopVersionHandler::execute ( GCShopVersion * pPacket , Player * pPlayer )
	 
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

			//------------------------------------------------------
			
			//------------------------------------------------------
			MShop* pShop = pNPC->GetShop();

			if (pShop==NULL)
			{
				
				pShop = new MShop;
				pShop->Init( MShopShelf::MAX_SHELF );
				
				
				pNPC->SetShop( pShop );				
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			pShop->SetCurrent( 0 );

			//------------------------------------------------------
			
			//------------------------------------------------------
			pNPC->CreateFixedShelf();
			pNPC->CreateFixedShelf(true);	// mysterious -_-;


			//------------------------------------------------------
			
			//------------------------------------------------------
			BOOL bSameAll = TRUE;
			for (ShopRackType_t i=0; i<SHOP_RACK_TYPE_MAX; i++)
			{
				//------------------------------------------------------
				
				
				//------------------------------------------------------
				if (i!=SHOP_RACK_SPECIAL)
				{
					continue;
				}

				MShopShelf* pShopShelf = pShop->GetShelf( i );

				//------------------------------------------------------
				
				//------------------------------------------------------
				if (pShopShelf==NULL)
				{
					pShopShelf = MShopShelf::NewShelf( (MShopShelf::SHELF_TYPE)i );

					pShop->SetShelf( (MShopShelf::SHELF_TYPE)i, pShopShelf );
				}

				unsigned int serverVersion = pPacket->getVersion( i );
				unsigned int clientVersion = pShopShelf->GetVersion();

				//------------------------------------------------------
				
				//------------------------------------------------------
				if (serverVersion!=clientVersion)
				{
					
					bSameAll = FALSE;

						
						CGShopRequestList	_CGShopRequestList;
						_CGShopRequestList.setObjectID( pNPC->GetID() );
						_CGShopRequestList.setRackType( i );

						g_pSocket->sendPacket( &_CGShopRequestList );						
				}
			}

			
			g_pPriceManager->SetMarketCondSell( pPacket->getMarketCondSell() );
			// 2004, 10, 25, sobeit add end
			
			//------------------------------------------------------
			
			
			//------------------------------------------------------
			if (bSameAll)
			{
				//------------------------------------------------------
				
				
				//------------------------------------------------------
				UI_RunShop();
				UI_SetShop( pShop );		
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
