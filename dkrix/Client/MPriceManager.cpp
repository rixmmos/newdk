//-----------------------------------------------------------------------------
// MPriceManager.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)

#include "MPriceManager.h"
#include "MItem.h"
#include "MItemOptionTable.h"
#include "MEventManager.h"
#include "MSkillManager.h"
#include "UserInformation.h"

#ifdef __GAME_CLIENT__
	#include "MPlayer.h"
	#include "MZone.h"
	#include "MTimeItemManager.h"
#else
	#include "VS_UI.h"
#endif

#define CHARGE_PRICE		5000

//extern std::map<int, bool> g_mapPremiumZone;

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
MPriceManager*		g_pPriceManager = NULL;

//-----------------------------------------------------------------------------
//
// contructor / destructor
// 
//-----------------------------------------------------------------------------
MPriceManager::MPriceManager()
{
	m_MarketCondBuy		= 25;		
	m_MarketCondSell	= 100;		
	m_EventFixPrice		= 0;
}

MPriceManager::~MPriceManager()
{
}

//-----------------------------------------------------------------------------
//
// member functions
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get Item Price
//-----------------------------------------------------------------------------
int
MPriceManager::GetItemPrice(MItem* pItem, TRADE_TYPE type, bool bMysterious)
{
	if (pItem==NULL)
	{
		return 0;
	}
	if(!pItem->IsIdentified())
		return GetMysteriousPrice(pItem);

	__int64	finalPrice;


	// 2004, 08, 02, sobeit add start
	if(pItem->GetItemClass() == ITEM_CLASS_MOON_CARD && pItem->GetItemType() == 4)
		return m_EventFixPrice;
	// 2004, 08, 02, sobeit add end				
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	int nRatio;

	switch (type)
	{
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		case NPC_TO_PC :
			if (pItem->GetItemClass()==ITEM_CLASS_SKULL)
			{
				
				nRatio = m_MarketCondBuy;
			}
			else
			{
				nRatio = m_MarketCondSell;
			}
		break;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		case PC_TO_NPC :
			nRatio = m_MarketCondBuy;
		break;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		case SILVERING :
		{
			ITEM_CLASS itemClass = pItem->GetItemClass();

			if (itemClass==ITEM_CLASS_BLADE
				|| itemClass==ITEM_CLASS_SWORD
				|| itemClass==ITEM_CLASS_CROSS
				|| itemClass==ITEM_CLASS_MACE)
			{
				double    maxSilver  = pItem->GetSilverMax();
				double    curSilver  = pItem->GetSilver();
				double    finalPrice = 0;

				
				if (maxSilver==curSilver)
				{
					return 0;
				}

				
				finalPrice = maxSilver; 

				return (int)finalPrice;
			}
			else
			{
				return 0;
			}
		}
		break;

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		case REPAIR :
			
			if (pItem->GetItemClass()==ITEM_CLASS_VAMPIRE_PORTAL_ITEM 
				
#ifdef __GAME_CLIENT__
				|| g_pTimeItemManager->IsExist( pItem->GetID() )
#endif
				// 2004, 8, 17, sobeit add end

				
				|| pItem->GetItemClass() == ITEM_CLASS_BLOOD_BIBLE_SIGN
				// 2004, 10, 21, sobeit add end
				)
			{
				return 0;	
			}

			
			nRatio = 10;

			
//			bMysterious = false;
		break;
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	if (pItem->IsChargeItem())
	{		
		int curCharge = pItem->GetNumber();
		int maxCharge = pItem->GetMaxNumber();
		
		int ChargePrice = CHARGE_PRICE;
		
		if( pItem->GetItemClass() == ITEM_CLASS_OUSTERS_SUMMON_ITEM )
			ChargePrice = 1000;

		if (type==REPAIR)
		{
			
			int charge = maxCharge - curCharge;

			return charge * ChargePrice;
		}
		
		int itemPrice = pItem->GetPrice();

		int finalPrice = itemPrice + curCharge * ChargePrice;
			
		finalPrice = finalPrice * nRatio / 100;

		return finalPrice;
	}	

	//-------------------------------------------------------
	//
	//						Skull
	//
	//-------------------------------------------------------
	/*
	if (pItem->GetItemClass()==ITEM_CLASS_SKULL)
	{
		if (type==REPAIR)
		{
			return 0;
		}

		//finalPrice = pItem->GetCurrentDurability();
	}
	*/
	//-------------------------------------------------------
	//
	
	//
	//-------------------------------------------------------
	//else
	{
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		
		int		itemDur = pItem->GetMaxDurability();
		long	curDurability = pItem->GetCurrentDurability();
		
		//--------------------------------------------------
		
		
		//--------------------------------------------------
		if (type==REPAIR)
		{
			if (itemDur<0 || itemDur==curDurability)
			{
				return 0;
			}
		}
		

		if (itemDur<0)
		{
			itemDur = 0;
		}

		long	originalPrice = pItem->GetPrice();
		long	maxDurability = itemDur;
			
		finalPrice = originalPrice;
		
		//-------------------------------------------------------
		
		//-------------------------------------------------------
//		if (bMysterious)
//		{

//			finalPrice = finalPrice*nRatio / 100;
//

//			finalPrice = finalPrice * 5;
//		}
//		else
		{
			
			if (!pItem->IsEmptyItemOptionList())
			{
				int priceMult = pItem->GetItemOptionPriceMultiplier();//(*g_pItemOptionTable)[pItem->GetItemOption()].PriceMultiplier;
				finalPrice = finalPrice * priceMult / 100;
			}
			
			
			float damaged;
			if (maxDurability==0)
			{
				damaged = 1;
			}
			else
			{
				damaged = (float)curDurability / (float)maxDurability;
			}

			//--------------------------------------------------
			
			//--------------------------------------------------
			if (type==REPAIR)
			{
				damaged = 1.0f - damaged;
			}
			
			finalPrice = finalPrice * damaged;
			
			
			finalPrice = finalPrice * nRatio / 100;
		}	
	}

	
	//-------------------------------------------------------
	
	//-------------------------------------------------------
#ifdef __GAME_CLIENT__
	if (pItem->GetItemClass()==ITEM_CLASS_POTION)
	{
		if ((pItem->GetItemType()==0 || pItem->GetItemType()==5)
			&& g_pPlayer->IsSlayer()			
			&& g_pPlayer->GetSTR() + g_pPlayer->GetDEX() + g_pPlayer->GetINT() <= 40)
		{
			finalPrice = finalPrice * 70 / 100;
		}

	}

	if(//g_pZone != NULL && g_mapPremiumZone[g_pZone->GetID()] == true &&
			(
				pItem->GetItemClass() == ITEM_CLASS_POTION ||
				pItem->GetItemClass() == ITEM_CLASS_SERUM ||
				pItem->GetItemClass() == ITEM_CLASS_LARVA ||
				pItem->GetItemClass() == ITEM_CLASS_PUPA ||
				pItem->GetItemClass() == ITEM_CLASS_COMPOS_MEI
			) &&
		g_pEventManager->IsEvent(EVENTID_PREMIUM_HALF)
		)
	{
		finalPrice /= 2;
	}
	else if(g_pSkillAvailable->IsEnableSkill(SKILL_HOLYLAND_BLOOD_BIBLE_NEMA)
			&&
				(
					pItem->GetItemClass() == ITEM_CLASS_POTION ||
					pItem->GetItemClass() == ITEM_CLASS_SERUM ||
					pItem->GetItemClass() == ITEM_CLASS_LARVA ||
					pItem->GetItemClass() == ITEM_CLASS_PUPA ||
					pItem->GetItemClass() == ITEM_CLASS_COMPOS_MEI
				)
			)
	{
		finalPrice /= 2;
	}

	if(g_pEventManager->IsEvent(EVENTID_TAX_CHANGE) && type == NPC_TO_PC)
	{
		// finalPrice = finalPrice + finalPrice * tax / 100;
		finalPrice = finalPrice *(g_pEventManager->GetEvent(EVENTID_TAX_CHANGE)->parameter1)/100;
	}
#endif

	
	if (finalPrice==0)
	{
		return 1;
	}

	#ifdef __GAME_CLIENT__
		if (pItem->GetItemClass()==ITEM_CLASS_SKULL
			&& g_pPlayer->IsVampire())
		{
			finalPrice >>= 1;	
		} 
		else
		if (pItem->GetItemClass()==ITEM_CLASS_SKULL &&g_pPlayer->IsOusters() )
		{
			finalPrice = finalPrice * 75 / 100;
		}
	#endif

		
	
	if(pItem->GetItemClass() == ITEM_CLASS_SKULL)
	{
		finalPrice    = finalPrice * g_pUserInformation->HeadPrice / 100;
	}


	return (int)finalPrice;
}

//-----------------------------------------------------------------------------
// Get ItemPrice
//-----------------------------------------------------------------------------

// price.type = -1;
// price.number = 0;
//-----------------------------------------------------------------------------
void		
MPriceManager::GetItemPrice(MItem* pItem, STAR_ITEM_PRICE& price)
{
	if (pItem==NULL)
	{
		price.type = -1;
		price.number = 0;
		return;
	}

	
	switch (pItem->GetItemOptionPart())
    {
		case ITEMOPTION_TABLE::PART_DAMAGE		: price.type = 0; break;	
        case ITEMOPTION_TABLE::PART_STR			: price.type = 1; break;	
        case ITEMOPTION_TABLE::PART_INT			: price.type = 2; break;	
        case ITEMOPTION_TABLE::PART_DEX			: price.type = 3; break;	
        case ITEMOPTION_TABLE::PART_ATTACK_SPEED	: price.type = 4; break;	

        default: 
			price.type = -1;
			price.number = 0;
		return;
	}

	
	price.number = (pItem->GetItemType() - 1) * 20;
}




int MPriceManager::GetMysteriousPrice(MItem *pItem) const
{
	int multiplier = 1;
#ifdef __GAME_CLIENT__
	if (g_pPlayer->IsSlayer())
	{
		int CSTR = g_pPlayer->GetBASIC_STR();
		int CDEX = g_pPlayer->GetBASIC_DEX();
		int CINT = g_pPlayer->GetBASIC_INT();
		int CSUM = CSTR + CDEX + CINT;

		
		if(CSUM > 0)
			multiplier = CSUM / 15;
	}
	else
	{
		int CLevel = g_pPlayer->GetLEVEL();

		
		multiplier = CLevel / 5;
	}

	
	multiplier = max(1, multiplier);

	
	int avr = (*g_pItemTable)[pItem->GetItemClass()].GetAveragePrice();
#else
	if(g_eRaceInterface == RACE_SLAYER)
	{
		int CLevel = g_char_slot_ingame.level;

		multiplier = CLevel / 5;
	} else
	{
		int CSTR = g_char_slot_ingame.STR_PURE;
		int CDEX = g_char_slot_ingame.DEX_PURE;
		int CINT = g_char_slot_ingame.INT_PURE;
		int CSUM = CSTR + CDEX + CINT;

		if(CSUM > 0)
			multiplier = CSUM / 15;
	}

	multiplier = max(1, multiplier);
	int avr = (*g_pItemTable)[pItem->GetItemClass()].GetAveragePrice();
#endif

	
	int final_price = avr * multiplier;

	if(g_pSkillAvailable->IsEnableSkill(SKILL_HOLYLAND_BLOOD_BIBLE_JAVE))
	{
		final_price /= 2;
	}

	if(g_pEventManager->IsEvent(EVENTID_TAX_CHANGE))
	{
		// finalPrice = finalPrice + finalPrice * tax / 100;
		final_price = final_price *(g_pEventManager->GetEvent(EVENTID_TAX_CHANGE)->parameter1)/100;
	}

	return final_price;
}

