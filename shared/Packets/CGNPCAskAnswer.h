//--------------------------------------------------------------------------------
//
// Filename    : CGNPCAskAnswer.h

//
//--------------------------------------------------------------------------------

#ifndef __CG_NPC_ASKANSWER_H__
#define __CG_NPC_ASKANSWER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CGNPCAskAnswer;
//
//--------------------------------------------------------------------------------

class CGNPCAskAnswer : public Packet {
public:
    CGNPCAskAnswer() {};
    ~CGNPCAskAnswer() {};

    void read(SocketInputStream& iStream);


    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_NPC_ASK_ANSWER;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*

    PacketSize_t getPacketSize() const {
        return szObjectID + szScriptID + szAnswerID;
    }

    // get packet name
    string getPacketName() const {
        return "CGNPCAskAnswer";
    }

    // get packet's debug string
    string toString() const;

public:
    // get / set ObjectID
    ObjectID_t getObjectID() {
        return m_ObjectID;
    }
    void setObjectID(ObjectID_t ObjectID) {
        m_ObjectID = ObjectID;
    }

    ScriptID_t getScriptID() {
        return m_ScriptID;
    }
    void setScriptID(ScriptID_t scriptID) {
        m_ScriptID = scriptID;
    }

    AnswerID_t getAnswerID() {
        return m_AnswerID;
    }
    void setAnswerID(AnswerID_t answerID) {
        m_AnswerID = answerID;
    }

private:
    // ObjectID
    ObjectID_t m_ObjectID;
    ScriptID_t m_ScriptID;
    AnswerID_t m_AnswerID;
};


//--------------------------------------------------------------------------------
//
// class CGNPCAskAnswerFactory;
//
// Factory for CGNPCAskAnswer
//
//--------------------------------------------------------------------------------

// Unconditional (matching the server's pre-migration file): the client
// Cpackets copy wrapped this whole class in #ifdef __DEBUG_OUTPUT__,
// but that macro is never defined by any target in either tree, so the
// class never actually compiled on the client either way (dead code
// pre-migration). The server's own PacketFactoryManager.cpp registers
// CGNPCAskAnswerFactory unconditionally, so it must stay unconditional
// here or the server build breaks.
class CGNPCAskAnswerFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    // create packet
    Packet* createPacket() throw() {
        return new CGNPCAskAnswer();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGNPCAskAnswer";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_NPC_ASK_ANSWER;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*

    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szScriptID + szAnswerID;
    }
};


//--------------------------------------------------------------------------------
//
// class CGNPCAskAnswerHandler;
//
//--------------------------------------------------------------------------------

// Server-only: CGNPCAskAnswerHandler::execute has no client-side
// definition or use. Guarded (matching the client Cpackets copy's
// existing guard) since no CGHandlersStub.cpp-style client stub exists
// for this family.
#ifndef __GAME_CLIENT__
class CGNPCAskAnswerHandler {
public:
    // execute packet's handler
    static void execute(CGNPCAskAnswer* pPacket, Player* player);
};
#endif

#endif
