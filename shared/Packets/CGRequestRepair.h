//-----------------------------------------------------------------------------
//
// Filename    : CGRequestRepair.h
// Written By  :
// Description :
//
//-----------------------------------------------------------------------------

#ifndef __CG_REQUEST_REPAIR_H__
#define __CG_REQUEST_REPAIR_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGRequestRepair : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_REQUEST_REPAIR;
    }

    PacketSize_t getPacketSize() const throw() {
        return szObjectID;
    }

    std::string getPacketName() const throw() {
        return "CGRequestRepair";
    }

    std::string toString() const throw();

    ObjectID_t getObjectID() throw() {
        return m_ObjectID;
    }

    void setObjectID(ObjectID_t objectID) throw() {
        m_ObjectID = objectID;
    }

private:
    ObjectID_t m_ObjectID;
};

class CGRequestRepairFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGRequestRepair();
    }

    std::string getPacketName() const throw() {
        return "CGRequestRepair";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_REQUEST_REPAIR;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};

#ifndef __GAME_CLIENT__
class CGRequestRepairHandler {
public:
    static void execute(CGRequestRepair* pPacket, Player* player);
    static void executeNormal(CGRequestRepair* pPacket, Player* player);
    static void executeMotorcycle(CGRequestRepair* pPacket, Player* player);
    static void executeAll(CGRequestRepair* pPacket, Player* player);
};
#endif

#endif
