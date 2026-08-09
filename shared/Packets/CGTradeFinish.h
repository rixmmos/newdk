////////////////////////////////////////////////////////////////////////////////
// Filename    : CGTradeFinish.h

// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __CG_TRADE_FINISH_H__
#define __CG_TRADE_FINISH_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum {
    
    CG_TRADE_FINISH_ACCEPT = 0,

    
    CG_TRADE_FINISH_REJECT,

    
    CG_TRADE_FINISH_RECONSIDER,


    CG_TRADE_FINISH_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinish;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeFinish : public Packet {
public:
    CGTradeFinish() {};
    virtual ~CGTradeFinish() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_CG_TRADE_FINISH;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "CGTradeFinish";
    }
    string toString() const;

public:
    ObjectID_t getTargetObjectID() const {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t id) {
        m_TargetObjectID = id;
    }

    BYTE getCode() const {
        return m_Code;
    }
    void setCode(BYTE code) {
        m_Code = code;
    }

private:
    ObjectID_t m_TargetObjectID; 
    BYTE m_Code;                 
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinishFactory;
//
////////////////////////////////////////////////////////////////////////////////

class CGTradeFinishFactory : public PacketFactory {
public:
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CGTradeFinish();
    }
    string getPacketName() const throw() {
        return "CGTradeFinish";
    }
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CG_TRADE_FINISH;
    }
    PacketSize_t getPacketMaxSize() const throw() {
        return szObjectID + szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class CGTradeFinishHandler;
//
////////////////////////////////////////////////////////////////////////////////

// Server-only: CGTradeFinishHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CGTradeFinishHandler {
public:
    static void execute(CGTradeFinish* pPacket, Player* player);
    static void executeSlayer(CGTradeFinish* pPacket, Player* player);
    static void executeVampire(CGTradeFinish* pPacket, Player* player);
    static void executeOusters(CGTradeFinish* pPacket, Player* player);
    static void executeError(CGTradeFinish* pPacket, Player* player, BYTE ErrorCode);
};
#endif

#endif
