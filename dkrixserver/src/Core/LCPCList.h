//////////////////////////////////////////////////////////////////////
//
// Filename    : LCPCList.h
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __LC_PC_LIST_H__
#define __LC_PC_LIST_H__

// include files
#include "PCInfo.h"
#include "PCOustersInfo.h"
#include "PCSlayerInfo.h"
#include "PCVampireInfo.h"
#include "Packet.h"
#include "PacketFactory.h"

//////////////////////////////////////////////////////////////////////
//
// class LCPCList;
//
//////////////////////////////////////////////////////////////////////

class LCPCList : public Packet {
public:
    // constructor
    
    LCPCList();

    // destructor
    
    ~LCPCList();

    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // execute packet's handler
    void execute(Player* pPlayer);

    // get packet id
    PacketID_t getPacketID() const {
        return PACKET_LC_PC_LIST;
    }

    // get packet's body size
    PacketSize_t getPacketSize() const;

    // get packet's name
    string getPacketName() const {
        return "LCPCList";
    }

    // get packet's debug string
    string toString() const;

public:
    // get/set pc info
    PCInfo* getPCInfo(Slot slot) const {
        if (m_pPCInfos[slot] == NULL)
            throw NoSuchElementException("no such PC exist in that slot");

        return m_pPCInfos[slot];
    }

    void setPCInfo(Slot slot, PCInfo* pPCInfo) {
        if (m_pPCInfos[slot] != NULL)
            throw DuplicatedException("PCInfo duplicated.");
        m_pPCInfos[slot] = pPCInfo;
    }

    bool isAgree() const {
        return (m_Agree ? true : false);
    }
    void setAgree(bool agree) {
        m_Agree = agree;
    }

private:
    
    PCInfo* m_pPCInfos[SLOT_MAX];

    
    BYTE m_Agree;
};


//////////////////////////////////////////////////////////////////////
//
// class LCPCListFactory;
//
// Factory for LCPCList
//
//////////////////////////////////////////////////////////////////////

class LCPCListFactory : public PacketFactory {
public:
    // create packet
    Packet* createPacket() {
        return new LCPCList();
    }

    // get packet name
    string getPacketName() const {
        return "LCPCList";
    }

    // get packet id
    PacketID_t getPacketID() const {
        return Packet::PACKET_LC_PC_LIST;
    }

    // get packet's max body size
    PacketSize_t getPacketMaxSize() const {
        
        
        return PCSlayerInfo::getMaxSize() * SLOT_MAX + SLOT_MAX + szBYTE;
    }
};


//////////////////////////////////////////////////////////////////////
//
// class LCPCListHandler;
//
//////////////////////////////////////////////////////////////////////

class LCPCListHandler {
public:
    // execute packet's handler
    static void execute(LCPCList* pPacket, Player* pPlayer);
};

#endif
