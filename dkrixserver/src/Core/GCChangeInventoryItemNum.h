//////////////////////////////////////////////////////////////////////
//
// Filename    : GCChangeInventoryItemNum.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_CHANGE_INVENTORY_ITEM_NUM_H__
#define __GC_CHANGE_INVENTORY_ITEM_NUM_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class GCChangeInventoryItemNum;
//

//
//////////////////////////////////////////////////////////////////////

class GCChangeInventoryItemNum {
public:
    // constructor
    GCChangeInventoryItemNum();

    // destructor
    ~GCChangeInventoryItemNum();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    PacketSize_t getPacketSize() const {
        return szBYTE + szObjectID * m_ChangedItemListNum + szItemNum * m_ChangedItemListNum;
    }

    // get packet's debug string
    string toString() const;

    // get / set ListNumber
    BYTE getChangedItemListNum() const {
        return m_ChangedItemListNum;
    }
    void setChangedItemListNum(BYTE ListNum) {
        m_ChangedItemListNum = ListNum;
    }

    // add / delete / clear S List
    void addChangedItemListElement(ObjectID_t objectID, BYTE itemNum);

    // ClearList
    void clearChangedItemList() {
        m_ChangedItemList.clear();
        m_ChangedItemNumList.clear();
        m_ChangedItemListNum = 0;
    }

    // pop front Element in Object List
    ObjectID_t popFrontChangedItemListElement() {
        ObjectID_t item = m_ChangedItemList.front();
        m_ChangedItemList.pop_front();
        return item;
    }
    ItemNum_t popFrontChangedItemNumListElement() {
        ItemNum_t itemNum = m_ChangedItemNumList.front();
        m_ChangedItemNumList.pop_front();
        return itemNum;
    }

protected:
    // List Element Number
    BYTE m_ChangedItemListNum;

    
    list<ObjectID_t> m_ChangedItemList;   
    list<ItemNum_t> m_ChangedItemNumList; 
};

#endif
