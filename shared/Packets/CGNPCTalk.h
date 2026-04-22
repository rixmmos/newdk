//--------------------------------------------------------------------------------
//
// Filename    : CGNPCTalk.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __CG_NPC_TALK_H__
#define __CG_NPC_TALK_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGNPCTalk : public Packet {
public:
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_NPC_TALK;
    }

    PacketSize_t getPacketSize() const throw() {
        return szObjectID;
    }

    std::string getPacketName() const throw() {
        return "CGNPCTalk";
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

class CGNPCTalkFactory : public PacketFactory {
public:
    Packet* createPacket() throw() {
        return new CGNPCTalk();
    }

    std::string getPacketName() const throw() {
        return "CGNPCTalk";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_NPC_TALK;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID;
    }
};

class CGNPCTalkHandler {
public:
    static void execute(CGNPCTalk* pPacket, Player* player);
};

#endif
