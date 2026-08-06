//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddVampirePortalHandler.cpp
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////
#include "Client_PCH.h"
#include "GCAddVampirePortal.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void GCAddVampirePortalHandler::execute ( GCAddVampirePortal * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY 
		
#ifdef __GAME_CLIENT__


	int serverID = pPacket->getObjectID();

	const char* ownerName = pPacket->getOwnerID().c_str();
	TYPE_SECTORPOSITION sX = pPacket->getX();
	TYPE_SECTORPOSITION sY = pPacket->getY();

	DWORD delayFrame = ConvertDurationToFrame( pPacket->getDuration() );

	int zoneID = pPacket->getTargetZoneID();
	TYPE_SECTORPOSITION zoneX = pPacket->getTargetX();
	TYPE_SECTORPOSITION zoneY = pPacket->getTargetY();

	
	bool bStartFromMainNode = (pPacket->getCreateFlag()==0);

	AddVampirePortal( serverID, 
						ownerName, 
						sX, sY,	
						delayFrame,	
						zoneID,	zoneX, zoneY,
						bStartFromMainNode );

#endif

	__END_CATCH
}
