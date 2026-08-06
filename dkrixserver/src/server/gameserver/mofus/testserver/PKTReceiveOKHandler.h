/////////////////////////////////////////////////////////////////////////////
// Filename : PKTReceiveOKHandler.h
// Desc		:
/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_RECEIVE_OK_HANDLER_H__
#define __PKT_RECEIVE_OK_HANDLER_H__

// include files
#include "MPacketHandler.h"

// class PKTReceiveOKHandler
class PKTReceiveOKHandler : public MPacketHandler {
public:
    
    MPacketID_t getID() const;

    
    void execute(GameServerPlayer* pPlayer, MPacket* pPacket);
};

#endif
