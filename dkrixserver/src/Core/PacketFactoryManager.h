//////////////////////////////////////////////////////////////////////
//
// Filename    : PacketFactoryManager.h
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __PACKET_FACTORY_MANAGER_H__
#define __PACKET_FACTORY_MANAGER_H__

// include files
#include "Packet.h"
#include "PacketFactory.h"


//////////////////////////////////////////////////////////////////////
//
// class PacketFactoryManager
//
//////////////////////////////////////////////////////////////////////

class PacketFactoryManager {
public:
    // constructor
    PacketFactoryManager();

    // destructor
    ~PacketFactoryManager() noexcept;

    
    
    void init();

    
    void addFactory(PacketFactory* pFactory);

    
    Packet* createPacket(PacketID_t packetID);

    
    string getPacketName(PacketID_t packetID);

    
    PacketSize_t getPacketMaxSize(PacketID_t packetID);

    // get debug string
    string toString() const;

private:
    
    PacketFactory** m_Factories;

    
    ushort m_Size;
};

extern PacketFactoryManager* g_pPacketFactoryManager;

#endif
