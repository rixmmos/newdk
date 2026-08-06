//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMakeItemOKHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMakeItemOK.h"
#include "ClientDef.h"
#include "TempInformation.h"
#include "MInventory.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCMakeItemOKHandler::execute ( GCMakeItemOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_SKILL_MAKE_ITEM)
	{
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		//g_pTempInformation->Value1 = itemClass;
		//g_pTempInformation->Value2 = itemType;

		
		UI_UnlockItem();
	}
	else
	{
		DEBUG_ADD( "[Error] Temp Mode is Not MODE_SKILL_MAKE_ITEM" );
		
		
	}
		

	//------------------------------------------------------------------
	//
	// Change Item Num
	//
	//------------------------------------------------------------------
	int size = pPacket->getChangedItemListNum();

	for (int i=0; i<size; i++)
	{
		int itemID = pPacket->popFrontChangedItemListElement();
		int itemNum = pPacket->popFrontChangedItemNumListElement();

		MItem* pItem = ((MItemManager*)g_pInventory)->GetItem( itemID );

		if (pItem!=NULL)
		{
			pItem->SetNumber( itemNum );
		}
	}

	//------------------------------------------------------------------
	//
	// Add Item To Inventory
	//
	//------------------------------------------------------------------	
	MItem* pItem = MItem::NewItem( (ITEM_CLASS)pPacket->getItemClass() );
	pItem->SetID( pPacket->getObjectID() );
	pItem->SetItemType( pPacket->getItemType() );
	pItem->SetItemOptionList( pPacket->getOptionType() );
	pItem->SetCurrentDurability( pPacket->getDurability() );
	pItem->SetNumber( pPacket->getItemNum() );

	int invenX = pPacket->getX();
	int invenY = pPacket->getY();	

	if (!g_pInventory->AddItem( pItem, invenX, invenY ))
	{
		// -_-;
		delete pItem;
	}	


	//------------------------------------------------------------------
	//
	// ModifyInfo
	//
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	AffectModifyInfo(g_pPlayer, pPacket);

#endif

	__END_CATCH
}
