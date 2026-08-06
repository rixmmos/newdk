//////////////////////////////////////////////////////////////////////
//
// Filename    : LGIncomingConnectionError.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LG_INCOMING_CONNECTION_ERROR_H__
#define __LG_INCOMING_CONNECTION_ERROR_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionError;
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionError : public DatagramPacket {
public:
    LGIncomingConnectionError() {};
    ~LGIncomingConnectionError() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LG_INCOMING_CONNECTION_ERROR;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Message.size();
    }

    // get packet name
    string getPacketName() const {
        return "LGIncomingConnectionError";
    }

    // get packet's debug string
    string toString() const;

    // get/set error message
    string getMessage() const {
        return m_Message;
    }
    void setMessage(string message) {
        m_Message = message;
    }

    // get/set player id
    string getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(string playerID) {
        m_PlayerID = playerID;
    }

private:
    // error message
    string m_Message;

    // player id
    string m_PlayerID;
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionErrorFactory;
//
// Factory for LGIncomingConnectionError
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionErrorFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LGIncomingConnectionError();
    }

    // get packet name
    string getPacketName() const {
        return "LGIncomingConnectionError";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LG_INCOMING_CONNECTION_ERROR;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGIncomingConnectionErrorHandler;
//
//////////////////////////////////////////////////////////////////////

class LGIncomingConnectionErrorHandler {
public:
    // execute packet's handler
    static void execute(LGIncomingConnectionError* pPacket);
};

#endif
