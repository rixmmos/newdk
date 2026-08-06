//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPetInfoHandler.cpp
// Written By  : 
//
//////////////////////////////////////////////////////////////////////

// include 
#include "Client_PCH.h"
#include "GCPetInfo.h"
#include "MFakeCreature.h"
#include "PacketFunction.h"

extern void SetPetInfo(PetInfo* pPetInfo, TYPE_OBJECTID objectID);

static void TraceGameEntryFlow(const char* step)
{
	(void)step;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
void GCPetInfoHandler::execute ( GCPetInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
	TraceGameEntryFlow("GCPetInfoHandler begin");
	SetPetInfo(pPacket->getPetInfo(), pPacket->getObjectID());
	TraceGameEntryFlow("GCPetInfoHandler end");
#endif

	__END_CATCH
}
