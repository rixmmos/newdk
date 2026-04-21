//////////////////////////////////////////////////////////////////////
//
// Filename    : CLGetWorldList.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CL_GET_WORLD_LIST_H__
#define __CL_GET_WORLD_LIST_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLGetWorldList;
//
//////////////////////////////////////////////////////////////////////

class CLGetWorldList : public Packet {
public:
    CLGetWorldList() {};
    virtual ~CLGetWorldList() {};
    // Initialize the packet by reading data from the input stream.
    void read(SocketInputStream& iStream) throw(ProtocolException, Error);

    // Serialize the packet into the output stream.
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);

    // execute packet's handler
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    // get packet id
    PacketID_t getPacketID() const throw() {
        return PACKET_CL_GET_WORLD_LIST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const throw() {
        return 0;
    }

    // get packet name
    string getPacketName() const throw() {
        return "CLGetWorldList";
    }

    // get packet's debug string
    string toString() const throw() {
        return "CLGetWorldList";
    }

private:
};


//////////////////////////////////////////////////////////////////////
//
// class CLGetWorldListFactory;
//
// Factory for CLGetWorldList
//
//////////////////////////////////////////////////////////////////////

class CLGetWorldListFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() throw() {
        return new CLGetWorldList();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CLGetWorldList";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CL_GET_WORLD_LIST;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLGetWorldListHandler;
//
//////////////////////////////////////////////////////////////////////

class CLGetWorldListHandler {
public:
    // execute packet's handler
    static void execute(CLGetWorldList* pPacket, Player* player) throw(ProtocolException, Error);
};

#endif
