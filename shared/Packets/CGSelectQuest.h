//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSelectQuest.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SELECT_QUEST_H__
#define __CG_SELECT_QUEST_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class CGSelectQuest;
//
//////////////////////////////////////////////////////////////////////

class CGSelectQuest : public Packet {
public:
    CGSelectQuest() {};
    virtual ~CGSelectQuest() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SELECT_QUEST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szQuestID + szObjectID;
    }

    // get packet name
    string getPacketName() const {
        return "CGSelectQuest";
    }

    // get packet's debug string
    string toString() const;

    // get/set QuestID
    QuestID_t getQuestID() const {
        return m_QuestID;
    }
    void setQuestID(QuestID_t QuestID) {
        m_QuestID = QuestID;
    }

    ObjectID_t getNPCObjectID() const {
        return m_NPCOID;
    }
    void setNPCObjectID(ObjectID_t oid) {
        m_NPCOID = oid;
    }

private:
    // Quest ID
    QuestID_t m_QuestID;

    
    ObjectID_t m_NPCOID;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectQuestFactory;
//
// Factory for CGSelectQuest
//
//////////////////////////////////////////////////////////////////////

class CGSelectQuestFactory : public PacketFactory {
public:
    // constructor
    CGSelectQuestFactory() {}

    // destructor
    virtual ~CGSelectQuestFactory() {}


public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGSelectQuest();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGSelectQuest";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SELECT_QUEST;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szQuestID + szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGSelectQuestHandler;
//
//////////////////////////////////////////////////////////////////////

// Unlike most Phase 12 pairs, this Handler class is NOT guarded by
// #ifndef __GAME_CLIENT__: dkrix/Client/CGHandlersStub.cpp already
// provides an empty-body client-side definition of
// CGSelectQuestHandler::execute (the "CGStoreOpen precedent"). Keeping
// it unconditional here means CGHandlersStub.cpp needs no structural
// change — only its #include and exception spec were updated.
class CGSelectQuestHandler {
public:
    // execute packet's handler
    static void execute(CGSelectQuest* pCGSelectQuest, Player* pPlayer);
};

#endif
