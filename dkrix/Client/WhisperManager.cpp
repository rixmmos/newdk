//----------------------------------------------------------------------
// WhisperManager.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "WhisperManager.h"
#include "Packet/RequestClientPlayerManager.h"
#include "RequestUserManager.h"
#include "MGameStringTable.h"

#include "Packet/Cpackets/CGRequestIP.h"
#include "Packet/Cpackets/CGWhisper.h"
#include "Packet/ClientPlayer.h"
#include "DebugInfo.h"
#include "ServerInfo.h"
#include "UserInformation.h"

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
WhisperManager*		g_pWhisperManager = NULL;

//----------------------------------------------------------------------
// extern
//----------------------------------------------------------------------
	extern ClientPlayer*		g_pSocket;

extern void		UI_AddChatToHistory(char* str, char* sz_id, int cond, DWORD color);


//----------------------------------------------------------------------
//
//						WhisperInfo
//
//----------------------------------------------------------------------
// Send to GameServer
//----------------------------------------------------------------------
void
WhisperInfo::SendToGameServer() const
{
	std::list<WHISPER_MESSAGE>::const_iterator iMessage = Messages.begin();


	CGWhisper _CGWhisper;
	_CGWhisper.setName( Name.c_str() );

	
	while (iMessage != Messages.end())
	{		
		_CGWhisper.setMessage( (*iMessage).msg.c_str() );
		_CGWhisper.setColor( (*iMessage).color );

		g_pSocket->sendPacket( &_CGWhisper );

		iMessage ++;
	}
}

//----------------------------------------------------------------------
//
//						WhisperManager
// 
//----------------------------------------------------------------------
WhisperManager::WhisperManager()
{
}

WhisperManager::~WhisperManager()
{
	Release();

}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void		
WhisperManager::Release()
{
	Lock();

	WHISPER_INFO_MAP::iterator iInfo = m_WhisperInfos.begin();

	while (iInfo != m_WhisperInfos.end())
	{
		WhisperInfo* pInfo = iInfo->second;

		delete pInfo;

		iInfo++;
	}

	m_WhisperInfos.clear();

	Unlock();
}

//----------------------------------------------------------------------
// Send / Remove Message
//----------------------------------------------------------------------
bool		
WhisperManager::HasWhisperMessage(const char* pName) const
{
	WHISPER_INFO_MAP::const_iterator iInfo = m_WhisperInfos.find( std::string(pName) );	

	if (iInfo != m_WhisperInfos.end())
	{
		return true;
	}

	return false;
}

//----------------------------------------------------------------------
// Send Message
//----------------------------------------------------------------------
//
// if (has toUser's IP in RequestUserManager)
// {			
//		RequestClientManager --(CRWhisper)--> RequestServer(toUser) [2]			
// }
// else
// {
//		CGWhisper --> GameServer [3]
// }
//
//----------------------------------------------------------------------
void		
WhisperManager::SendWhisperMessage(const char* pName, const char* pMessage, DWORD color)
{
	//CRWhisper *pPacket;
	//_CRWhisper.setName( pName );
	//_CRWhisper.setMessage( pMessage );
	//Packet* pPacket = NULL;

	if( 0 && g_pUserInformation->bKorean == true )	
	{
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		if (g_pRequestClientPlayerManager->HasConnection(pName)
			|| g_pRequestClientPlayerManager->HasTryingConnection(pName))//!g_pRequestClientPlayerManager->SendPacket(pName, pPacket))
		{
			AddWhisperMessage( pName, pMessage, color );
		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		else
		{
			RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo(pName);

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (pUserInfo!=NULL)
			{
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (pUserInfo->IsStatusUnable())
				{
					
						CGWhisper _CGWhisper;
						_CGWhisper.setName( pName );

						_CGWhisper.setMessage( pMessage );
						_CGWhisper.setColor( color );

						g_pSocket->sendPacket( &_CGWhisper );
				}
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				else
				{
					AddWhisperMessage( pName, pMessage, color );
					g_pRequestClientPlayerManager->Connect(pUserInfo->IP.c_str(), pName, REQUEST_CLIENT_MODE_WHISPER);
				}
			}
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			else
			{
				if (!g_pRequestUserManager->HasRequestingUser( pName ))
				{
					if( g_pUserInformation->bKorean == true )
					{
						
						CGRequestIP _CGRequestIP;
						_CGRequestIP.setName( pName );

						g_pSocket->sendPacket( &_CGRequestIP );			
		
					g_pRequestUserManager->AddRequestingUser( pName, RequestUserManager::REQUESTING_FOR_WHISPER );
					}
				}

				
				AddWhisperMessage( pName, pMessage, color );
			}
		}	
	}
	else	
	{
		
		CGWhisper _CGWhisper;
		_CGWhisper.setName( pName );
		
		_CGWhisper.setMessage( pMessage );
		_CGWhisper.setColor( color );
		
		g_pSocket->sendPacket( &_CGWhisper );
	}
}

//----------------------------------------------------------------------
// Add Message
//----------------------------------------------------------------------
void		
WhisperManager::AddWhisperMessage(const char* pName, const char* pMessage, DWORD color)
{
	Lock();

	WHISPER_INFO_MAP::iterator iInfo = m_WhisperInfos.find( std::string(pName) );

	//------------------------------------------------
	
	//------------------------------------------------
	if (iInfo == m_WhisperInfos.end())
	{
		WhisperInfo* pInfo = new WhisperInfo;
		pInfo->Name		= std::string(pName);
		WHISPER_MESSAGE temp_message;
		temp_message.msg = std::string(pMessage);
		temp_message.color = color;
		pInfo->Messages.push_back( temp_message );

		m_WhisperInfos[pInfo->Name] = pInfo;
	}
	//------------------------------------------------
	
	//------------------------------------------------
	else
	{
		WhisperInfo* pInfo = iInfo->second;
		
		WHISPER_MESSAGE temp_message;
		temp_message.msg = std::string(pMessage);
		temp_message.color = color;
		pInfo->Messages.push_back( temp_message );
	}

	Unlock();
}

//----------------------------------------------------------------------
// Get Message
//----------------------------------------------------------------------
const std::list<WHISPER_MESSAGE>*
WhisperManager::GetWhisperMessages(const char* pName) const
{
	WHISPER_INFO_MAP::const_iterator iInfo = m_WhisperInfos.find( std::string(pName) );

	if (iInfo != m_WhisperInfos.end())
	{
		return &iInfo->second->Messages;
	}

	return NULL;
}

//----------------------------------------------------------------------
// Remove Message
//----------------------------------------------------------------------
void		
WhisperManager::TryToSendWhisperMessage(const char* pName)
{
	Lock();

	WHISPER_INFO_MAP::const_iterator iInfo = m_WhisperInfos.find( std::string(pName) );

	if (iInfo != m_WhisperInfos.end())
	{
		
		iInfo->second->TryingCount++;		
	}
	else
	{
		
//		g_pRequestUserManager->RemoveRequestUserLater( pName );
	}

	Unlock();
}

//----------------------------------------------------------------------
// Remove Message
//----------------------------------------------------------------------
bool
WhisperManager::RemoveWhisperMessage(const char* pName)
{
	Lock();

	WHISPER_INFO_MAP::iterator iInfo = m_WhisperInfos.find( std::string(pName) );

	if (iInfo != m_WhisperInfos.end())
	{
		delete iInfo->second;

		m_WhisperInfos.erase( iInfo );

		Unlock();
		return true;
	}

	Unlock();
	return false;
}


//----------------------------------------------------------------------
// Update
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
WhisperManager::Update()
{
	Lock();  // Must lock BEFORE accessing the container

	if (m_WhisperInfos.empty())
	{
		Unlock();
		return;
	}

	WHISPER_INFO_MAP::iterator iInfo = m_WhisperInfos.begin();

	while (iInfo != m_WhisperInfos.end())
	{
		WhisperInfo* pInfo = iInfo->second;

		//-------------------------------------------------------
		
		
		
		//-------------------------------------------------------
		if (pInfo->TryingCount > 2)		// 2 _-_;
		{
			WHISPER_INFO_MAP::iterator iTemp = iInfo ++;

			
			//UI_AddChatToHistory( (*g_pGameStringTable)[STRING_MESSAGE_WHISPER_FAILED].GetString(), NULL, 5 );

			
			pInfo->SendToGameServer();

			
			RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo(pInfo->Name.c_str());

			if (pUserInfo!=NULL)
			{
				pUserInfo->SetStatusUnable();
			}

			
			delete pInfo;
			m_WhisperInfos.erase( iTemp );

			continue;
		}
		//-------------------------------------------------------
		
		//-------------------------------------------------------
		else
		{			
			const char* pName = pInfo->Name.c_str();

			//-------------------------------------------------------
			
			//-------------------------------------------------------
			if (g_pRequestClientPlayerManager->HasTryingConnection(pName)
				|| g_pRequestClientPlayerManager->HasConnection(pName))			
			{			
			}
			//-------------------------------------------------------
			
			//-------------------------------------------------------
			else
			{
				RequestUserInfo* pUserInfo = g_pRequestUserManager->GetUserInfo(pName);
				
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				if (pUserInfo!=NULL)
				{
					if (pUserInfo->IsStatusUnable())
					{
						pInfo->SendToGameServer();
						
						
						WHISPER_INFO_MAP::iterator iTemp = iInfo ++;
						delete pInfo;
						m_WhisperInfos.erase( iTemp );
						continue;
					}
					else
					{
						g_pRequestClientPlayerManager->Connect(pUserInfo->IP.c_str(), pName, REQUEST_CLIENT_MODE_WHISPER);
					}
				}
				
				//-------------------------------------------------------
				
				//-------------------------------------------------------
				else
				{
					if (!g_pRequestUserManager->HasRequestingUser( pName ))
					{
						if( 0 && g_pUserInformation->bKorean == true )
						{
							
							CGRequestIP _CGRequestIP;
							_CGRequestIP.setName( pName );

							g_pSocket->sendPacket( &_CGRequestIP );

						g_pRequestUserManager->AddRequestingUser( pName, RequestUserManager::REQUESTING_FOR_WHISPER );
						}
					}
				}
			}	
		}
		

		iInfo++;
	}

	Unlock();
}
