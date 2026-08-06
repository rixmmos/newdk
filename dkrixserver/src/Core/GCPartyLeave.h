//////////////////////////////////////////////////////////////////////////////
// Filename    : GCPartyLeave.h
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __GC_PARTY_LEAVE_H__
#define __GC_PARTY_LEAVE_H__

#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////////////
// class GCPartyLeave;
//////////////////////////////////////////////////////////////////////////////

class GCPartyLeave : public Packet {
public:
    GCPartyLeave() {};
    ~GCPartyLeave() {};
    void read(SocketInputStream& iStream);
    void write(SocketOutputStream& oStream) const;
    void execute(Player* pPlayer);
    PacketID_t getPacketID() const {
        return PACKET_GC_PARTY_LEAVE;
    }
    PacketSize_t getPacketSize() const {
        return szBYTE * 2 + m_Expeller.size() + m_Expellee.size();
    }
    string getPacketName() const {
        return "GCPartyLeave";
    }
    string toString() const;

public:
    string getExpeller(void) const {
        return m_Expeller;
    }
    void setExpeller(const string& name) {
        m_Expeller = name;
    }

    string getExpellee(void) const {
        return m_Expellee;
    }
    void setExpellee(const string& name) {
        m_Expellee = name;
    }

private:
    
    
    
    
    
    string m_Expeller; 
    string m_Expellee; 
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyLeaveFactory;
//////////////////////////////////////////////////////////////////////////////

class GCPartyLeaveFactory : public PacketFactory {
public:
    Packet* createPacket() {
        return new GCPartyLeave();
    }
    string getPacketName() const {
        return "GCPartyLeave";
    }
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_PARTY_LEAVE;
    }
    PacketSize_t getPacketMaxSize() const {
        return szBYTE * 2 + 20;
    }
};


//////////////////////////////////////////////////////////////////////////////
// class GCPartyLeaveHandler;
//////////////////////////////////////////////////////////////////////////////

class GCPartyLeaveHandler {
public:
    static void execute(GCPartyLeave* pPacket, Player* pPlayer);
};

#endif
