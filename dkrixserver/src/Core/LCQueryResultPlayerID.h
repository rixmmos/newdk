//////////////////////////////////////////////////////////////////////
//
// Filename    : LCQueryResultPlayerID.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_QUERY_RESULT_PLAYER_ID_H__
#define __LC_QUERY_RESULT_PLAYER_ID_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCQueryResultPlayerID;
//


//
//////////////////////////////////////////////////////////////////////

class LCQueryResultPlayerID : public Packet {
public:
    // constructor
    LCQueryResultPlayerID() : m_bExist(false) {}
    ~LCQueryResultPlayerID() {};

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_QUERY_RESULT_PLAYER_ID;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szbool + szBYTE + m_PlayerID.size();
    }

    // get packet name
    string getPacketName() const {
        return "LCQueryResultPlayerID";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set player id
    string getPlayerID() const {
        return m_PlayerID;
    }
    void setPlayerID(const string& playerID) {
        m_PlayerID = playerID;
    }

    // get/set player id's existence
    bool isExist() const {
        return m_bExist;
    }
    void setExist(bool bExist = true) {
        m_bExist = bExist;
    }

private:
    // player id
    string m_PlayerID;

    // player id's existence
    bool m_bExist;
};


//////////////////////////////////////////////////////////////////////
//
// class LCQueryResultPlayerIDFactory;
//
// Factory for LCQueryResultPlayerID
//
//////////////////////////////////////////////////////////////////////

class LCQueryResultPlayerIDFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCQueryResultPlayerID();
    }

    // get packet name
    string getPacketName() const {
        return "LCQueryResultPlayerID";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_QUERY_RESULT_PLAYER_ID;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szbool + szBYTE + 20;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCQueryResultPlayerIDHandler;
//
//////////////////////////////////////////////////////////////////////

class LCQueryResultPlayerIDHandler {
public:
    // execute packet's handler
    static void execute(LCQueryResultPlayerID* pPacket, Player* pPlayer);
};

#endif
