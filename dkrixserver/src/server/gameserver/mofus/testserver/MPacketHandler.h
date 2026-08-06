/////////////////////////////////////////////////////////////////////////////
// Filename : MPacketHandler.h

/////////////////////////////////////////////////////////////////////////////

#ifndef __MPACKET_HANDLER_H__
#define __MPACKET_HANDLER_H__

// include files
#include "MPacket.h"

// forward declaration
class GameServerPlayer;

// class MPacketHandler
class MPacketHandler {
public:
    virtual MPacketID_t getID() const = 0;

    virtual void execute(GameServerPlayer* pPlayer, MPacket* pPacket) = 0;
};

#endif
