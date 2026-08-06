//////////////////////////////////////////////////////////////////////////////
// Filename    : PriceManager.h

// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __PRICEMANAGER_H__
#define __PRICEMANAGER_H__

#include "Exception.h"
#include "Item.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// class PriceManager
//////////////////////////////////////////////////////////////////////////////

// class Item;
class Creature;

class PriceManager {
public:
    
    Price_t getPrice(Item* pItem, MarketCond_t nDiscount, ShopRackType_t shopType, Creature* pCreature) const;

    
    Price_t getMysteriousPrice(Item::ItemClass itemClass, Creature* pCreature) const;

    
    Price_t getRepairPrice(Item* pItem, Creature* pCreature = NULL) const;

    
    Price_t getSilverCoatingPrice(Item* pItem, Creature* pCreature = NULL) const;

    
    Price_t getStashPrice(BYTE index, Creature* pCreature = NULL) const;

    
    int getStarPrice(Item* pItem, XMAS_STAR& star) const;
    int getBallPrice(int price, XMAS_STAR& star) const;
};

// global variable declaration
extern PriceManager* g_pPriceManager;

#endif
