//////////////////////////////////////////////////////////////////////
//
// Filename    : GCShopMarketConditionHandler.cpp

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCShopMarketCondition.h"
#include "ClientDef.h"
#include "MNPC.h"
#include "MShopShelf.h"
#include "MShop.h"
#include "MPriceManager.h"
#include "UIFunction.h"

void GCShopMarketConditionHandler::execute ( GCShopMarketCondition * pPacket , Player * pPlayer )
	 

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

				
				//pNPC->CreateFixedShelf();
			}

			//------------------------------------------------------
			
			//------------------------------------------------------
			g_pPriceManager->SetMarketCondBuy( pPacket->getMarketCondBuy() );
			g_pPriceManager->SetMarketCondSell( pPacket->getMarketCondSell() );
			
			//------------------------------------------------------
			
			//------------------------------------------------------
			UI_OpenInventoryToSell();

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
