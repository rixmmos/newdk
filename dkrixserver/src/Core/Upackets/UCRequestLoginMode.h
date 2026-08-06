//--------------------------------------------------------------------------------
//
// Filename    : UCRequestLoginMode.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __UC_REQUEST_LOGIN_MODE_H__
#define __UC_REQUEST_LOGIN_MODE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class UCRequestLoginMode;
//
//--------------------------------------------------------------------------------

class UCRequestLoginMode : public Packet {
public:
    
    void read(SocketInputStream& iStream) throw(ProtocolException, Error) {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }

    
    void read(Socket* pSocket) throw(ProtocolException, Error);

    
    void write(SocketOutputStream& oStream) const throw(ProtocolException, Error) {
        throw UnsupportedError(__PRETTY_FUNCTION__);
    }

    
    void write(Socket* pSocket) const throw(ProtocolException, Error);

    // execute packet's handler
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    // get packet id
    PacketID_t getPacketID() const throw() {
        return PACKET_UC_REQUEST_LOGIN_MODE;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const throw() {
        return szBYTE;
    }

    
    static PacketSize_t getPacketMaxSize() throw() {
        return szBYTE;
    }

    // get packet's name
    string getPacketName() const throw() {
        return "UCRequestLoginMode";
    }

    // get packet's debug string
    string toString() const throw();


public:
    // get/set login mode
    BYTE getLoginMode() const {
        return m_LoginMode;
    }
    void setLoginMode(BYTE loginMode) {
        m_LoginMode = loginMode;
    }

private:
    BYTE m_LoginMode;
};


//--------------------------------------------------------------------------------
//
// class UCRequestLoginModeFactory;
//
// Factory for UCRequestLoginMode
//
//--------------------------------------------------------------------------------

class UCRequestLoginModeFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() throw() {
        return new UCRequestLoginMode();
    }

    // get packet name
    string getPacketName() const throw() {
        return "UCRequestLoginMode";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_UC_REQUEST_LOGIN_MODE;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE;
    }
};


//--------------------------------------------------------------------------------
//
// class UCRequestLoginModeHandler;
//
//--------------------------------------------------------------------------------

class UCRequestLoginModeHandler {
public:
    // execute packet's handler
    static void execute(UCRequestLoginMode* pPacket, Player* pPlayer) throw(ProtocolException, Error);
};

#endif
