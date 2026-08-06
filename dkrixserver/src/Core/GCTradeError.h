////////////////////////////////////////////////////////////////////////////////
// Filename    : GCTradeError.h

// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __GC_TRADE_ERROR_H__
#define __GC_TRADE_ERROR_H__

#include "Packet.h"
#include "PacketFactory.h"

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

enum {
    
    GC_TRADE_ERROR_CODE_TARGET_NOT_EXIST = 0,

    
    GC_TRADE_ERROR_CODE_RACE_DIFFER,

    
    GC_TRADE_ERROR_CODE_NOT_SAFE,

    
    GC_TRADE_ERROR_CODE_MOTORCYCLE,

    
    GC_TRADE_ERROR_CODE_BAT_OR_WOLF,

    
    GC_TRADE_ERROR_CODE_ALREADY_TRADING,

    
    GC_TRADE_ERROR_CODE_NOT_TRADING,

    
    GC_TRADE_ERROR_CODE_ADD_ITEM,

    
    GC_TRADE_ERROR_CODE_REMOVE_ITEM,

    
    GC_TRADE_ERROR_CODE_INCREASE_MONEY,

    
    GC_TRADE_ERROR_CODE_DECREASE_MONEY,

    
    GC_TRADE_ERROR_CODE_NOT_ENOUGH_SPACE,

    
    GC_TRADE_ERROR_CODE_EVENT_GIFT_BOX,

    
    GC_TRADE_ERROR_CODE_UNKNOWN,

    GC_TRADE_ERROR_CODE_MAX
};

////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeError;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeError : public Packet {
public:
    GCTradeError() {};
    ~GCTradeError() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_TRADE_ERROR;
    }
    PacketSize_t getPacketSize() const {
        return szObjectID + szBYTE;
    }
    string getPacketName() const {
        return "GCTradeError";
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
// class GCTradeErrorFactory;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeErrorFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCTradeError();
    }
    string getPacketName() const {
        return "GCTradeError";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_TRADE_ERROR;
    }
    PacketSize_t getPacketMaxSize() const {
        return szObjectID + szBYTE;
    }
};


////////////////////////////////////////////////////////////////////////////////
//
// class GCTradeErrorHandler;
//
////////////////////////////////////////////////////////////////////////////////

class GCTradeErrorHandler {
public:
    static void execute(GCTradeError* pPacket, Player* pPlayer);
};

#endif
