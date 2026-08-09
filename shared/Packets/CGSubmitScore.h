//////////////////////////////////////////////////////////////////////
//
// Filename    : CGSubmitScore
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SUBMIT_SCORE_H__
#define __CG_SUBMIT_SCORE_H__

// include files
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Types.h"


//////////////////////////////////////////////////////////////////////
//
// class CGSubmitScore;
//
//////////////////////////////////////////////////////////////////////

class CGSubmitScore : public Packet {
public:
    // constructor
    CGSubmitScore();

    // destructor
    ~CGSubmitScore();


public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_SUBMIT_SCORE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + szBYTE + szWORD;
    }

    // get packet name
    string getPacketName() const {
        return "CGSubmitScore";
    }

    // get packet's debug string
    string toString() const;

public:
    BYTE getGameType() const {
        return m_GameType;
    }
    void setGameType(BYTE type) {
        m_GameType = type;
    }

    BYTE getLevel() const {
        return m_Level;
    }
    void setLevel(BYTE level) {
        m_Level = level;
    }

    WORD getScore() const {
        return m_Score;
    }
    void setScore(WORD score) {
        m_Score = score;
    }

private:
    BYTE m_GameType;
    BYTE m_Level;
    WORD m_Score;
};


//////////////////////////////////////////////////////////////////////
//
// class CGSubmitScoreFactory;
//
// Factory for CGSubmitScore
//
//////////////////////////////////////////////////////////////////////

class CGSubmitScoreFactory : public PacketFactory {
public:
    // constructor
    CGSubmitScoreFactory() {}

    // destructor
    virtual ~CGSubmitScoreFactory() {}


public:
    // create packet
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGSubmitScore();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CGSubmitScore";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SUBMIT_SCORE;
    }

    // get Packet Max Size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + szBYTE + szWORD;
    }
};

//////////////////////////////////////////////////////////////////////
//
// class CGSubmitScoreHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CGSubmitScoreHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGSubmitScoreHandler {
public:
    // execute packet's handler
    static void execute(CGSubmitScore* pCGSubmitScore, Player* player);
};
#endif

#endif
