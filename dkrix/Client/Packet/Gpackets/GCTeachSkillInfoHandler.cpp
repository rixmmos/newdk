//////////////////////////////////////////////////////////////////////
//
// Filename    : GCTeachSkillInfoHandler.cc

// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCTeachSkillInfo.h"
#include "TempInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
void GCTeachSkillInfoHandler::execute ( GCTeachSkillInfo * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
	
#ifdef __GAME_CLIENT__

	// get/set NPC's object id
	//pPacket->getObjectID();
	
	
	UI_RunSkillTree( (int)pPacket->getDomainType(), 100 );//pPacket->getMaxLevel() );
	
	
	g_pTempInformation->SetMode(TempInformation::MODE_SKILL_LEARN);
	g_pTempInformation->Value1	=	pPacket->getDomainType();
	g_pTempInformation->Value2	=	100;//pPacket->getMaxLevel();
	

#endif

	__END_CATCH
}
