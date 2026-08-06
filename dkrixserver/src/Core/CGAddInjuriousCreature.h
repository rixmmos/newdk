//////////////////////////////////////////////////////////////////////
//
// Filename    : CGAddInjuriousCreature.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __CG_ADD_INJURIOUS_CREATURE_H__
#define __CG_ADD_INJURIOUS_CREATURE_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class CGAddInjuriousCreature;
//


//
//////////////////////////////////////////////////////////////////////

class CGAddInjuriousCreature : public Packet {
public:
    CGAddInjuriousCreature() {};
    ~CGAddInjuriousCreature() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CG_ADD_INJURIOUS_CREATURE;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Name.size() + szBYTE + m_Message.size();
    }

    // get packet name
    string getPacketName() const {
        return "CGAddInjuriousCreature";
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
// class CGAddInjuriousCreatureFactory;
//
// Factory for CGAddInjuriousCreature
//
//////////////////////////////////////////////////////////////////////

class CGAddInjuriousCreatureFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CGAddInjuriousCreature();
    }

    // get packet name
    string getPacketName() const {
        return "CGAddInjuriousCreature";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CG_ADD_INJURIOUS_CREATURE;
    }

    // get packet's max body size
    
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 10;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CGAddInjuriousCreatureHandler;
//
//////////////////////////////////////////////////////////////////////

class CGAddInjuriousCreatureHandler {
public:
    // execute packet's handler
    static void execute(CGAddInjuriousCreature* pPacket, Player* pPlayer);
};

#endif
