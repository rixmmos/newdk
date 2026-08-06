//////////////////////////////////////////////////////////////////////
//
// Filename    : RCConnectVerifyHandler.cc
// Written By  : crazydog
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "RCConnectVerify.h"
#include "../RequestClientPlayer.h"
//#include "CRRequest.h"
//#include "MZone.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void RCConnectVerifyHandler::execute ( RCConnectVerify * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	RequestClientPlayer* pRequestClientPlayer = dynamic_cast<RequestClientPlayer*>( pPlayer );

	if (pRequestClientPlayer!=NULL)
	{
		switch (pPacket->getCode())
		{
			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			case RC_CONNECT_OK :
				pRequestClientPlayer->setPlayerStatus( CPS_REQUEST_CLIENT_NORMAL );

				
				
				 

			break;

			//---------------------------------------------------------------
			
			//---------------------------------------------------------------
			case RC_CONNECT_REJECT :
			case RC_CONNECT_WRONG :
				pRequestClientPlayer->setPlayerStatus( CPS_END_SESSION );
				//pRequestClientPlayer->disconnect(UNDISCONNECTED);
				throw DisconnectException("Reject or Wrong");
			break;
		}
	}

#endif

	__END_CATCH
}
