//////////////////////////////////////////////////////////////////////
//
// Filename    : SerialDatagramPacket.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __SERIAL_DATAGRAM_PACKET_H__
#define __SERIAL_DATAGRAM__PACKET_H__

// include files
#include "Packet.h"
#include "SerialDatagram.h"
#include "SocketAPI.h"


//////////////////////////////////////////////////////////////////////
//
// class SerialDatagramPacket;
//




//
//////////////////////////////////////////////////////////////////////

class SerialDatagramPacket : public Packet {
public:
    // destructor
    virtual ~SerialDatagramPacket() {}

    
    
    virtual void read(SocketInputStream& iStream) {
        throw ProtocolException("datagram packet from TCP socket");
    }

    
    virtual void read(SerialDatagram& iDatagram) = 0;

    
    
    virtual void write(SocketOutputStream& oStream) const {
        throw Error("cannot write datagram-packet to TCP-socket-stream");
    }

    
    virtual void write(SerialDatagram& oDatagram) const = 0;

    // execute packet's handler
    virtual void execute(Player* pPlayer) = 0;

    // get packet's SerialDatagramPacketID
    virtual PacketID_t getPacketID() const = 0;

    // get packet's body size
    virtual PacketSize_t getPacketSize() const = 0;

    // get packet's name
    virtual string getPacketName() const = 0;

    // get packet's debug string
    virtual string toString() const = 0;

    // get/set host
    string getHost() const {
        return m_Host;
    }
    void setHost(string host) {
        m_Host = host;
    }

    // get/set port
    uint getPort() const {
        return m_Port;
    }
    void setPort(uint port) {
        m_Port = port;
    }

    // get/set serial number
    uint getSerial() const {
        return m_Serial;
    }
    void setSerial(uint serial) {
        m_Serial = serial;
    }


protected:
    // sender's host
    string m_Host;

    // sender's port
    uint m_Port;

    // Packet Serial Number
    uint m_Serial;
};

#endif
