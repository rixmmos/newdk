//////////////////////////////////////////////////////////////////////
//
// Filename    : GCMakeItemFailHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCMakeItemFail.h"
#include "TempInformation.h"
#include "ClientDef.h"
#include "UIFunction.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCMakeItemFailHandler::execute ( GCMakeItemFail * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__


	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	if (g_pTempInformation->GetMode() == TempInformation::MODE_SKILL_MAKE_ITEM)
	{
		g_pTempInformation->SetMode(TempInformation::MODE_NULL);
		//g_pTempInformation->Value1 = itemClass;
		//g_pTempInformation->Value2 = itemType;

		
		UI_UnlockItem();
	}
	else
	{
		DEBUG_ADD( "[Error] Temp Mode is Not MODE_SKILL_MAKE_ITEM" );
	}

#endif

	__END_CATCH
}
