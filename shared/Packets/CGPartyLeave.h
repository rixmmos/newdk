//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPartyLeave.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_PARTY_LEAVE_H__
#define __CG_PARTY_LEAVE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGPartyLeave
//////////////////////////////////////////////////////////////////////////////

class CGPartyLeave : public Packet {
public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_PARTY_LEAVE;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE + m_TargetName.size();
    }

    string getPacketName() const {
        return "CGPartyLeave";
    }
    string toString() const;

public:
    string getTargetName(void) const {
        return m_TargetName;
    }
    void setTargetName(const string& name) {
        m_TargetName = name;
    }

private:
    string m_TargetName;
};


//////////////////////////////////////////////////////////////////////////////
// class CGPartyLeaveFactory;
//////////////////////////////////////////////////////////////////////////////

// Unconditional (matching the server's pre-migration file): the client
// Cpackets copy wrapped this whole class in #ifdef __DEBUG_OUTPUT__,
// but that macro is never defined by any target in either tree, so the
// class never actually compiled on the client either way (dead code
// pre-migration). The server's own PacketFactoryManager.cpp registers
// CGPartyLeaveFactory unconditionally, so it must stay unconditional
// here or the server build breaks.
class CGPartyLeaveFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGPartyLeave();
    }

    string getPacketName() const throw() {
        return "CGPartyLeave";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_PARTY_LEAVE;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 10;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGPartyLeaveHandler
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGPartyLeaveHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGPartyLeaveHandler {
public:
    static void execute(CGPartyLeave* pPacket, Player* player);
};
#endif

#endif
