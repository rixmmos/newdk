////////////////////////////////////////////////////////////////////////////////
// Filename    : CGGetEventItem.h
// Written By  :
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_GET_EVENT_ITEM_H__
#define __CG_GET_EVENT_ITEM_H__

#include "Packet.h"
#include "PacketFactory.h"

enum EventType {
    EVENT_TYPE_200501_COMBACK_ITEM = 0,
    EVENT_TYPE_200501_COMBACK_PREMIUM_ITEM,
    EVENT_TYPE_200501_COMBACK_RECOMMEND_ITEM,
    EVENT_TYPE_MAX
};

class CGGetEventItem : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_GET_EVENT_ITEM;
    }

    PacketSize_t getPacketSize() const throw() {
        return szBYTE;
    }

    std::string getPacketName() const throw() {
        return "CGGetEventItem";
    }

    std::string toString() const throw();

    BYTE getEventType() const {
        return m_EventType;
    }

    void setEventType(BYTE eventType) {
        m_EventType = eventType;
    }

private:
    BYTE m_EventType;
};

class CGGetEventItemFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGGetEventItem();
    }

    std::string getPacketName() const throw() {
        return "CGGetEventItem";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_GET_EVENT_ITEM;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE;
    }
};

#ifndef __GAME_CLIENT__
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

#endif
