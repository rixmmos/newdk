/////////////////////////////////////////////////////////////////////////////
// Filename	: MPacketManager.h
/////////////////////////////////////////////////////////////////////////////

#ifndef __MPACKET_MANAGER_H__
#define __MPACKET_MANAGER_H__

// include files
#include "MPacket.h"

// forward declaration
class MPlayer;
class MPacketHandler;


// class MPacketManager
class MPacketManager {
public:
    MPacketManager();
    ~MPacketManager();

public:
    void init();

public:
    
    void addCreator(MPacket* pPacket);

    
    void addHandler(MPacketHandler* pHandler);

public:
    
    MPacket* createPacket(MPacketID_t ID) const;

    
    void execute(MPlayer* pPlayer, MPacket* pPacket);

    
    bool hasHandler(MPacketID_t ID) const;

    
    MPacketSize_t getPacketSize(MPacketID_t ID) const;

private:
    
    struct IMPL;
    IMPL* m_pImpl;
};


// global variable
extern MPacketManager* g_pMPacketManager;

#endif
