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
    void read(SocketInputStream& iStream);

    // Serialize the packet into the output stream.
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_GET_WORLD_LIST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return 0;
    }

    // get packet name
    string getPacketName() const {
        return "CLGetWorldList";
    }

    // get packet's debug string
    string toString() const {
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
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; the override must be at least as strict or C++11
    // rejects it as a looser exception specification. Keep throw() (not
    // noexcept) to match the rest of the codebase until a sweep bumps
    // everything. The server tree's base PacketFactory is unconstrained,
    // so throw() here is a legal narrowing on that side too.
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

// Server-only: CLGetWorldListHandler::execute has no client-side
// definition or use. Guarded (not unconditional, unlike CGStoreOpen's
// migrated header) because the client Cpackets copy already guarded it
// this way and no CGHandlersStub.cpp-style client stub exists for the CL
// family — keeping the guard avoids needing to add one.
#ifndef __GAME_CLIENT__
class CLGetWorldListHandler {
public:
    // execute packet's handler
    static void execute(CLGetWorldList* pPacket, Player* player);
};
#endif

#endif
