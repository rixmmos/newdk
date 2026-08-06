//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemMap.h

// Description :

//////////////////////////////////////////////////////////////////////////////

#ifndef __ITEM_MAP__
#define __ITEM_MAP__

#include <list>
#include <map>

#include "Item.h"

//////////////////////////////////////////////////////////////////////////////
// class ItemMap
//////////////////////////////////////////////////////////////////////////////

class Inventory;
class Item;

class ItemMap : public std::map<ulonglong, Item*> {
    // Inner type definition
public:
    typedef std::map<ulonglong, Item*> ITEM_MAP;
    typedef ITEM_MAP::iterator iterator;
    typedef ITEM_MAP::const_iterator const_iterator;

    // Member methods
public:
    ItemMap();
    ~ItemMap();

public:
    
    void clearAll(void) {
        clear();
        m_Num2x2 = 0;
        m_Num2x2Temp = 0;
    }

    
    bool addItem(Item* pItem);

    
    void set2x2(int n);
    int get2x2() const {
        return m_Num2x2;
    }

protected:
    
    ulonglong getKey(Item* pItem);

    // Member data
protected:
    int m_Num2x2;
    int m_Num2x2Temp; 
};


#endif
