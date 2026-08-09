//////////////////////////////////////////////////////////////////////
//
// Filename    : CGAttack
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ATTACK_H__
#define __CG_ATTACK_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

#ifdef __LINUX__
// #include "GCGlobalHandler.h"
#endif // __LINUX__


//////////////////////////////////////////////////////////////////////
//
// class CGAttack;
//
//////////////////////////////////////////////////////////////////////

class CGAttack : public Packet {
public:
    // constructor
    CGAttack();

    // destructor
    ~CGAttack();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_ATTACK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szObjectID + szCoord + szCoord + szDir;
    }

    // get packet name
    string getPacketName() const {
        return "CGAttack";
    }

    // get packet's debug string
    string toString() const;

    // get/set X Coordicate
    Coord_t getX() const {
        return m_X;
    }
    void setX(Coord_t x) {
        m_X = x;
    }

    // get/set Y Coordicate
    Coord_t getY() const {
        return m_Y;
    }
    void setY(Coord_t y) {
        m_Y = y;
    }

    // get/set Direction
    Dir_t getDir() const {
        return m_Dir;
    }
    void setDir(Dir_t dir) {
        m_Dir = dir;
    }

    // get/set ObjectID
    ObjectID_t getObjectID() const {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

private:
    ObjectID_t m_ObjectID; // ObjectID
    Coord_t m_X;           
    Coord_t m_Y;           
    Dir_t m_Dir;           
};


//////////////////////////////////////////////////////////////////////
//
// class CGAttackFactory;
//
// Factory for CGAttack
//
//////////////////////////////////////////////////////////////////////

class CGAttackFactory : public PacketFactory {
public:
    // constructor
    CGAttackFactory() {}

    // destructor
    virtual ~CGAttackFactory() {}


public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGAttack();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGAttack";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_ATTACK;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szCoord + szCoord + szDir;
    }
};

//////////////////////////////////////////////////////////////////////
//
// class CGAttackHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGAttackHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGAttackHandler {
public:
    // execute packet's handler
    static void execute(CGAttack* pCGAttack, Player* player);
};
#endif

#endif
