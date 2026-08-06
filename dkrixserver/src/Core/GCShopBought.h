//////////////////////////////////////////////////////////////////////////////
// Filename    : GCShopBought.h

// Description :





//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_SHOP_BOUGHT_H__
#define __GC_SHOP_BOUGHT_H__

#include <list>

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCShopBought;
//////////////////////////////////////////////////////////////////////////////

class GCShopBought : public Packet {
public:
    GCShopBought();
    virtual ~GCShopBought();

    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_SHOP_BOUGHT;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID +                   // NPC OID
               szShopVersion +                // shop version
               szShopRackType +               // shop rack type
               szBYTE +                       // shop rack index
               szObjectID +                   // item OID
               szBYTE +                       // item class
               szItemType +                   // item type
               szBYTE + m_OptionType.size() + // item option type
               szDurability +                 // item durability
               szSilver +                     // silver coating amount
               szGrade + szEnchantLevel;      // enchant level
    }
    string getPacketName() const {
        return "GCShopBought";
    }
    string toString() const;


public:
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t creatureID) {
        m_ObjectID = creatureID;
    }

    ShopVersion_t getShopVersion(void) const {
        return m_Version;
    }
    void setShopVersion(const ShopVersion_t ver) {
        m_Version = ver;
    }

    ShopRackType_t getShopType(void) const {
        return m_ShopType;
    }
    void setShopType(ShopRackType_t type) {
        m_ShopType = type;
    }

    BYTE getShopIndex(void) const {
        return m_ShopIndex;
    }
    void setShopIndex(BYTE index) {
        m_ShopIndex = index;
    }

    ObjectID_t getItemObjectID(void) const {
        return m_ItemObjectID;
    }
    void setItemObjectID(ObjectID_t oid) {
        m_ItemObjectID = oid;
    }

    int getItemClass(void) const {
        return m_ItemClass;
    }
    void setItemClass(int iclass) {
        m_ItemClass = iclass;
    }

    ItemType_t getItemType(void) const {
        return m_ItemType;
    }
    void setItemType(ItemType_t type) {
        m_ItemType = type;
    }

    int getOptionTypeSize(void) const {
        return m_OptionType.size();
    }
    const list<OptionType_t>& getOptionType() const {
        return m_OptionType;
    }
    OptionType_t popOptionType(void) {
        if (m_OptionType.empty())
            return 0;
        OptionType_t optionType = m_OptionType.front();
        m_OptionType.pop_front();
        return optionType;
    }
    void addOptionType(OptionType_t type) {
        m_OptionType.push_back(type);
    }
    void setOptionType(const list<OptionType_t>& OptionTypes) {
        m_OptionType = OptionTypes;
    }

    Durability_t getDurability(void) const {
        return m_Durability;
    }
    void setDurability(Durability_t dur) {
        m_Durability = dur;
    }

    Silver_t getSilver(void) const {
        return m_Silver;
    }
    void setSilver(Silver_t silver) {
        m_Silver = silver;
    }

    Grade_t getGrade(void) const {
        return m_Grade;
    }
    void setGrade(Grade_t grade) {
        m_Grade = grade;
    }

    EnchantLevel_t getEnchantLevel(void) const {
        return m_EnchantLevel;
    }
    void setEnchantLevel(EnchantLevel_t level) {
        m_EnchantLevel = level;
    }


private:
    ObjectID_t m_ObjectID;           // NPC's object id
    ShopVersion_t m_Version;         
    ShopRackType_t m_ShopType;       
    BYTE m_ShopIndex;                
    ObjectID_t m_ItemObjectID;       
    BYTE m_ItemClass;                
    ItemType_t m_ItemType;           
    list<OptionType_t> m_OptionType; 
    Durability_t m_Durability;       
    Silver_t m_Silver;               
    Grade_t m_Grade;
    EnchantLevel_t m_EnchantLevel; 
};


//////////////////////////////////////////////////////////////////////////////
// class GCShopBoughtFactory;
//////////////////////////////////////////////////////////////////////////////

class GCShopBoughtFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCShopBought();
    }
    string getPacketName() const {
        return "GCShopBought";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_SHOP_BOUGHT;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID +              // NPC OID
               szShopVersion +           // shop version
               szShopRackType +          // shop rack type
               szBYTE +                  // shop rack index
               szObjectID +              // item OID
               szBYTE +                  // item class
               szItemType +              // item type
               szBYTE + 255 +            // item option type
               szDurability +            // item durablility
               szSilver +                // silver coating amount
               szGrade + szEnchantLevel; // enchant level
    }
};


//////////////////////////////////////////////////////////////////////////////
// class GCShopBoughtHandler;
//////////////////////////////////////////////////////////////////////////////

class GCShopBoughtHandler {
public:
    static void execute(GCShopBought* pPacket, Player* pPlayer);
};

#endif
