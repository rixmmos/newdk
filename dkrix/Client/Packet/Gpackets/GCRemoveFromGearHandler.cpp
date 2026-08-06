//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRemoveFromGearHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCRemoveFromGear.h"
#include "ClientDef.h"
#include "MSlayerGear.h"
#include "MVampireGear.h"
#include "MOustersGear.h"
#include "MGameStringTable.h"
#include "MItemOptionTable.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCRemoveFromGearHandler::execute ( GCRemoveFromGear * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__
	

	int slotID = pPacket->getSlotID();

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	switch(g_pPlayer->GetRace())
	{
	case RACE_SLAYER:
	{
		MItem* pRemovedItem = g_pSlayerGear->RemoveItem( (MSlayerGear::GEAR_SLAYER)slotID );

		
		if (pRemovedItem==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] No Removed Item in Slot=%d", slotID);
		}
		else
		{
			//----------------------------------------------------------
			
			//----------------------------------------------------------

			if(pRemovedItem->GetItemClass() != ITEM_CLASS_COUPLE_RING && pRemovedItem->GetItemClass() != ITEM_CLASS_VAMPIRE_COUPLE_RING)
			{
				char str[128]; 
				
				if (pRemovedItem->IsEmptyItemOptionList() )
				{
					sprintf(str, "%s %s",					
						pRemovedItem->GetName(),
						(*g_pGameStringTable)[STRING_MESSAGE_ITEM_BROKEN].GetString());
				}
				else
				{
					std::string option_name;
					for(int i = 0; i < pRemovedItem->GetItemOptionListCount(); i++)
					{
						option_name += pRemovedItem->GetItemOptionName(i);
						if(i != pRemovedItem->GetItemOptionListCount())
							option_name += " ";
					}
					sprintf(str, "%s%s %s",					
						option_name.c_str(),	
						pRemovedItem->GetName(),
						(*g_pGameStringTable)[STRING_MESSAGE_ITEM_BROKEN].GetString());
				}
				
				g_pGameMessage->Add(str);
			}
			
			
			
			int addonSlot[] = 
			{
				ADDON_HELM,			
					ADDON_NULL,			
					ADDON_COAT,			
					ADDON_LEFTHAND,		
					ADDON_RIGHTHAND,	
					ADDON_NULL,			
					ADDON_NULL,			
					ADDON_TROUSER,		
					ADDON_NULL,			
					ADDON_NULL,			
					ADDON_NULL,			
					ADDON_NULL,			
					ADDON_NULL,			
					ADDON_NULL,			
					ADDON_NULL,			
			};
		
			//----------------------------------------------------------
			
			
			//----------------------------------------------------------
			if (slotID==MSlayerGear::GEAR_SLAYER_LEFTHAND)
			{
				const MItem* pLeftItem = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_LEFTHAND );
				const MItem* pRightItem = g_pSlayerGear->GetItem( MSlayerGear::GEAR_SLAYER_RIGHTHAND );

				if (pLeftItem==NULL 
					&& pRightItem!=NULL && pRightItem->IsGearSlotTwoHand())
				{
					slotID = MSlayerGear::GEAR_SLAYER_RIGHTHAND;
				}
			}

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			int addonSlotID = addonSlot[slotID];

			if (addonSlotID != ADDON_NULL)
			{
				g_pPlayer->SetStop();

				#ifdef	OUTPUT_DEBUG				
					if (g_pPlayer->RemoveAddon( addonSlotID ))
					{
						DEBUG_ADD_FORMAT("[OK] RemoveAddon. Slot=%d", addonSlotID);
					}
					else
					{
						const MCreatureWear::ADDON_INFO& addonInfo = g_pPlayer->GetAddonInfo( addonSlotID );
						DEBUG_ADD_FORMAT("[Error] RemoveAddon. Slot=%d, AddonFrameID=%d", addonSlotID, addonInfo.FrameID);						
					}				
				#else
					g_pPlayer->RemoveAddon( addonSlotID );
				#endif
			}

			
			UI_RemoveDescriptor( (void*)pRemovedItem );

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			delete pRemovedItem;
		}
	}
	break;

	case RACE_VAMPIRE:
	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	{
		MItem* pRemovedItem = g_pVampireGear->RemoveItem( (MVampireGear::GEAR_VAMPIRE)slotID );

		if (pRemovedItem==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] No Removed Item in Slot=%d", slotID);
		}
		else
		{
			//----------------------------------------------------------
			
			//----------------------------------------------------------
			if(pRemovedItem->GetItemClass() != ITEM_CLASS_COUPLE_RING && pRemovedItem->GetItemClass() != ITEM_CLASS_VAMPIRE_COUPLE_RING)
			{
				char str[128]; 
				
				if (pRemovedItem->IsEmptyItemOptionList())
				{
					sprintf(str, "%s %s",					
						pRemovedItem->GetName(),
						(*g_pGameStringTable)[STRING_MESSAGE_ITEM_BROKEN].GetString());
				}
				else
				{
					std::string option_name;
					for(int i = 0; i < pRemovedItem->GetItemOptionListCount(); i++)
					{
						option_name += pRemovedItem->GetItemOptionName(i);
						if(i != pRemovedItem->GetItemOptionListCount())
							option_name += " ";
					}
					sprintf(str, "%s%s %s",					
						option_name.c_str(),	
						pRemovedItem->GetName(),
						(*g_pGameStringTable)[STRING_MESSAGE_ITEM_BROKEN].GetString());
				}
				
				g_pGameMessage->Add(str);
			}

			int addonSlot[] = 
			{
				ADDON_NULL,			
				ADDON_COAT,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
			};

			//----------------------------------------------------------
			
			
			//----------------------------------------------------------
			if (slotID==MVampireGear::GEAR_VAMPIRE_LEFTHAND)
			{
				const MItem* pLeftItem = g_pVampireGear->GetItem( MVampireGear::GEAR_VAMPIRE_LEFTHAND );
				const MItem* pRightItem = g_pVampireGear->GetItem( MVampireGear::GEAR_VAMPIRE_RIGHTHAND );

				if (pLeftItem==NULL 
					&& pRightItem!=NULL && pRightItem->IsGearSlotTwoHand())
				{
					slotID = MVampireGear::GEAR_VAMPIRE_RIGHTHAND;
				}
			}

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			int addonSlotID = addonSlot[slotID];

			if (addonSlotID != ADDON_NULL)
			{
				g_pPlayer->SetStop();

				#ifdef	OUTPUT_DEBUG				
					if (g_pPlayer->RemoveAddon( addonSlotID ))
					{
						DEBUG_ADD_FORMAT("[OK] RemoveAddon. Slot=%d", addonSlotID);
					}
					else
					{
						const MCreatureWear::ADDON_INFO& addonInfo = g_pPlayer->GetAddonInfo( addonSlotID );
						DEBUG_ADD_FORMAT("[Error] RemoveAddon. Slot=%d, AddonFrameID=%d", addonSlotID, addonInfo.FrameID);						
					}				
				#else
					g_pPlayer->RemoveAddon( addonSlotID );
				#endif
			}

			
			UI_RemoveDescriptor( (void*)pRemovedItem );

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			delete pRemovedItem;
		}
	}
	break;

	case RACE_OUSTERS:
	{
		MItem* pRemovedItem = g_pOustersGear->RemoveItem( (MOustersGear::GEAR_OUSTERS)slotID );

		if (pRemovedItem==NULL)
		{
			DEBUG_ADD_FORMAT("[Error] No Removed Item in Slot=%d", slotID);
		}
		else
		{
			//----------------------------------------------------------
			
			//----------------------------------------------------------
			if(1)
			{
				char str[128]; 
				
				if (pRemovedItem->IsEmptyItemOptionList())
				{
					sprintf(str, "%s %s",					
						pRemovedItem->GetName(),
						(*g_pGameStringTable)[STRING_MESSAGE_ITEM_BROKEN].GetString());
				}
				else
				{
					std::string option_name;
					for(int i = 0; i < pRemovedItem->GetItemOptionListCount(); i++)
					{
						option_name += pRemovedItem->GetItemOptionName(i);
						if(i != pRemovedItem->GetItemOptionListCount())
							option_name += " ";
					}
					sprintf(str, "%s%s %s",					
						option_name.c_str(),	
						pRemovedItem->GetName(),
						(*g_pGameStringTable)[STRING_MESSAGE_ITEM_BROKEN].GetString());
				}
				
				g_pGameMessage->Add(str);
			}

			int addonSlot[] = 
			{
				ADDON_NULL,			
				ADDON_COAT,			
				ADDON_LEFTHAND,		
				ADDON_RIGHTHAND,	
				ADDON_TROUSER,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
				ADDON_NULL,			
			};

			//----------------------------------------------------------
			
			
			//----------------------------------------------------------
			if (slotID==MOustersGear::GEAR_OUSTERS_LEFTHAND)
			{
				const MItem* pLeftItem = g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_LEFTHAND );
				const MItem* pRightItem = g_pOustersGear->GetItem( MOustersGear::GEAR_OUSTERS_RIGHTHAND );

				if (pLeftItem==NULL 
					&& pRightItem!=NULL && pRightItem->IsGearSlotTwoHand())
				{
					slotID = MOustersGear::GEAR_OUSTERS_RIGHTHAND;
				}
			}

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			int addonSlotID = addonSlot[slotID];

			if (addonSlotID != ADDON_NULL)
			{
				g_pPlayer->SetStop();

				#ifdef	OUTPUT_DEBUG				
					if (g_pPlayer->RemoveAddon( addonSlotID ))
					{
						DEBUG_ADD_FORMAT("[OK] RemoveAddon. Slot=%d", addonSlotID);
					}
					else
					{
						const MCreatureWear::ADDON_INFO& addonInfo = g_pPlayer->GetAddonInfo( addonSlotID );
						DEBUG_ADD_FORMAT("[Error] RemoveAddon. Slot=%d, AddonFrameID=%d", addonSlotID, addonInfo.FrameID);						
					}				
				#else
					g_pPlayer->RemoveAddon( addonSlotID );
				#endif
			}

			
			UI_RemoveDescriptor( (void*)pRemovedItem );

			//----------------------------------------------------------
			
			//----------------------------------------------------------
			delete pRemovedItem;
		}
	}
	break;
	}


//	__BEGIN_HELP_EVENT
////		ExecuteHelpEvent( HE_ITEM_BROKEN );
//	__END_HELP_EVENT

#endif

	__END_CATCH
}
