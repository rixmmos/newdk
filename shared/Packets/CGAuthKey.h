//////////////////////////////////////////////////////////////////////////////
// Filename    : CGAuthKey.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_AUTH_KEY_H__
#define __CG_AUTH_KEY_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGAuthKey;

//////////////////////////////////////////////////////////////////////////////

class CGAuthKey : public Packet {
public:
    CGAuthKey() {};
    ~CGAuthKey() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_AUTH_KEY;
    }
    PacketSize_t getPacketSize() const {
        return szDWORD;
    }
    string getPacketName() const {
        return "CGAuthKey";
    }
    string toString() const;

    DWORD getKey() const {
        return m_Key;
    }
    void setKey(DWORD key) {
        m_Key = key;
    }

private:
    DWORD m_Key;
};


//////////////////////////////////////////////////////////////////////////////
// class CGAuthKeyFactory;
//////////////////////////////////////////////////////////////////////////////


class CGAuthKeyFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGAuthKey();
    }
    string getPacketName() const throw() {
        return "CGAuthKey";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_AUTH_KEY;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class CGAuthKeyHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGAuthKeyHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGAuthKeyHandler {
public:
    static void execute(CGAuthKey* pPacket, Player* pPlayer);
};
#endif

#endif
