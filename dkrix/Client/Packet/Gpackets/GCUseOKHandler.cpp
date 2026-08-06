//////////////////////////////////////////////////////////////////////
//
// Filename    : GCUseOKHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "Client_PCH.h"
#include "GCUseOK.h"
#include "ClientDef.h"
#include "MInventory.h"
#include "MQuickSlot.h"
#include "MSlayerGear.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void GCUseOKHandler::execute ( GCUseOK * pPacket , Player * pPlayer )
	 
throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	// message

	

	UseItemOK();

	//------------------------------------------------------------------
	//
	
	//
	//------------------------------------------------------------------
	AffectModifyInfo( g_pPlayer, pPacket );


	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

#endif

	__END_CATCH
}

void GCUseSkillCardOKHandler::execute ( GCUseSkillCardOK * pPacket , Player * pPlayer )
	 

throw ( ProtocolException , Error )
{
	__BEGIN_TRY
		
#ifdef __GAME_CLIENT__

	// message

	

	UseSkillCardOK(pPacket->getCardType());

	//------------------------------------------------------------------
	//
	
	//
	//------------------------------------------------------------------
//	AffectModifyInfo( g_pPlayer, pPacket );


	//------------------------------------------------------------------
	
	
	//------------------------------------------------------------------
	//UI_SetHP( g_pPlayer->GetHP(), g_pPlayer->GetMAX_HP() );
	//UI_SetMP( g_pPlayer->GetMP(), g_pPlayer->GetMAX_MP() );

#endif

	__END_CATCH
}