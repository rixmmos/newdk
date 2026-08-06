//////////////////////////////////////////////////////////////////////
//
// Filename    : LCSelectPCError.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_SELECT_PC_ERROR_H__
#define __LC_SELECT_PC_ERROR_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

enum SELECT_PC_ERROR {
    SELECT_PC_ERROR_NULL,

    SELECT_PC_CANNOT_PLAY,         
    SELECT_PC_NOT_BILLING_CHECK,   
    SELECT_PC_CANNOT_PLAY_BY_ATTR, 
    SELECT_PC_DIDNOT_AGREE,        
};

//////////////////////////////////////////////////////////////////////
//
// class LCSelectPCError;
//



//
//////////////////////////////////////////////////////////////////////

class LCSelectPCError : public Packet {
public:
    LCSelectPCError() {};
    ~LCSelectPCError() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_SELECT_PC_ERROR;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "LCSelectPCError";
    }

    // get packet's debug string
    string toString() const;

    // get/set error message
    BYTE getCode() const {
        return m_Code;
    }
    void setCode(BYTE code) {
        m_Code = code;
    }

private:
    
    BYTE m_Code;
};


//////////////////////////////////////////////////////////////////////
//
// class LCSelectPCErrorFactory;
//
// Factory for LCSelectPCError
//
//////////////////////////////////////////////////////////////////////

class LCSelectPCErrorFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCSelectPCError();
    }

    // get packet name
    string getPacketName() const {
        return "LCSelectPCError";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_SELECT_PC_ERROR;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCSelectPCErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LCSelectPCErrorHandler {
public:
    // execute packet's handler
    static void execute(LCSelectPCError* pPacket, Player* pPlayer);
};

#endif
