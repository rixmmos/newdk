/////////////////////////////////////////////////////////////////////////////
// Filename : PKTLogoutHandler.h
// Desc		:
/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_LOGOUT_HANDLER_H__
#define __PKT_LOGOUT_HANDLER_H__

// include files
#include "MPacketHandler.h"

// class PKTLogoutHandler
class PKTLogoutHandler : public MPacketHandler {
public:
    
    MPacketID_t getID() const;

    
    void execute(GameServerPlayer* pPlayer, MPacket* pPacket);
};

#endif
