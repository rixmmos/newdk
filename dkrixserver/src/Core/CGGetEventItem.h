////////////////////////////////////////////////////////////////////////////////
// Filename    : CGGetEventItem.h

// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_GET_EVENT_ITEM_H__
#define __CG_GET_EVENT_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
enum EventType {
    EVENT_TYPE_200501_COMBACK_ITEM = 0,       
    EVENT_TYPE_200501_COMBACK_PREMIUM_ITEM,   
    EVENT_TYPE_200501_COMBACK_RECOMMEND_ITEM, 

    EVENT_TYPE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGGetEventItem
//
////////////////////////////////////////////////////////////////////////////////
class CGGetEventItem : public Packet {
public:
    CGGetEventItem() {};
    ~CGGetEventItem() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_GET_EVENT_ITEM;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }
    string getPacketName() const {
        return "CGGetEventItem";
    }
    string toString() const;

public:
    // get / set Event Type
    BYTE getEventType() const {
        return m_EventType;
    }
    void setEventType(BYTE eventType) {
        m_EventType = eventType;
    }

private:
    BYTE m_EventType; 
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGGetEventItemFactory
//
////////////////////////////////////////////////////////////////////////////////

class CGGetEventItemFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new CGGetEventItem();
    }
    string getPacketName() const {
        return "CGGetEventItem";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_GET_EVENT_ITEM;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGGetEventItemHandler
//
////////////////////////////////////////////////////////////////////////////////
class CGGetEventItemHandler {
public:
    static void execute(CGGetEventItem* pPacket, Player* player);

#ifdef __GAME_SERVER__
    static void executeCombackItem(CGGetEventItem* pPacket, Player* pPlayer);
    static void executeCombackPremiumItem(CGGetEventItem* pPacket, Player* pPlayer);
    static void executeCombackRecommendItem(CGGetEventItem* pPacket, Player* pPlayer);
#endif
};

#endif
