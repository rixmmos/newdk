//--------------------------------------------------------------------------------
//
// Filename    : CLQueryPlayerID.h
// Written By  : Reiot
// Description :
//
//--------------------------------------------------------------------------------

#ifndef __CL_QUERY_PLAYER_ID_H__
#define __CL_QUERY_PLAYER_ID_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//--------------------------------------------------------------------------------
//
// class CLQueryPlayerID;
//


//
//--------------------------------------------------------------------------------

class CLQueryPlayerID : public Packet {
public:
    CLQueryPlayerID() {};
    virtual ~CLQueryPlayerID() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_QUERY_PLAYER_ID;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_PlayerID.size();
    }

    // get packet name
    string getPacketName() const {
        return "CLQueryPlayerID";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set player's id
    string getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(const string& playerID) {
        m_PlayerID = playerID;
    }

private:
    
    string m_PlayerID;
};


//--------------------------------------------------------------------------------
//
// class CLQueryPlayerIDFactory;
//
// Factory for CLQueryPlayerID
//
//--------------------------------------------------------------------------------

class CLQueryPlayerIDFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLQueryPlayerID();
    }

    // get packet name
    string getPacketName() const {
        return "CLQueryPlayerID";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_QUERY_PLAYER_ID;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20;
    }
};


//--------------------------------------------------------------------------------
//
// class CLQueryPlayerIDHandler;
//
//--------------------------------------------------------------------------------

class CLQueryPlayerIDHandler {
public:
    // execute packet's handler
    static void execute(CLQueryPlayerID* pPacket, Player* pPlayer);
};

#endif
