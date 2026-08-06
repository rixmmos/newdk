//////////////////////////////////////////////////////////////////////
//
// Filename    : LCReconnect.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_RECONNECT_H__
#define __LC_RECONNECT_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCReconnect;
//



//
//////////////////////////////////////////////////////////////////////

class LCReconnect : public Packet {
public:
    LCReconnect() {};
    ~LCReconnect() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_RECONNECT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_GameServerIP.size() 
               + szuint                       
               + szDWORD;                     
    }

    // get packet name
    string getPacketName() const {
        return "LCReconnect";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set game server's ip
    string getGameServerIP() const {
        return m_GameServerIP;
    }
    void setGameServerIP(const string& ip) {
        m_GameServerIP = ip;
    }

    // get/set game server's port
    uint getGameServerPort() const {
        return m_GameServerPort;
    }
    void setGameServerPort(uint port) {
        m_GameServerPort = port;
    }

    // get/set key
    DWORD getKey() const {
        return m_Key;
    }
    void setKey(DWORD key) {
        m_Key = key;
    }

private:
    // New GameServer's IP
    string m_GameServerIP;

    // New GameServer's TCP Port
    uint m_GameServerPort;

    // authentication key
    DWORD m_Key;
};


//////////////////////////////////////////////////////////////////////
//
// class LCReconnectFactory;
//
// Factory for LCReconnect
//
//////////////////////////////////////////////////////////////////////

class LCReconnectFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCReconnect();
    }

    // get packet name
    string getPacketName() const {
        return "LCReconnect";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_RECONNECT;
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
// class LCReconnectHandler;
//
//////////////////////////////////////////////////////////////////////

class LCReconnectHandler {
public:
    // execute packet's handler
    static void execute(LCReconnect* pPacket, Player* pPlayer);
};

#endif
