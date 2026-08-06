//////////////////////////////////////////////////////////////////////
//
// Filename    : CGRangerSay.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_RANGER_SAY_H__
#define __CG_RANGER_SAY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGRangerSay;
//

//
//////////////////////////////////////////////////////////////////////

class CGRangerSay : public Packet {
public:
    CGRangerSay() {};
    ~CGRangerSay() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_RANGER_SAY;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Message.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGRangerSay";
    }

    // get packet's debug string
    string toString() const;

    // get/set chatting message
    string getMessage() const {
        return m_Message;
    }
    void setMessage(const string& msg) {
        m_Message = msg;
    }


private:
    // chatting message
    string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class CGRangerSayFactory;
//
// Factory for CGRangerSay
//
//////////////////////////////////////////////////////////////////////

class CGRangerSayFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGRangerSay();
    }

    // get packet name
    string getPacketName() const {
        return "CGRangerSay";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_RANGER_SAY;
    }

    // get packet's max body size
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGRangerSayHandler;
//
//////////////////////////////////////////////////////////////////////

class CGRangerSayHandler {
public:
    // execute packet's handler
    static void execute(CGRangerSay* pPacket, Player* pPlayer);
};

#endif
