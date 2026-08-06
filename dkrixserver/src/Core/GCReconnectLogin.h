//////////////////////////////////////////////////////////////////////
//
// Filename    : GCReconnectLogin.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_RECONNECT_LOGIN_H__
#define __GC_RECONNECT_LOGIN_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCReconnectLogin;
//



//
//////////////////////////////////////////////////////////////////////

class GCReconnectLogin : public Packet {
public:
    GCReconnectLogin() {};
    ~GCReconnectLogin() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_RECONNECT_LOGIN;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_LoginServerIP.size() 
               + szuint                        
               + szDWORD;                      
    }

    // get packet name
    string getPacketName() const {
        return "GCReconnectLogin";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set game server's ip
    string getLoginServerIP() const {
        return m_LoginServerIP;
    }
    void setLoginServerIP(const string& ip) {
        m_LoginServerIP = ip;
    }

    // get/set game server's port
    uint getLoginServerPort() const {
        return m_LoginServerPort;
    }
    void setLoginServerPort(uint port) {
        m_LoginServerPort = port;
    }

    // get/set key
    DWORD getKey() const {
        return m_Key;
    }
    void setKey(DWORD key) {
        m_Key = key;
    }

private:
    // New LoginServer's IP
    string m_LoginServerIP;

    // New LoginServer's TCP Port
    uint m_LoginServerPort;

    // authentication key
    DWORD m_Key;
};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectLoginFactory;
//
// Factory for GCReconnectLogin
//
//////////////////////////////////////////////////////////////////////

class GCReconnectLoginFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCReconnectLogin();
    }

    // get packet name
    string getPacketName() const {
        return "GCReconnectLogin";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_RECONNECT_LOGIN;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 15 
               + szuint    
               + szDWORD;  
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCReconnectLoginHandler;
//
//////////////////////////////////////////////////////////////////////

class GCReconnectLoginHandler {
public:
    // execute packet's handler
    static void execute(GCReconnectLogin* pPacket, Player* pPlayer);
};

#endif
