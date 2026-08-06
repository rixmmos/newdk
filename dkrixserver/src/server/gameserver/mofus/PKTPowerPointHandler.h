/////////////////////////////////////////////////////////////////////////////
// Filename : PKTPowerPointHandler.h
// Desc		:
/////////////////////////////////////////////////////////////////////////////

#ifndef __PKT_POWER_POINT_HANDLER_H__
#define __PKT_POWER_POINT_HANDLER_H__

// include files
#include "MPacketHandler.h"

// class PKTPowerPointHandler
class PKTPowerPointHandler : public MPacketHandler {
public:
    
    MPacketID_t getID() const;

    
    void execute(MPlayer* pPlayer, MPacket* pPacket);
};

#endif
