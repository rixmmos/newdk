
//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSelectBloodBible.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_BLOOD_BIBLE_H__
#define __CG_SELECT_BLOOD_BIBLE_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSelectBloodBible;
//
//////////////////////////////////////////////////////////////////////

class CGSelectBloodBible : public Packet {
public:
    CGSelectBloodBible() {};
    virtual ~CGSelectBloodBible() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_BLOOD_BIBLE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szItemType;
    }

    // get packet name
    string getPacketName() const {
        return "CGSelectBloodBible";
    }

    // get packet's debug string
    string toString() const;

    // get/set BloodBibleID
    ItemType_t getBloodBibleID() const {
        return m_BloodBibleID;
    }
    void setBloodBibleID(ItemType_t BloodBibleID) {
        m_BloodBibleID = BloodBibleID;
    }


private:
    // BloodBible ID
    ItemType_t m_BloodBibleID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectBloodBibleFactory;
//
// Factory for CGSelectBloodBible
//
//////////////////////////////////////////////////////////////////////

class CGSelectBloodBibleFactory : public PacketFactory {
public:
    // constructor
    CGSelectBloodBibleFactory() {}

    // destructor
    virtual ~CGSelectBloodBibleFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGSelectBloodBible();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGSelectBloodBible";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_BLOOD_BIBLE;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szItemType;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectBloodBibleHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGSelectBloodBibleHandler::execute has no client-side
// definition or use. No CGHandlersStub.cpp-style client stub exists for
// this family, but unlike CGAuthKey the client's own pre-migration copy
// left this class declaration itself unguarded (only the .cpp's dispatch
// call is guarded) — a static method that is declared but never
// ODR-used under __GAME_CLIENT__ needs no definition, so this matches
// the client tree's existing behavior exactly.
class CGSelectBloodBibleHandler {
public:
    // execute packet's handler
    static void execute(CGSelectBloodBible* pCGSelectBloodBible, Player* pPlayer);
};

#endif
