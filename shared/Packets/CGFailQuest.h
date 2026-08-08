//////////////////////////////////////////////////////////////////////
//
// Filename    : CGFailQuest.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_FAIL_QUEST_H__
#define __CG_FAIL_QUEST_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGFailQuest;
//
//////////////////////////////////////////////////////////////////////

class CGFailQuest : public Packet {
public:
    CGFailQuest() {};
    ~CGFailQuest() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_FAIL_QUEST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }

    // get packet name
    string getPacketName() const {
        return "CGFailQuest";
    }

    // get packet's debug string
    string toString() const;

public:
    BYTE isFail() const {
        return m_bFail != 0;
    }
    void setFail(bool bFail) {
        m_bFail = (bFail) ? 1 : 0;
    }

private:
    BYTE m_bFail;
};


// Both CGFailQuestFactory and CGFailQuestHandler are wrapped together in
// #ifndef __GAME_CLIENT__ here, matching the client Cpackets copy's own
// pre-existing choice exactly (batch precedent: Wave 1 batch 2's
// CGGQuestCancel/CGModifyTaxRatio). Safe on the server: __GAME_CLIENT__
// is never defined there, so both classes stay defined; the server's
// PacketFactoryManager.cpp registration and the CGFailQuestHandler
// dispatch call both keep working unchanged.
#ifndef __GAME_CLIENT__

//////////////////////////////////////////////////////////////////////
//
// class CGFailQuestFactory;
//
// Factory for CGFailQuest
//
//////////////////////////////////////////////////////////////////////

class CGFailQuestFactory : public PacketFactory {
public:
    // constructor
    CGFailQuestFactory() {}

    // destructor
    virtual ~CGFailQuestFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGFailQuest();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGFailQuest";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_FAIL_QUEST;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGFailQuestHandler;
//
//////////////////////////////////////////////////////////////////////

class CGFailQuestHandler {
public:
    // execute packet's handler
    static void execute(CGFailQuest* pCGFailQuest, Player* pPlayer);
};
#endif

#endif
