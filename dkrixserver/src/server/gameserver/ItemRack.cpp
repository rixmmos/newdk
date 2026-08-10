//////////////////////////////////////////////////////////////////////////////
// Filename    : ItemRack.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ItemRack.h"

#include "Assert.h"
#include "Item.h"
#include "ParkingCenter.h"
#include "item/Key.h"

//////////////////////////////////////////////////////////////////////////////
// class ItemRack member methods
//////////////////////////////////////////////////////////////////////////////

ItemRack::ItemRack() {
    m_ppItem = NULL;
    m_nSize = 0;
}

ItemRack::ItemRack(int size) {
    
    m_ppItem = new Item*[size];
    Assert(m_ppItem != NULL);

    
    for (int i = 0; i < size; i++)
        m_ppItem[i] = NULL;

    m_nSize = size;
}

ItemRack::~ItemRack() {
    if (m_ppItem != NULL) {
        for (int i = 0; i < m_nSize; i++) {
            Item* pItem = m_ppItem[i];

            if (pItem != NULL && pItem->getItemClass() == Item::ITEM_CLASS_KEY) {
                Key* pKey = dynamic_cast<Key*>(pItem);
                
                if (g_pParkingCenter->hasMotorcycleBox(pKey->getTarget())) {
                    g_pParkingCenter->deleteMotorcycleBox(pKey->getTarget());
                }
            }

            SAFE_DELETE(m_ppItem[i]);
        }

        SAFE_DELETE_ARRAY(m_ppItem);
    }
}

void ItemRack::init(int size) {
    
    if (m_ppItem != NULL) {
        for (int i = 0; i < m_nSize; i++)
            SAFE_DELETE(m_ppItem[i]);

        SAFE_DELETE_ARRAY(m_ppItem);
    }

    
    m_ppItem = new Item*[size];
    Assert(m_ppItem != NULL);

    
    for (int i = 0; i < size; i++)
        m_ppItem[i] = NULL;

    m_nSize = size;
}

bool ItemRack::isFull(void) const {
    
    for (int i = 0; i < m_nSize; i++)
        if (m_ppItem[i] == NULL)
            return false;

    return true;
}

bool ItemRack::isEmpty(void) const {
    
    for (int i = 0; i < m_nSize; i++)
        if (m_ppItem[i] != NULL)
            return false;

    return true;
}

// The four index accessors below were guarded only by Assert(verifyIndex(...)),
// which disappears under NDEBUG. `index` is the NPC shop slot and reaches here
// as a raw BYTE off the wire (CGShopRequestBuyHandler's pPacket->getShopIndex(),
// via NPC::getShopItem/insertShopItem/removeShopItem/isExistShopItem), while
// m_ppItem holds only m_nSize pointers. insert() and remove() write, so this is
// an arbitrary-offset heap write and not merely a read.
//
// They throw rather than returning a sentinel, for the same reason as the NPC
// accessors that call them: the buy handler dereferences get()'s result without
// a NULL test. Throwing also matches today's Debug behaviour, where the Assert
// already throws. verifyIndex() itself is unchanged and still the single
// definition of "in range".

bool ItemRack::isExist(BYTE index) const {
    if (!verifyIndex(index))
        throw OutOfBoundException("ItemRack::isExist: index out of range");

    Assert(verifyIndex(index));


    return (m_ppItem[index] == NULL ? false : true);
}

void ItemRack::insert(BYTE index, Item* pItem) {
    if (!verifyIndex(index))
        throw OutOfBoundException("ItemRack::insert: index out of range");

    Assert(verifyIndex(index));


    Assert(m_ppItem[index] == NULL);


    m_ppItem[index] = pItem;
}

void ItemRack::remove(BYTE index) {
    if (!verifyIndex(index))
        throw OutOfBoundException("ItemRack::remove: index out of range");

    Assert(verifyIndex(index));


    m_ppItem[index] = NULL;
}

Item* ItemRack::get(BYTE index) {
    if (!verifyIndex(index))
        throw OutOfBoundException("ItemRack::get: index out of range");

    Assert(verifyIndex(index));

    return m_ppItem[index];
}

void ItemRack::clear(void) {
    for (int i = 0; i < m_nSize; i++)
        SAFE_DELETE(m_ppItem[i]);
}

BYTE ItemRack::getFirstEmptySlot(void) const {
    
    for (int i = 0; i < m_nSize; i++)
        if (m_ppItem[i] == NULL)
            return i;

    return m_nSize;
}

BYTE ItemRack::getLastEmptySlot(void) const {
    
    for (int i = m_nSize - 1; i >= 0; i--)
        if (m_ppItem[i] == NULL)
            return i;

    return m_nSize;
}

bool ItemRack::verifyIndex(BYTE index) const {
    if (index >= m_nSize)
        return false;
    return true;
}
