//////////////////////////////////////////////////////////////////////
//
// Filename    : CLSelectServer.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CL_SELECT_SERVER_H__
#define __CL_SELECT_SERVER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CLSelectServer;
//
//////////////////////////////////////////////////////////////////////

class CLSelectServer : public Packet {
public:
    CLSelectServer() {};
    virtual ~CLSelectServer() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_SELECT_SERVER;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szServerGroupID;
    }

    // get packet name
    string getPacketName() const {
        return "CLSelectServer";
    }

    // get / set ServerGroupID
    ServerGroupID_t getServerGroupID() const {
        return m_ServerGroupID;
    }
    void setServerGroupID(ServerGroupID_t ServerGroupID) {
        m_ServerGroupID = ServerGroupID;
    }

    // get packet's debug string
    string toString() const {
        return "CLSelectServer";
    }

private:
    ServerGroupID_t m_ServerGroupID;
};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectServerFactory;
//
// Factory for CLSelectServer
//
//////////////////////////////////////////////////////////////////////

class CLSelectServerFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLSelectServer();
    }

    // get packet name
    string getPacketName() const {
        return "CLSelectServer";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_SELECT_SERVER;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szServerGroupID;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLSelectServerHandler;
//
//////////////////////////////////////////////////////////////////////

class CLSelectServerHandler {
public:
    // execute packet's handler
    static void execute(CLSelectServer* pPacket, Player* player);
};

#endif
