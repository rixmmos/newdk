//////////////////////////////////////////////////////////////////////
//
// Filename    : GCPhoneConnectedHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCPhoneConnected.h"
#include "UserInformation.h"
#include "ClientDef.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCPhoneConnectedHandler::execute ( GCPhoneConnected * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY


	int pcsNumber = pPacket->getPhoneNumber();
	int slot = pPacket->getSlotID();

	
	g_pUserInformation->OtherPCSNumber[ slot ] = pcsNumber;
	g_pUserInformation->PCSUserName[ slot ] = pPacket->getName().c_str();

	
	char pName[128];
	strcpy(pName, pPacket->getName().c_str());

//	UI_OnLinePCS(pName, pcsNumber);

	__END_CATCH
}
