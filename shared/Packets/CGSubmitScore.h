//////////////////////////////////////////////////////////////////////
// Filename    : CGSubmitScore.h
//////////////////////////////////////////////////////////////////////

#ifndef __CG_SUBMIT_SCORE_H__
#define __CG_SUBMIT_SCORE_H__

#include "Types.h"
#include "Exception.h"
#include "Packet.h"
#include "PacketFactory.h"

class CGSubmitScore : public Packet {
public:
    CGSubmitScore() throw();
    ~CGSubmitScore() throw();

    void read(SocketInputStream& iStream) throw(ProtocolException, Error);
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    PacketID_t getPacketID() const throw() {
        return PACKET_CG_SUBMIT_SCORE;
    }

    PacketSize_t getPacketSize() const throw() {
        return szBYTE + szBYTE + szWORD;
    }

    std::string getPacketName() const throw() {
        return "CGSubmitScore";
    }

    std::string toString() const throw();

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

class CGSubmitScoreFactory : public PacketFactory {
public:
    CGSubmitScoreFactory() throw() {}
    virtual ~CGSubmitScoreFactory() throw() {}

    Packet* createPacket() throw() {
        return new CGSubmitScore();
    }

    std::string getPacketName() const throw() {
        return "CGSubmitScore";
    }

    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_SUBMIT_SCORE;
    }

    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + szBYTE + szWORD;
    }
};

class CGSubmitScoreHandler {
public:
    static void execute(CGSubmitScore* pPacket, Player* player);
};

#endif
