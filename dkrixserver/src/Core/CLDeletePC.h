//----------------------------------------------------------------------
//
// Filename    : CLDeletePC.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//----------------------------------------------------------------------

#ifndef __CL_DELETE_PC_H__
#define __CL_DELETE_PC_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"

//----------------------------------------------------------------------
//
// class CLDeletePC;
//

//
//----------------------------------------------------------------------

class CLDeletePC : public Packet {
public:
    CLDeletePC() {};
    virtual ~CLDeletePC() {};
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_CL_DELETE_PC;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const {
        return szBYTE + m_Name.size() + szSlot + szBYTE + m_SSN.size();
    }

    // get packet's name
    string getPacketName() const {
        return "CLDeletePC";
    }

    // get packet's debug string
    string toString() const;

    // get/set name
    string getName() const {
        return m_Name;
    }
    void setName(string name) {
        m_Name = name;
    }

    // get/set Slot
    Slot getSlot() const {
        return m_Slot;
    }
    void setSlot(Slot slot) {
        m_Slot = slot;
    }

    // get/set SSN
    string getSSN() const {
        return m_SSN;
    }
    void setSSN(const string& SSN) {
        m_SSN = SSN;
    }

private:
    // PC name
    string m_Name;

    // Slot
    Slot m_Slot;

    
    string m_SSN;
};


//////////////////////////////////////////////////////////////////////
//
// class CLDeletePCFactory;
//
// Factory for CLDeletePC
//
//////////////////////////////////////////////////////////////////////

class CLDeletePCFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new CLDeletePC();
    }

    // get packet name
    string getPacketName() const {
        return "CLDeletePC";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_CL_DELETE_PC;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        return szBYTE + 20 + szSlot + szBYTE + 20;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLDeletePCHandler;
//
//////////////////////////////////////////////////////////////////////

class CLDeletePCHandler {
public:
    // execute packet's handler
    static void execute(CLDeletePC* pPacket, Player* pPlayer);
};

#endif
