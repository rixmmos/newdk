////////////////////////////////////////////////////////////////////////////////
//
// Filename    : CGDepositPet.h

// Description :
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_DEPOSIT_PET_H__
#define __CG_DEPOSIT_PET_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////
//
// class CGDepositPet;
//
////////////////////////////////////////////////////////////////////////////////

class CGDepositPet : public Packet {
public:
    CGDepositPet() {};
    ~CGDepositPet() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_DEPOSIT_PET;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "CGDepositPet";
    }
    string toString() const;

public:
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t objectID) {
        m_ObjectID = objectID;
    }

    BYTE getIndex(void) const {
        return m_Index;
    }
    void setIndex(BYTE index) {
        m_Index = index;
    }

private:
    ObjectID_t m_ObjectID;
    BYTE m_Index;
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGDepositPetFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGDepositPetFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGDepositPet();
    }
    string getPacketName() const throw() {
        return "CGDepositPet";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_DEPOSIT_PET;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGDepositPetHandler;
//
////////////////////////////////////////////////////////////////////////////////

// Server-only: CGDepositPetHandler::execute has no client-side definition
// or use. Neither pre-migration copy guarded this class declaration, and
// the client's execute() simply omitted the dispatch call rather than
// guarding it — the merged .cpp uses the standard #ifndef __GAME_CLIENT__
// guarded call instead (the CLGetServerList precedent from Wave 3), which
// is behaviorally identical on both sides. A declared-but-never-ODR-used
// static method needs no client-side definition.
class CGDepositPetHandler {
public:
    // execute packet's handler
    static void execute(CGDepositPet* pPacket, Player* player);
    // static void executeSlayer(CGDepositPet* pPacket, Player* player) ;
    // static void executeVampire(CGDepositPet* pPacket, Player* player) ;
};

#endif
