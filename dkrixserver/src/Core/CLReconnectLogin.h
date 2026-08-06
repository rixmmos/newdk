//////////////////////////////////////////////////////////////////////
//
// Filename    : CLReconnectLogin.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CL_RECONNECT_LOGIN_H__
#define __CL_RECONNECT_LOGIN_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLReconnectLogin;
//



//
//////////////////////////////////////////////////////////////////////

class CLReconnectLogin : public Packet {
public:
    CLReconnectLogin() {};
    virtual ~CLReconnectLogin() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_RECONNECT_LOGIN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szDWORD + szBYTE; // authentication key
    }

    // get packet name
    string getPacketName() const {
        return "CLReconnectLogin";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set key
    DWORD getKey() const {
        return m_Key;
    }
    void setKey(DWORD key) {
        m_Key = key;
    }

    // Web login
    void setWebLogin() {
        m_LoginMode = LOGIN_MODE_WEBLOGIN;
    }
    bool isWebLogin() const {
        return m_LoginMode == LOGIN_MODE_WEBLOGIN;
    }

private:
    // authentication key
    DWORD m_Key;

    // Login Mode
    BYTE m_LoginMode;
};


//////////////////////////////////////////////////////////////////////
//
// class CLReconnectLoginFactory;
//
// Factory for CLReconnectLogin
//
//////////////////////////////////////////////////////////////////////

class CLReconnectLoginFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLReconnectLogin();
    }

    // get packet name
    string getPacketName() const {
        return "CLReconnectLogin";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_RECONNECT_LOGIN;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szDWORD + szBYTE; // authentication key
    }
};

//////////////////////////////////////////////////////////////////////
//
// class CLReconnectLoginHandler;
//
//////////////////////////////////////////////////////////////////////

class CLReconnectLoginHandler {
public:
    // execute packet's handler
    static void execute(CLReconnectLogin* pPacket, Player* pPlayer);
    static bool onChildGuardTimeArea(int pm, int am, string enable);
};

#endif
