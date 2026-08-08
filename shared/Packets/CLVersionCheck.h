//--------------------------------------------------------------------------------
//
// Filename    : CLVersionCheck.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_VERSION_CHECK_H__
#define __CL_VERSION_CHECK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLVersionCheck;
//


//
//--------------------------------------------------------------------------------

class CLVersionCheck : public Packet {
public:
    CLVersionCheck() {};
    ~CLVersionCheck() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_VERSION_CHECK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szDWORD;
    }

    // get packet name
    string getPacketName() const {
        return "CLVersionCheck";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set Client Version
    DWORD getVersion() const {
        return m_Version;
    }
    void setVersion(DWORD Version) {
        m_Version = Version;
    }

private:

    DWORD m_Version;
};


//--------------------------------------------------------------------------------
//
// class CLVersionCheckFactory;
//
// Factory for CLVersionCheck
//
//--------------------------------------------------------------------------------

// Client Cpackets copy wrapped this whole class in
// #ifdef __DEBUG_OUTPUT__ (the client never needs a factory to decode
// its own outgoing CL packets, only to build debug tooling); adopting
// the server's unconditional canonical style here per Phase 12's
// reconciliation rules is behaviorally neutral -- the class has no
// side effects, it just becomes compilable in more configurations.
class CLVersionCheckFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CLVersionCheck();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CLVersionCheck";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CL_VERSION_CHECK;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szDWORD;
    }
};


//--------------------------------------------------------------------------------
//
// class CLVersionCheckHandler;
//
//--------------------------------------------------------------------------------

// Server-only: CLVersionCheckHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CLVersionCheckHandler {
public:
    // execute packet's handler
    static void execute(CLVersionCheck* pPacket, Player* pPlayer);
};
#endif

#endif
