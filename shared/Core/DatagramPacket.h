//////////////////////////////////////////////////////////////////////
//
// Filename    : DatagramPacket.h
// Description : Shared canonical UDP packet base class
//
//////////////////////////////////////////////////////////////////////

#ifndef __DATAGRAM_PACKET_H__
#define __DATAGRAM_PACKET_H__

// include files
#include "Datagram.h"
#include "Packet.h"
#include "SocketAPI.h"

class DatagramPacket : public Packet {
public:
    virtual ~DatagramPacket() noexcept {}

    // Datagram packets must not arrive over the TCP packet stream.
    virtual void read(SocketInputStream& iStream) {
        throw ProtocolException("datagram packet from TCP socket");
    }

    virtual void read(Datagram& iDatagram) = 0;

    // Datagram packets likewise cannot be serialized into the TCP stream.
    virtual void write(SocketOutputStream& oStream) const {
        throw Error("cannot write datagram-packet to TCP-socket-stream");
    }

    virtual void write(Datagram& oDatagram) const = 0;

    virtual void execute(Player* pPlayer) = 0;
    virtual PacketID_t getPacketID() const = 0;
    virtual PacketSize_t getPacketSize() const = 0;

#if !defined(__GAME_CLIENT__) || defined(__DEBUG_OUTPUT__)
    virtual string getPacketName() const = 0;
    virtual string toString() const = 0;
#endif

    const string& getHost() const { return m_Host; }
    void setHost(const string& host) { m_Host = host; }

    uint getPort() const { return m_Port; }
    void setPort(uint port) { m_Port = port; }

protected:
    string m_Host;
    uint m_Port;
};

#endif
