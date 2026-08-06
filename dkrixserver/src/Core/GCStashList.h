//////////////////////////////////////////////////////////////////////////////
// Filename    : GCStashList.h

// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_STASH_LIST_H__
#define __GC_STASH_LIST_H__

#include <list>

#include "Packet.h"
#include "PacketFactory.h"
#include "SubItemInfo.h"

typedef struct _STASHITEM {
    int getPacketSize() const {
        return szObjectID + szBYTE + szItemType + szBYTE + optionType.size() + szDurability + szItemNum + szSilver +
               szGrade + szEnchantLevel;
    }

    static int getPacketMaxSize() {
        return szObjectID + szBYTE + szItemType + szBYTE + 255 + szDurability + szItemNum + szSilver + szGrade +
               szEnchantLevel;
    }

    ObjectID_t objectID;
    BYTE itemClass;
    ItemType_t itemType;
    list<OptionType_t> optionType;
    Durability_t durability;
    ItemNum_t num;
    Silver_t silver;
    Grade_t grade;
    EnchantLevel_t enchantLevel;
} STASHITEM;

//////////////////////////////////////////////////////////////////////////////
// class GCStashList;
//////////////////////////////////////////////////////////////////////////////

class Item;

class GCStashList : public Packet {
public:
    GCStashList();
    virtual ~GCStashList();

    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_STASH_LIST;
    }
    PacketSize_t getPacketSize() const;
    string getPacketName() const {
        return "GCStashList";
    }
    string toString() const;

public:
    bool isExist(BYTE rack, BYTE index) const;

    STASHITEM getStashItem(BYTE rack, BYTE index) const;
    void setStashItem(BYTE rack, BYTE index, Item* pItem);

    list<SubItemInfo*>& getSubItems(BYTE rack, BYTE index);

    BYTE getSubItemCount(BYTE rack, BYTE index);

    Gold_t getStashGold() const {
        return m_StashGold;
    }
    void setStashGold(Gold_t gold) {
        m_StashGold = gold;
    }

    BYTE getStashNum() const {
        return m_StashNum;
    }
    void setStashNum(BYTE num) {
        m_StashNum = num;
    }

private:
    bool m_bExist[STASH_RACK_MAX][STASH_INDEX_MAX];
    STASHITEM m_pItems[STASH_RACK_MAX][STASH_INDEX_MAX];
    list<SubItemInfo*> m_pSubItems[STASH_RACK_MAX][STASH_INDEX_MAX];
    BYTE m_SubItemsCount[STASH_RACK_MAX][STASH_INDEX_MAX];
    Gold_t m_StashGold;
    BYTE m_StashNum;
};


//////////////////////////////////////////////////////////////////////////////
// class GCStashListFactory;
//////////////////////////////////////////////////////////////////////////////

class GCStashListFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCStashList();
    }
    string getPacketName() const {
        return "GCStashList";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_STASH_LIST;
    }
    PacketSize_t getPacketMaxSize() const {
        PacketSize_t size = 0;
        PacketSize_t unit_size = szBYTE * 2 +                             
                                                                          
                                 STASHITEM::getPacketMaxSize() + szBYTE + 
                                 SubItemInfo::getSize() * 8;              

        size += szBYTE;                                       
        size += szBYTE;                                       
        size += unit_size * STASH_RACK_MAX * STASH_INDEX_MAX; 
        size += szGold;                                       

        return size;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class GCStashListHandler;
//////////////////////////////////////////////////////////////////////////////

class GCStashListHandler {
public:
    static void execute(GCStashList* pPacket, Player* pPlayer);
};

#endif
