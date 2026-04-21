//////////////////////////////////////////////////////////////////////
// Filename    : CGUntransform.h
// Written By  : crazydog
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_UNTRANSFORM_H__
#define __CG_UNTRANSFORM_H__

#include "Packet.h"
#include "PacketFactory.h"

class CGUntransform : public Packet {
public:
    CGUntransform() {}
    virtual ~CGUntransform() {}

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_UNTRANSFORM; }
    PacketSize_t getPacketSize() const throw() { return 0; }
    string getPacketName() const throw() { return "CGUntransform"; }
    string toString() const throw();
};

class CGUntransformFactory : public PacketFactory {
public:
    Packet* createPacket() throw() { return new CGUntransform(); }
    string getPacketName() const throw() { return "CGUntransform"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_UNTRANSFORM; }
    PacketSize_t getPacketMaxSize() const throw() { return 0; }
};

class CGUntransformHandler {
public:
    static void execute(CGUntransform* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
