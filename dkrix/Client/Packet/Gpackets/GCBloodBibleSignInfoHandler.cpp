//////////////////////////////////////////////////////////////////////
//
// Filename    : GCBloodBibleSignInfoHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
// include files
#include "GCBloodBibleSignInfo.h"
#include "PacketFunction.h"
//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCBloodBibleSignInfoHandler::execute ( GCBloodBibleSignInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY 
	
#ifdef __GAME_CLIENT__
	SetBloodBibleSlot(pPacket->getSignInfo());
	//cout << pPacket->toString() << endl;
#endif

	 __END_CATCH
}
