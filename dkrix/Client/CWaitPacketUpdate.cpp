//-----------------------------------------------------------------------------
// CWaitPacketUpdate.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#pragma warning(disable:4786)
#include "CWaitPacketUpdate.h"
#include "Client.h"


// Global
CWaitPacketUpdate*		g_pCWaitPacketUpdate = NULL;

//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
void
CWaitPacketUpdate::Init()
{
	
	g_pSDLInput->SetMouseEventReceiver( NULL );

	
	g_pSDLInput->SetKeyboardEventReceiver( NULL );
}


//-----------------------------------------------------------------------------
// Update WaitPosition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void 
CWaitPacketUpdate::Update()
{
	//DEBUG_ADD("[WaitPacketUpdate] Begin");

	//------------------------------------------	
	
	//------------------------------------------
	if ((DWORD)g_CurrentTime > m_DelayLimit)
	{
		DEBUG_ADD_FORMAT("[WaitPacket]   : Current=%d, Limit=%d", g_CurrentTime, m_DelayLimit);			

		SetMode( MODE_MAINMENU );
		UpdateDisconnected();

	}

	//------------------------------------------
	// Sound Stream
	//------------------------------------------
//	if (g_pSDLStream!=NULL)
//	{
//		//DEBUG_ADD("[WaitPacketUpdate] SoundStream");
//
//		g_pSDLStream->Update();
//	}

	//------------------------------------------
	
	//------------------------------------------	
	if (!UpdateSocketInput())
	{
		//DEBUG_ADD("[WaitPacketUpdate] SocketInput");

		return;
	}

	//------------------------------------------	
	
	//------------------------------------------	
	if (g_ModeNext!=MODE_NULL)
	{
		DEBUG_ADD_FORMAT("[WaitPacketUpdate] g_ModeNext is Not MODE_NULL(%d)", (int)g_ModeNext);

		SetMode( g_ModeNext );
		g_ModeNext = MODE_NULL;

		return;
	}

	//DEBUG_ADD("[WaitPacketUpdate] SocketOutput");
	
	UpdateSocketOutput();

	//DEBUG_ADD("[WaitPacketUpdate] End");
}

//-----------------------------------------------------------------------------
// Set Delay
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void		
CWaitPacketUpdate::SetDelay(DWORD delay)
{
	g_CurrentTime = timeGetTime();
			
	m_DelayLimit = g_CurrentTime + delay;

	DEBUG_ADD_FORMAT("[WaitPacket] Current=%d, Limit=%d", g_CurrentTime, m_DelayLimit);
}