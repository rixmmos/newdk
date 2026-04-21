//////////////////////////////////////////////////////////////////////
// Filename    : CGVisible.h
// Written By  : crazydog
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_VISIBLE_H__
#define __CG_VISIBLE_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGVisible : public Packet {
public:
    CGVisible() {}
    virtual ~CGVisible() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_VISIBLE; }
    PacketSize_t getPacketSize() const throw() { return 0; }
    string getPacketName() const throw() { return "CGVisible"; }
    string toString() const throw();
};

class CGVisibleFactory : public PacketFactory {
public:
    Packet* createPacket() throw() { return new CGVisible(); }
    string getPacketName() const throw() { return "CGVisible"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_VISIBLE; }
    PacketSize_t getPacketMaxSize() const throw() { return 0; }
};

class CGVisibleHandler {
public:
    static void execute(CGVisible* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
