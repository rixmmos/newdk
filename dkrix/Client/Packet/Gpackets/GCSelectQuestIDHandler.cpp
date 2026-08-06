
//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSelectQuestIDHandler.cc
// Written By  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCSelectQuestID.h"
#include "ClientDef.h"
#include "TempInformation.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCSelectQuestIDHandler::execute ( GCSelectQuestID * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__
	
	DEBUG_ADD("[GCSelectQuestIDHandler] Execute");
	UI_RunQuestList( pPacket );
	
	g_pTempInformation->SetMode( TempInformation::MODE_SELECT_QUEST );	
	
#endif

	__END_CATCH
}
