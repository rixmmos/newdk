//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPartySay.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_PARTY_SAY_H__
#define __GC_PARTY_SAY_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class GCPartySay;
//


//
//////////////////////////////////////////////////////////////////////

class GCPartySay : public Packet {
public:
    GCPartySay() {};
    ~GCPartySay() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_PARTY_SAY;
    }

    // get packet's body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Name.size() + szDWORD + szBYTE + m_Message.size();
    }

    // get packet's name
    string getPacketName() const {
        return "GCPartySay";
    }

    // get packet's debug string
    string toString() const;


public:
    string getName() const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

    DWORD getColor() const {
        return m_Color;
    }
    void setColor(DWORD color) {
        m_Color = color;
    }

    string getMessage() const {
        return m_Message;
    }
    void setMessage(const string& msg) {
        m_Message = msg;
    }

private:
    string m_Name;
    DWORD m_Color;
    string m_Message;
};


//////////////////////////////////////////////////////////////////////
//
// class GCPartySayFactory;
//
// Factory for GCPartySay
//
//////////////////////////////////////////////////////////////////////

class GCPartySayFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCPartySay();
    }

    // get packet name
    string getPacketName() const {
        return "GCPartySay";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_PARTY_SAY;
    }

    // get packet's max body size
    // *OPTIMIZATION HINT*
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20 + szDWORD + szBYTE + 128;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCPartySayHandler;
//
//////////////////////////////////////////////////////////////////////

class GCPartySayHandler {
public:
    // execute packet's handler
    static void execute(GCPartySay* pPacket, Player* pPlayer);
};

#endif
