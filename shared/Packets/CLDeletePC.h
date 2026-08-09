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
    // Base PacketFactory declares these four with throw() specs on the
    // client tree; narrowing to throw() here also satisfies the server
    // tree's unconstrained base. See CLGetWorldList.h (Phase 12 pilot)
    // for the precedent.
    Packet* createPacket() throw() {
        return new CLDeletePC();
    }

    // get packet name
    string getPacketName() const throw() {
        return "CLDeletePC";
    }

    // get packet id
    PacketID_t getPacketID() const throw() {
        return Packet::PACKET_CL_DELETE_PC;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const throw() {
        return szBYTE + 20 + szSlot + szBYTE + 20;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class CLDeletePCHandler;
//
//////////////////////////////////////////////////////////////////////

// Server-only: CLDeletePCHandler::execute has no client-side definition
// or use. Guarded (matching the client Cpackets copy's existing guard)
// since no CGHandlersStub.cpp-style client stub exists for this family.
#ifndef __GAME_CLIENT__
class CLDeletePCHandler {
public:
    // execute packet's handler
    static void execute(CLDeletePC* pPacket, Player* pPlayer);
};
#endif

#endif
