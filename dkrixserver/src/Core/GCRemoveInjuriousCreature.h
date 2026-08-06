//////////////////////////////////////////////////////////////////////
//
// Filename    : GCRemoveInjuriousCreature.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GC_REMOVE_INJURIOUS_CREATURE_H__
#define __GC_REMOVE_INJURIOUS_CREATURE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class GCRemoveInjuriousCreature;
//


//
//////////////////////////////////////////////////////////////////////

class GCRemoveInjuriousCreature : public Packet {
public:
    GCRemoveInjuriousCreature() {};
    ~GCRemoveInjuriousCreature() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_GC_REMOVE_INJURIOUS_CREATURE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Name.size();
    }

    // get packet name
    string getPacketName() const {
        return "GCRemoveInjuriousCreature";
    }

    // get packet's debug string
    string toString() const;

    // get/set Name
    string getName() const {
        return m_Name;
    }
    void setName(const string& Name) {
        m_Name = Name;
    }

private:
    string m_Name;
};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveInjuriousCreatureFactory;
//
// Factory for GCRemoveInjuriousCreature
//
//////////////////////////////////////////////////////////////////////

class GCRemoveInjuriousCreatureFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new GCRemoveInjuriousCreature();
    }

    // get packet name
    string getPacketName() const {
        return "GCRemoveInjuriousCreature";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_GC_REMOVE_INJURIOUS_CREATURE;
    }

    // get packet's max body size
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 10;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class GCRemoveInjuriousCreatureHandler;
//
//////////////////////////////////////////////////////////////////////

class GCRemoveInjuriousCreatureHandler {
public:
    // execute packet's handler
    static void execute(GCRemoveInjuriousCreature* pPacket, Player* pPlayer);
};

#endif
