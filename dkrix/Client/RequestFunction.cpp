//--------------------------------------------------------------------------------
// RequestFunction.cpp
//--------------------------------------------------------------------------------
#include "Client_PCH.h"
#include "Packet/RequestServerPlayerManager.h"
#include "Packet/RequestClientPlayerManager.h"
#include "DebugInfo.h"


//--------------------------------------------------------------------------------
// Request Connect
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void	
RequestConnect(const char* pIP, const char* pName)
{
	DEBUG_ADD_FORMAT("[RequestConnect] ip=%s, name=%s", pIP, pName);

	if (g_pRequestClientPlayerManager!=NULL)
	{
		g_pRequestClientPlayerManager->Connect( pIP, pName );
	}
}

//--------------------------------------------------------------------------------
// Request Disconnect
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void	
RequestDisconnect(const char* pName)
{
	DEBUG_ADD_FORMAT("[RequestDisconnect] name=%d", pName);

	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	if (g_pRequestClientPlayerManager!=NULL)
	{
		g_pRequestClientPlayerManager->Disconnect( pName );
	}

	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	if (g_pRequestServerPlayerManager!=NULL)
	{
		g_pRequestServerPlayerManager->Disconnect( pName );
	}
}
