//----------------------------------------------------------------------
//
// Filename    : GTOAcknowledgement.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __GTO_ACKNOWLEDGEMENT_H__
#define __GTO_ACKNOWLEDGEMENT_H__

// include files
#include <string>

#include "DatagramPacket.h"
#include "PacketFactory.h"

const string Message = "One Server to rule them all, One Server to find them all, One Server to bring them all, and in "
                       "the network bind them.";

//----------------------------------------------------------------------
//
// class GTOAcknowledgement;
//



//
// *CAUTION*
//






//
//----------------------------------------------------------------------

class GTOAcknowledgement : public DatagramPacket {
public:
    GTOAcknowledgement() {
        m_Message = Message;
    }

    
    void read(Datagram& iDatagram) throw(ProtocolException, Error);

    
    void write(Datagram& oDatagram) const throw(ProtocolException, Error);

    // execute packet's handler
    void execute(Player* pPlayer) throw(ProtocolException, Error);

    // get packet id
    PacketID_t getPacketID() const throw() {
        return PACKET_GTO_ACKNOWLEDGEMENT;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const throw() {
        return szBYTE + szBYTE + m_Message.size() + szBYTE + m_ServerIP.size() + szuint;
    }

    // get packet name
    string getPacketName() const throw() {
        return "GTOAcknowledgement";
    }

    // get packet's debug string
    string toString() const throw();

public:
    string getServerIP() const throw() {
        return m_ServerIP;
    }
    void setServerIP(const string& ServerIP) throw() {
        m_ServerIP = ServerIP;
    }

    string getMessage() const {
        return m_Message;
    }

    BYTE getServerType() const throw() {
        return m_ServerType;
    }
    void setServerType(BYTE ServerType) throw() {
        m_ServerType = ServerType;
    }

    uint getUDPPort() const throw() {
        return m_udpPort;
    }
    void setUDPPort(uint port) throw() {
        m_udpPort = port;
    }


private:
    BYTE m_ServerType;
    string m_Message;
    string m_ServerIP;
    uint m_udpPort;
};


//////////////////////////////////////////////////////////////////////
//
// class GTOAcknowledgementFactory;
//
// Factory for GTOAcknowledgement
//
//////////////////////////////////////////////////////////////////////

class GTOAcknowledgementFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() throw() {
        return new GTOAcknowledgement();
    }

    // get packet name
    string getPacketName() const throw() {
        return "GTOAcknowledgement";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_GTO_ACKNOWLEDGEMENT;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + szBYTE + 1024 + szBYTE + 15 + szuint; // client ip
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GTOAcknowledgementHandler;
//
//////////////////////////////////////////////////////////////////////

class GTOAcknowledgementHandler {
public:
    // execute packet's handler
    static void execute(GTOAcknowledgement* pPacket) throw(ProtocolException, Error);
};

#endif
