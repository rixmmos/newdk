//----------------------------------------------------------------------
//
// Filename    : GLIncomingConnection.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __GL_INCOMING_CONNECTION_H__
#define __GL_INCOMING_CONNECTION_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class GLIncomingConnection;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class GLIncomingConnection : public DatagramPacket {
public:
    GLIncomingConnection() {};
    ~GLIncomingConnection() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GL_INCOMING_CONNECTION;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return +szBYTE + m_PlayerID.size()   // Player ID
               + szBYTE + m_ClientIP.size(); // client ip
    }

    // get packet name
    string getPacketName() const {
        return "GLIncomingConnection";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set playerID
    const string& getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(const string& playerID) {
        m_PlayerID = playerID;
    }

    // get/set client ip
    const string& getClientIP() const {
        return m_ClientIP;
    }
    void setClientIP(const string& ip) {
        m_ClientIP = ip;
    }

private:
    // Player ID
    string m_PlayerID;

    
    string m_ClientIP;
};


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionFactory;
//
// Factory for GLIncomingConnection
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GLIncomingConnection();
    }

    // get packet name
    string getPacketName() const {
        return "GLIncomingConnection";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GL_INCOMING_CONNECTION;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return +szBYTE + 20   // creature name
               + szBYTE + 15; // client ip
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GLIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class GLIncomingConnectionHandler {
public:
    // execute packet's handler
    static void execute(GLIncomingConnection* pPacket);
};

#endif
