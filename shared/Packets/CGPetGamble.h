
//////////////////////////////////////////////////////////////////////////////
// Filename    : CGPetGamble.h
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __CG_PET_GAMBLE_H__
#define __CG_PET_GAMBLE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class CGPetGamble;
//////////////////////////////////////////////////////////////////////////////

class CGPetGamble : public Packet {
public:
    CGPetGamble();
    ~CGPetGamble();

public:
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_PET_GAMBLE;
    }
    PacketSize_t getPacketSize() const {
        return 0;
    }
    string getPacketName() const {
        return "CGPetGamble";
    }
    string toString() const;

public:
private:
};

//////////////////////////////////////////////////////////////////////////////
// class CGPetGambleFactory;
//////////////////////////////////////////////////////////////////////////////

class CGPetGambleFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGPetGamble();
    }
    string getPacketName() const throw() {
        return "CGPetGamble";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_PET_GAMBLE;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return 0;
    }
};

//////////////////////////////////////////////////////////////////////////////
// class CGPetGambleHandler;
//////////////////////////////////////////////////////////////////////////////

// Server-only: CGPetGambleHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGPetGambleHandler {
public:
    static void execute(CGPetGamble* pPacket, Player* player);
};
#endif

#endif
