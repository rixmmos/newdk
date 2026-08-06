//////////////////////////////////////////////////////////////////////
//
// Filename    : GLIncomingConnectionOK.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GL_INCOMING_CONNECTION_OK_H__
#define __GL_INCOMING_CONNECTION_OK_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionOK;
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionOK : public DatagramPacket {
public:
    GLIncomingConnectionOK() {};
    ~GLIncomingConnectionOK() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GL_INCOMING_CONNECTION_OK;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_PlayerID.size() + szuint + szDWORD;
    }

    // get packet name
    string getPacketName() const {
        return "GLIncomingConnectionOK";
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
// class GLIncomingConnectionOKFactory;
//
// Factory for GLIncomingConnectionOK
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GLIncomingConnectionOK();
    }

    // get packet name
    string getPacketName() const {
        return "GLIncomingConnectionOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GL_INCOMING_CONNECTION_OK;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20 + szuint + szDWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionOKHandler;
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionOKHandler {
public:
    // execute packet's handler
    static void execute(GLIncomingConnectionOK* pPacket);
};

#endif
