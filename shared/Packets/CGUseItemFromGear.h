//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseItemFromGear.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_USE_ITEM_FROM_GEAR_H__
#define __CG_USE_ITEM_FROM_GEAR_H__

#include "Packet.h"
#include "PacketFactory.h"

class GamePlayer;
class Item;

class CGUseItemFromGear : public Packet {
public:
    CGUseItemFromGear() {}
    virtual ~CGUseItemFromGear() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() {
        return PACKET_CG_USE_ITEM_FROM_GEAR;
    }
    PacketSize_t getPacketSize() const throw() {
        return szObjectID + szBYTE;
    }
    string getPacketName() const throw() {
        return "CGUseItemFromGear";
    }
    string toString() const throw();

    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) {
        m_ObjectID = objectID;
    }

    BYTE getPart() const {
        return m_Part;
    }
    void setPart(BYTE part) {
        m_Part = part;
    }

private:
    ObjectID_t m_ObjectID;
    BYTE m_Part;
};

class CGUseItemFromGearFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGUseItemFromGear();
    }
    string getPacketName() const throw() {
        return "CGUseItemFromGear";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_USE_ITEM_FROM_GEAR;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szBYTE;
    }
};

class CGUseItemFromGearHandler {
public:
    static void execute(CGUseItemFromGear* pPacket, Player* player) throw(ProtocolException, Error);

protected:
    static void executeCoupleRing(CGUseItemFromGear* pPacket, GamePlayer* pGamePlayer) throw(ProtocolException, Error);
};

#endif
