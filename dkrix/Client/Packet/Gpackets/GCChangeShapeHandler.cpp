//////////////////////////////////////////////////////////////////////
//
// Filename    : GCChangeShapeHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCChangeShape.h"
#include "ClientDef.h"
#include "MItem.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCChangeShapeHandler::execute ( GCChangeShape * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


	//--------------------------------------------------------
	//
	
	//
	//--------------------------------------------------------
	MItem* pItem = NULL;

	pItem = MItem::NewItem( (enum ITEM_CLASS)pPacket->getItemClass() );

	//--------------------------------------------------------
	
	//--------------------------------------------------------
	if (pItem==NULL)
	{
		DEBUG_ADD_FORMAT("[Error] Can't new Item ID=%d, Type=%d", pPacket->getItemClass(), pPacket->getItemType());
		
		return;
	}

	pItem->SetItemType( pPacket->getItemType() );
	pItem->AddItemOption( pPacket->getOptionType() );
	pItem->SetQuestFlag( (pPacket->getFlag() & SHAPE_FLAG_QUEST) ? true : false );


	//------------------------------------------------------
	//
	
	//
	//------------------------------------------------------
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

		//--------------------------------------------------
		
		//--------------------------------------------------
		if (pCreature==NULL)
		{
			// message
			DEBUG_ADD_FORMAT("[Error] Not Exist Creature. ID=%d", pPacket->getObjectID());
		}
		//--------------------------------------------------
		
		
		//--------------------------------------------------
		else if (pCreature->IsWear())
		{
			MCreatureWear* pCreatureWear = (MCreatureWear*)pCreature;

			
			pCreatureWear->SetStop();
			
			#ifdef	OUTPUT_DEBUG				
				if (pCreatureWear->SetAddonItem( pItem ))
				{
					DEBUG_ADD_FORMAT("[OK] Addon Item. ID=%d, Class=%d, Type=%d", pPacket->getObjectID(), pItem->GetItemClass(), pItem->GetItemType());
				}
				else
				{
					DEBUG_ADD_FORMAT("[Error] Can't Addon Item. ID=%d, Class=%d, Type=%d", pPacket->getObjectID(), pItem->GetItemClass(), pItem->GetItemType());
				}				
			#else
				pCreatureWear->SetAddonItem( pItem );
			#endif

			pCreatureWear->SetWeaponSpeed( pPacket->getAttackSpeed() );
		}
			
		//--------------------------------------------------
		
		//--------------------------------------------------
		else 
		{
			DEBUG_ADD_FORMAT("[Error] Creature is Not CreatureWear. ID=%d", pPacket->getObjectID());
		}
	}	

	delete pItem;

#endif

	__END_CATCH
}
