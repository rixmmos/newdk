//----------------------------------------------------------------------
//
// Filename    : LCDeletePCErrorHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LCDeletePCError.h"

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void LCDeletePCErrorHandler::execute(LCDeletePCError* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_CLIENT__

            cout
        << "Fail to deletePC... (" << pPacket->toString() << ")" << endl;
    throw ProtocolException("DeletePC Failed!");

#endif

    __END_DEBUG_EX __END_CATCH
}
