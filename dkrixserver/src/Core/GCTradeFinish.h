////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeFinish.h

// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_FINISH_H__
#define __GC_TRADE_FINISH_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum {
    
    GC_TRADE_FINISH_ACCEPT = 0,

    
    GC_TRADE_FINISH_REJECT,

    
    GC_TRADE_FINISH_RECONSIDER,

    
    GC_TRADE_FINISH_EXECUTE,

    GC_TRADE_FINISH_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeFinish;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeFinish : public Packet {
public:
    GCTradeFinish() {};
    ~GCTradeFinish() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_TRADE_FINISH;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "GCTradeFinish";
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
// class GCTradeFinishFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeFinishFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCTradeFinish();
    }
    string getPacketName() const {
        return "GCTradeFinish";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_TRADE_FINISH;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeFinishHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeFinishHandler {
public:
    static void execute(GCTradeFinish* pPacket, Player* pPlayer);
};

#endif
