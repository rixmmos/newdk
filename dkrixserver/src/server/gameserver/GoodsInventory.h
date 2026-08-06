//////////////////////////////////////////////////////////////////////////
// Filename			: GoodsInventory.h
// Written By		: bezz

//////////////////////////////////////////////////////////////////////////

#ifndef __GOODS_INVENTORY_H__
#define __GOODS_INVENTORY_H__

#include <list>

#include "Item.h"
#include "Types.h"

typedef struct {
    string m_ID;
    Item* m_pItem;
} BuyItem;

//////////////////////////////////////////////////////////////
// Class GoodsInventory
//////////////////////////////////////////////////////////////

class GoodsInventory {
public:
    typedef list<BuyItem> ListItem;
    typedef ListItem::iterator ListItemItr;
    typedef ListItem::const_iterator ListItemConstItr;

public:
    GoodsInventory() {};
    ~GoodsInventory();

public:
    ListItem& getGoods() {
        return m_Goods;
    }

    
    void addItem(string ID, Item* pItem);

    
    Item* popItem(ObjectID_t oid);

    
    Item* getItem(ObjectID_t oid);

    
    bool empty() {
        return m_Goods.empty();
    }

    void clear();

    
    int getNum() const {
        return m_Goods.size();
    }

private:
    ListItem m_Goods;
};

#endif
