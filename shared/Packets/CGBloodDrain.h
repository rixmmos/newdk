//////////////////////////////////////////////////////////////////////
// Filename    : CGBloodDrain
// Written By  : crazydog
// Description :
//////////////////////////////////////////////////////////////////////

#ifndef __CG_BLOOD_DRAIN_H__
#define __CG_BLOOD_DRAIN_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGBloodDrain : public Packet {
public:
    CGBloodDrain() throw();
    ~CGBloodDrain() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);
    PacketID_t getPacketID() const throw() { return PACKET_CG_BLOOD_DRAIN; }
    PacketSize_t getPacketSize() const throw() { return szObjectID; }
    string getPacketName() const throw() { return "CGBloodDrain"; }
    string toString() const throw();

    ObjectID_t getObjectID() const throw() { return m_ObjectID; }
    void setObjectID(ObjectID_t objectID) throw() { m_ObjectID = objectID; }

private:
    ObjectID_t m_ObjectID;
};

class CGBloodDrainFactory : public PacketFactory {
public:
    CGBloodDrainFactory() throw() {}
    virtual ~CGBloodDrainFactory() throw() {}

    Packet* createPacket() throw() { return new CGBloodDrain(); }
    string getPacketName() const throw() { return "CGBloodDrain"; }
    PacketID_t getPacketID() const throw() { return Packet::PACKET_CG_BLOOD_DRAIN; }
    PacketSize_t getPacketMaxSize() const throw() { return szObjectID; }
};

class CGBloodDrainHandler {
public:
    static void execute(CGBloodDrain* pCGBloodDrain, Player* player) throw(ProtocolException, Error);
};

#endif
