//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPartyError.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_PARTY_ERROR_H__
#define __GC_PARTY_ERROR_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
enum {
    
    GC_PARTY_ERROR_TARGET_NOT_EXIST = 0,

    
    GC_PARTY_ERROR_RACE_DIFFER,

    
    GC_PARTY_ERROR_NOT_SAFE,

    
    GC_PARTY_ERROR_NOT_NORMAL_FORM,

    
    GC_TRADE_ERROR_ALREADY_INVITING,

    
    GC_PARTY_ERROR_NOT_INVITING,

    
    GC_PARTY_ERROR_NO_AUTHORITY,

    
    GC_TRADE_ERROR_UNKNOWN,

    GC_PARTY_ERROR_MAX
};

//////////////////////////////////////////////////////////////////////////////
// class GCPartyError;
//////////////////////////////////////////////////////////////////////////////

class GCPartyError : public Packet {
public:
    GCPartyError() {};
    ~GCPartyError() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_PARTY_ERROR;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE + szObjectID;
    }
    string getPacketName() const {
        return "GCPartyError";
    }
    string toString() const;

public:
    BYTE getCode() const {
        return m_Code;
    }
    void setCode(BYTE code) {
        m_Code = code;
    }

    ObjectID_t getTargetObjectID(void) const {
        return m_TargetObjectID;
    }
    void setTargetObjectID(ObjectID_t id) {
        m_TargetObjectID = id;
    }

private:
    ObjectID_t m_TargetObjectID;
    BYTE m_Code; 
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyErrorFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPartyErrorFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCPartyError();
    }
    string getPacketName() const {
        return "GCPartyError";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_PARTY_ERROR;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szObjectID;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyErrorHandler;
//////////////////////////////////////////////////////////////////////////////

class GCPartyErrorHandler {
public:
    static void execute(GCPartyError* pPacket, Player* pPlayer);
};

#endif
