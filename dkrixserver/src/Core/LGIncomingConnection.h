//----------------------------------------------------------------------
//
// Filename    : LGIncomingConnection.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __LG_INCOMING_CONNECTION_H__
#define __LG_INCOMING_CONNECTION_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class LGIncomingConnection;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class LGIncomingConnection : public DatagramPacket {
public:
    LGIncomingConnection() {};
    ~LGIncomingConnection() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LG_INCOMING_CONNECTION;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return +szBYTE + m_PlayerID.size()   // Player ID
               + szBYTE + m_PCName.size()    // PC name
               + szBYTE + m_ClientIP.size(); // client ip
    }

    // get packet name
    string getPacketName() const {
        return "LGIncomingConnection";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set playerID
    string getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(const string& playerID) {
        m_PlayerID = playerID;
    }

    // get/set pcName
    string getPCName() const {
        return m_PCName;
    }
    void setPCName(const string& pcName) {
        m_PCName = pcName;
    }

    // get/set client ip
    string getClientIP() const {
        return m_ClientIP;
    }
    void setClientIP(const string& ip) {
        m_ClientIP = ip;
    }

private:
    // Player ID
    string m_PlayerID;

    // PC name
    string m_PCName;

    
    string m_ClientIP;
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionFactory;
//
// Factory for LGIncomingConnection
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LGIncomingConnection();
    }

    // get packet name
    string getPacketName() const {
        return "LGIncomingConnection";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LG_INCOMING_CONNECTION;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return +szBYTE + 20   // creature name
               + szBYTE + 20  // PC name
               + szBYTE + 15; // client ip
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionHandler;
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionHandler {
public:
    // execute packet's handler
    static void execute(LGIncomingConnection* pPacket);
};

#endif
