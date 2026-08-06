////////////////////////////////////////////////////////////////////////////////
// Filename    : GCPetStashListHandler.cpp

// Description :
////////////////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPetStashList.h"
#include "ClientDef.h"
#include "MStorage.h"
#include "UIDialog.h"
#include "MGameStringTable.h"
#include "UIFunction.h"
#include "ExperienceTable.h"
void GCPetStashListHandler::execute ( GCPetStashList * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY 
//	__BEGIN_DEBUG_EX
	__BEGIN_DEBUG
	
#ifdef __GAME_CLIENT__
		//------------------------------------------------------------
		
		//------------------------------------------------------------
		if (g_pStorage!=NULL) 
		{
			delete g_pStorage;
		}

		g_pStorage = new MStorage;

		
		g_pStorage->Init( 1 ); 

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		g_pStorage->SetCurrent( 0 );

		std::vector<PetStashItemInfo*> vPetInfo = pPacket->getPetStashItemInfos();

		std::vector<PetStashItemInfo*>::const_iterator		itr;

		itr = vPetInfo.begin();
		PetInfo* pPetInfo;
		PetStashItemInfo* petStashItemInfo;
		int index = 0;
		while(itr != vPetInfo.end() && index < 20)
		{
			petStashItemInfo = (PetStashItemInfo*)*itr;
			if(NULL == petStashItemInfo)
			{
				itr ++;
				index ++;
				continue;
			}
			pPetInfo = petStashItemInfo->pPetInfo;
			if(NULL == pPetInfo)
			{
				itr ++;
				index ++;
				continue;
			}
			MPetItem *pItem = NULL;
			pItem = dynamic_cast<MPetItem *>(MItem::NewItem( ITEM_CLASS_PET_ITEM ));
			pItem->SetItemType( pPetInfo->getPetType() );
			pItem->SetCurrentDurability( pPetInfo->getPetHP() );
			pItem->SetEnchantLevel( pPetInfo->getPetAttr() );
			pItem->SetSilver( pPetInfo->getPetAttrLevel() );
			pItem->SetNumber( pPetInfo->getPetLevel() );
			pItem->SetPetFoodType( pPetInfo->getFoodType() );
			int accum = 0;
			accum = g_pExperienceTable->GetPetExp(pPetInfo->getPetLevel()).AccumExp;
			pItem->SetPetExpRemain(accum-pPetInfo->getPetExp());
			pItem->SetPetCutHead( pPetInfo->canCutHead()?true:false );
			pItem->SetPetGamble( pPetInfo->canGamble()?true:false );
			pItem->SetPetAttack( pPetInfo->canAttack()?true:false );
			
			pItem->ClearItemOption();
			if(pPetInfo->getPetOption() != NULL)
			{
				pItem->AddItemOption(pPetInfo->getPetOption());
			}

			pItem->SetID( pPetInfo->getPetItemObjectID());
			pItem->SetItemType( pPetInfo->getPetType() );
			
			pItem->SetPetKeepedDay(petStashItemInfo->KeepDays);
			//------------------------------------------------------------
			
			//------------------------------------------------------------
			if (!g_pStorage->SetItem( index, pItem ))
			{
				
				delete pItem;

				DEBUG_ADD_FORMAT("[Error] Can't Add Item to Storage. slot=%d", index);
			}

			itr ++;
			index ++;
		}

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		UI_RunPetStorage();
		UI_SetPetStorage(g_pStorage);
#endif

	__END_DEBUG
//	__END_DEBUG_EX 
	__END_CATCH
}
