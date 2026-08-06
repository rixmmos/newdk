//////////////////////////////////////////////////////////////////////
//
// Filename    : LCLoginOK.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_LOGIN_OK_H__
#define __LC_LOGIN_OK_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCLoginOK;
//

//
//////////////////////////////////////////////////////////////////////

class LCLoginOK : public Packet {
public:
    LCLoginOK() : m_LastDays(0xffff) {}
    ~LCLoginOK() {};

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_LOGIN_OK;
    }

    // get packet body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szBYTE + szBYTE + szBYTE + szWORD;
    }

    // get packet's name
    string getPacketName() const {
        return "LCLoginOK";
    }

    // get / set GoreLevel
    bool isAdult() const {
        return m_isAdult;
    }
    void setAdult(bool isAdult) {
        m_isAdult = isAdult;
    }

    bool isFamily() const {
        return m_bFamily;
    }
    void setFamily(bool isFamily) {
        m_bFamily = isFamily;
    }

    BYTE getStat() const {
        return m_Stat;
    }
    void setStat(BYTE Stat) {
        m_Stat = Stat;
    }

    WORD getLastDays() const {
        return m_LastDays;
    }
    void setLastDays(WORD LastDays) {
        m_LastDays = LastDays;
    }

    // get packet's debug string
    string toString() const {
        return "LCLoginOK";
    }

private:
    
    
    
    bool m_isAdult;

    
    bool m_bFamily;

    
    BYTE m_Stat;

    WORD m_LastDays;
};


//////////////////////////////////////////////////////////////////////
//
// class LCLoginOKFactory;
//
// Factory for LCLoginOK
//
//////////////////////////////////////////////////////////////////////

class LCLoginOKFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCLoginOK();
    }

    // get packet name
    string getPacketName() const {
        return "LCLoginOK";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_LOGIN_OK;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + szBYTE + szBYTE + szWORD;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCLoginOKHandler;
//
//////////////////////////////////////////////////////////////////////

class LCLoginOKHandler {
public:
    // execute packet's handler
    static void execute(LCLoginOK* pPacket, Player* pPlayer);
};

#endif
