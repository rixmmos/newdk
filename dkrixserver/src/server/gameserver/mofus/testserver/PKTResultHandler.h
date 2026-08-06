/////////////////////////////////////////////////////////////////////////////
// Filename : PKTResultHandler.h
// Desc		:
/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_RESULT_HANDLER_H__
#define __PKT_RESULT_HANDLER_H__

// include files
#include "MPacketHandler.h"

// class PKTResultHandler
class PKTResultHandler : public MPacketHandler {
public:
    
    MPacketID_t getID() const;

    
    void execute(GameServerPlayer* pPlayer, MPacket* pPacket);
};

#endif
