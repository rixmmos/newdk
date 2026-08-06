//////////////////////////////////////////////////////////////////////
//
// Filename    : LGIncomingConnectionOK.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LG_INCOMING_CONNECTION_OK_H__
#define __LG_INCOMING_CONNECTION_OK_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionOK;
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionOK : public DatagramPacket {
public:
    LGIncomingConnectionOK() {};
    ~LGIncomingConnectionOK() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LG_INCOMING_CONNECTION_OK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_PlayerID.size() + szuint + szDWORD;
    }

    // get packet name
    string getPacketName() const {
        return "LGIncomingConnectionOK";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set player id
    string getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(string playerID) {
        m_PlayerID = playerID;
    }

    // get/set tcp port
    uint getTCPPort() const {
        return m_TCPPort;
    }
    void setTCPPort(uint tcpPort) {
        m_TCPPort = tcpPort;
    }

    // get/set auth key
    DWORD getKey() const {
        return m_Key;
    }
    void setKey(DWORD key) {
        m_Key = key;
    }

private:
    
    string m_PlayerID;

    
    
    uint m_TCPPort;

    
    DWORD m_Key;
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionOKFactory;
//
// Factory for LGIncomingConnectionOK
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LGIncomingConnectionOK();
    }

    // get packet name
    string getPacketName() const {
        return "LGIncomingConnectionOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LG_INCOMING_CONNECTION_OK;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20 + szuint + szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionOKHandler {
public:
    // execute packet's handler
    static void execute(LGIncomingConnectionOK* pPacket);
};

#endif
