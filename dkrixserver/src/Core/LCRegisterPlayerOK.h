//////////////////////////////////////////////////////////////////////
//
// Filename    : LCRegisterPlayerOK.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_REGISTER_PLAYER_OK_H__
#define __LC_REGISTER_PLAYER_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCRegisterPlayerOK;
//

//
//////////////////////////////////////////////////////////////////////

class LCRegisterPlayerOK : public Packet {
public:
    LCRegisterPlayerOK() {};
    ~LCRegisterPlayerOK() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_REGISTER_PLAYER_OK;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szBYTE + m_GroupName.size() + szBYTE;
    }

    // get packet's name
    string getPacketName() const {
        return "LCRegisterPlayerOK";
    }

    // get / set Groupname
    string getGroupName() const {
        return m_GroupName;
    }
    void setGroupName(const string& GroupName) {
        m_GroupName = GroupName;
    }

    // get / set GoreLevel
    bool isAdult() const {
        return m_isAdult;
    }
    void setAdult(bool isAdult) {
        m_isAdult = isAdult;
    }


    // get packet's debug string
    string toString() const {
        return "LCRegisterPlayerOK";
    }

private:
    
    string m_GroupName;

    
    
    
    bool m_isAdult;
};


//////////////////////////////////////////////////////////////////////
//
// class LCRegisterPlayerOKFactory;
//
// Factory for LCRegisterPlayerOK
//
//////////////////////////////////////////////////////////////////////

class LCRegisterPlayerOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCRegisterPlayerOK();
    }

    // get packet name
    string getPacketName() const {
        return "LCRegisterPlayerOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_REGISTER_PLAYER_OK;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20 + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCRegisterPlayerOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCRegisterPlayerOKHandler {
public:
    // execute packet's handler
    static void execute(LCRegisterPlayerOK* pPacket, Player* pPlayer);
};

#endif
