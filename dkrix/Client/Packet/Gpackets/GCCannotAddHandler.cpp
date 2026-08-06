//////////////////////////////////////////////////////////////////////
//
// Filename    : GCCannotAddHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCCannotAdd.h"
#include "ClientDef.h"
#include "UIFunction.h"
#include "VS_UI.h"
#include "MInventory.h"
#include "../ClientPlayer.h"
#include "packet/Cpackets/CGAddZoneToMouse.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCCannotAddHandler::execute ( GCCannotAdd * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	// Pickup to Inventory
	//-----------------------------------------------------------------
	if (g_pPlayer->IsItemCheckBufferPickupToInventory())
	{
		DEBUG_ADD("Can't Drop Item to Inventory");

		MItem* pItem = g_pPlayer->GetItemCheckBuffer();
		if (pItem != NULL && gpC_mouse_pointer->GetPickUpItem() == NULL)
		{
			CGAddZoneToMouse packet;
			packet.setObjectID( pItem->GetID() );
			packet.setZoneX( pItem->GetX() );
			packet.setZoneY( pItem->GetY() );

			ClientPlayer* pClientPlayer = dynamic_cast<ClientPlayer*>(pPlayer);
			if (pClientPlayer != NULL)
			{
				pClientPlayer->sendPacket( &packet );
				g_pPlayer->SetItemCheckBuffer(pItem, MPlayer::ITEM_CHECK_BUFFER_PICKUP_TO_MOUSE);
				return;
			}
		}
		
		
		
		g_pPlayer->ClearItemCheckBuffer();
	}
	//-----------------------------------------------------------------
	// Pickup to Mouse
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferPickupToMouse())
	{
		DEBUG_ADD("Can't Pickup Item to Mouse");
		
		
		g_pPlayer->ClearItemCheckBuffer();
	}
	//-----------------------------------------------------------------
	// Drop to Zone
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferDropToZone())
	{
		DEBUG_ADD("Can't Drop Item to Zone");
		
		
		MItem* pItem = g_pPlayer->GetItemCheckBuffer();

		UI_PickUpItem( pItem );

		
		g_pPlayer->ClearItemCheckBuffer();
	}
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferPickupSomeFromInventory())
	{
		DEBUG_ADD("Can't PIckupSome From Inventory");

		g_pPlayer->ClearItemCheckBuffer();		
	}
	//-----------------------------------------------------------------
	// Drop to RelicTable
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferDropToRelicTable())
	{
		DEBUG_ADD("Can't Drop to RelicTable");

		g_pPlayer->ClearItemCheckBuffer();		
	}
	//-----------------------------------------------------------------
	// Drop to Creature
	//-----------------------------------------------------------------
	else if (g_pPlayer->IsItemCheckBufferDropToCreature())
	{
		DEBUG_ADD("Can't Drop to Creature");

		g_pPlayer->ClearItemCheckBuffer();		
	}
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	else
	{
		DEBUG_ADD("Else.. what?");

		g_pPlayer->ClearItemCheckBuffer();		
	}


#endif

	__END_CATCH
}
