//----------------------------------------------------------------------
//
// Filename    : LGKickCharacter.h
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

#ifndef __LG_KICK_CHARACTER_H__
#define __LG_KICK_CHARACTER_H__

// include files
#include "DatagramPacket.h"
#include "PacketFactory.h"


//----------------------------------------------------------------------
//
// class LGKickCharacter;
//


//

//
//----------------------------------------------------------------------

class LGKickCharacter : public DatagramPacket {
public:
    LGKickCharacter() {};
    ~LGKickCharacter() {};
    
    void read(Datagram& iDatagram);

    
    void write(Datagram& oDatagram) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LG_KICK_CHARACTER;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_PCName.size() // PC name
               + szuint;
    }

    // get packet name
    string getPacketName() const {
        return "LGKickCharacter";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set pcName
    string getPCName() const {
        return m_PCName;
    }
    void setPCName(const string& pcName) {
        m_PCName = pcName;
    }

    uint getID() const {
        return m_ID;
    }
    void setID(uint id) {
        m_ID = id;
    }

private:
    // PC name
    string m_PCName;

    uint m_ID;
};


//////////////////////////////////////////////////////////////////////
//
// class LGKickCharacterFactory;
//
// Factory for LGKickCharacter
//
//////////////////////////////////////////////////////////////////////

class LGKickCharacterFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LGKickCharacter();
    }

    // get packet name
    string getPacketName() const {
        return "LGKickCharacter";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LG_KICK_CHARACTER;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20 // PC name
               + szuint;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LGKickCharacterHandler;
//
//////////////////////////////////////////////////////////////////////

class LGKickCharacterHandler {
public:
    // execute packet's handler
    static void execute(LGKickCharacter* pPacket);
};

#endif
