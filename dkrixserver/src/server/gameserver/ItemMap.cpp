////////////////////////////////////////////////////////////////////////////////
// Filename : ItemMap.h

// Description :

////////////////////////////////////////////////////////////////////////////////

#include "ItemMap.h"

#include "Inventory.h"
#include "Item.h"

#define TWO_BY_TWO_PACKING_SIZE 12

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
ItemMap::ItemMap()

{
    __BEGIN_TRY

    m_Num2x2 = 0;
    m_Num2x2Temp = 0;

    __END_CATCH
}

////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
ItemMap::~ItemMap()

{
    __BEGIN_TRY

    clear();

    __END_CATCH_NO_RETHROW
}

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void ItemMap::set2x2(int n)

{
    __BEGIN_TRY

    
    
    
    m_Num2x2 = n;
    m_Num2x2Temp = (n / 3) * 3;

    __END_CATCH
}

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
bool ItemMap::addItem(Item* pItem)

{
    __BEGIN_TRY

    ulonglong key = getKey(pItem);

    ITEM_MAP::const_iterator itr = find(key);

    if (itr == end()) 
    {
        insert(ITEM_MAP::value_type(key, pItem));
        return true;
    }

    return false;

    __END_CATCH
}


////////////////////////////////////////////////////////////

//

//
// 4 Byte : gridWidth* gridHeight
// 4 Byte : ObjectID
//

////////////////////////////////////////////////////////////
ulonglong ItemMap::getKey(Item* pItem)

{
    __BEGIN_TRY

    ObjectID_t objectID = pItem->getObjectID();
    int gridWidth = pItem->getVolumeWidth();
    int gridHeight = pItem->getVolumeHeight();
    int gridSize = gridWidth * gridHeight;

    
    if (gridSize == 4) {
        if (m_Num2x2Temp > 0) {
            m_Num2x2Temp--;
            gridSize = TWO_BY_TWO_PACKING_SIZE;
        }
    }

    gridSize = 0xFF - gridSize;

    ulonglong key = gridSize;
    key = (key << 32) | objectID;

    return key;

    __END_CATCH
}
