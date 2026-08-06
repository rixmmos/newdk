//---------------------------------------------------------------------------
// RequestFileManager.cpp
//---------------------------------------------------------------------------
#include "Client_PCH.h"
#include "RequestFileManager.h"
#include "Packet/RequestClientPlayer.h"
#include "Packet/RequestServerPlayer.h"
#include "ProfileManager.h"

#include "Packet/Rpackets/RCRequestedFile.h"
#include "Packet/Rpackets/RCRequestVerify.h"

#include "ServerInfo.h"
#include "DebugInfo.h"
#include "MTypeDef.h"
#include "UIFunction.h"

//---------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------
RequestFileManager* g_pRequestFileManager = NULL;

#define	MAX_BUFFER	4096		// 4K

//---------------------------------------------------------------------------
//
//				Request ReceiveInfo
//
//---------------------------------------------------------------------------
RequestReceiveInfo::RequestReceiveInfo(const char* pRequestUser)
{
	m_RequestUser = pRequestUser;
}

RequestReceiveInfo::~RequestReceiveInfo()
{
	while (!m_FileInfos.empty()) 
	{ 
		delete m_FileInfos.front(); 
		m_FileInfos.pop_front(); 
	}		
}

ReceiveFileInfo::ReceiveFileInfo(const char* pFilename, 
								REQUEST_FILE_TYPE fileType)
{
	m_Mode		= REQUEST_FILE_MODE_BEFORE;
	m_FileType	= REQUEST_FILE_NULL;

	m_Filename	= pFilename;
	m_FileType	= fileType;
	
	m_FileSizeLeft = 0;
}

ReceiveFileInfo::~ReceiveFileInfo()
{
	
	if (m_FileStream.is_open())
	{
		m_FileStream.close();
	}
}

//---------------------------------------------------------------------------
// Start Receive
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void
ReceiveFileInfo::StartReceive(DWORD filesize)
{
	m_Mode = REQUEST_FILE_MODE_RECEIVE;

	
	m_FilenameTemp = m_Filename;

	int dot = m_FilenameTemp.rfind(".");

	m_FilenameTemp[dot] = '-';

	m_FilenameTemp += ".tmp";

	
	if (m_FileStream.is_open())
	{
		m_FileStream.close();
	}

	m_FileStream.open( m_FilenameTemp.c_str() , std::ios::out | std::ios::binary | std::ios::trunc );

	
	m_FileSizeLeft = filesize;
}

//---------------------------------------------------------------------------
// Receive
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void		
ReceiveFileInfo::Receive(const char* pBuffer, DWORD nReceived)
{
	m_FileStream.write( pBuffer, nReceived );

	m_FileSizeLeft -= nReceived;
}

//---------------------------------------------------------------------------
// End Receive
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void		
ReceiveFileInfo::EndReceive(const std::string& requestUser)
{
	m_Mode = REQUEST_FILE_MODE_AFTER;

	m_FileStream.close();

	switch (m_FileType)
	{
		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		case REQUEST_FILE_PROFILE :
			if (rename( m_FilenameTemp.c_str(), m_Filename.c_str() )==0)
			{
				g_pProfileManager->AddProfile( requestUser.c_str(), m_Filename.c_str() );
			}
			else
			{
				remove( m_Filename.c_str() );

				if (rename( m_FilenameTemp.c_str(), m_Filename.c_str() )==0)
				{
					g_pProfileManager->AddProfile( requestUser.c_str(), m_Filename.c_str() );	
				}
				else
				{
					remove( m_FilenameTemp.c_str() );
				}
			}
			UI_RefreshInfoImage();
		break;

		//--------------------------------------------------------------------
		
		//--------------------------------------------------------------------
		case REQUEST_FILE_PROFILE_INDEX :
			if (rename( m_FilenameTemp.c_str(), m_Filename.c_str() )==0)
			{			
			}
			else
			{
				remove( m_Filename.c_str() );

				if (rename( m_FilenameTemp.c_str(), m_Filename.c_str() )==0)
				{
				}
				else
				{
					remove( m_FilenameTemp.c_str() );
				}
			}
		break;
	}
}

//---------------------------------------------------------------------------
//
//				Request SendInfo
//
//---------------------------------------------------------------------------
RequestSendInfo::RequestSendInfo(const char* pRequestUser)
{
	m_RequestUser = pRequestUser;
}

RequestSendInfo::~RequestSendInfo()
{
	while (!m_FileInfos.empty()) 
	{ 
		delete m_FileInfos.front(); 
		m_FileInfos.pop_front(); 
	}		
}

//---------------------------------------------------------------------------
// Make RCRequestedFile Packet
//---------------------------------------------------------------------------
void				
RequestSendInfo::MakeRCRequestedFilePacket(RCRequestedFile& packet) const
{
	std::list<SendFileInfo*>::const_iterator iInfo = m_FileInfos.begin();

	
	//while (iInfo != m_FileInfos.end())
	if (iInfo != m_FileInfos.end())
	{
		SendFileInfo* pInfo = *iInfo;

		RCRequestedFileInfo* pFileInfo = new RCRequestedFileInfo;

		pFileInfo->setRequestFileType( pInfo->GetFileType() );
		pFileInfo->setVersion( 0 );
		pFileInfo->setFilename ( pInfo->GetFilename() );
		pFileInfo->setFileSize ( pInfo->GetFileSizeLeft() );

		packet.addInfo( pFileInfo );

		iInfo ++;
	}
}

SendFileInfo::SendFileInfo(const char* pFilename, 
							REQUEST_FILE_TYPE fileType)
{
	m_Mode		= REQUEST_FILE_MODE_BEFORE;
	m_FileType	= REQUEST_FILE_NULL;

	m_Filename	= pFilename;
	m_FileType	= fileType;

	m_FileSizeLeft = 0;
}

SendFileInfo::~SendFileInfo()
{
	
	if (m_FileStream.is_open())
	{
		m_FileStream.close();
	}
}

//---------------------------------------------------------------------------
// Start Send
//---------------------------------------------------------------------------
void		
SendFileInfo::StartSend()
{
	m_Mode = REQUEST_FILE_MODE_SEND;

	m_FileStream.open( m_Filename.c_str(), std::ios::in | std::ios::binary);// |  );

	if (m_FileStream.is_open())
	{
		m_FileStream.seekg( 0, std::ios::end );

		m_FileSizeLeft = m_FileStream.tellg();	

		m_FileStream.seekg( 0, std::ios::beg );
	}
	else
	{
		m_FileSizeLeft = 0;
	}
}

//---------------------------------------------------------------------------
// Send
//---------------------------------------------------------------------------
DWORD		
SendFileInfo::Send(char* pBuffer)
{
	m_FileStream.read(pBuffer, MAX_BUFFER);

	DWORD nRead = m_FileStream.gcount();

	m_FileSizeLeft -= nRead;

	return nRead;
}

//---------------------------------------------------------------------------
// Send
//---------------------------------------------------------------------------
// -_-;;
//---------------------------------------------------------------------------
void
SendFileInfo::SendBack(DWORD nBack)
{
	m_FileStream.seekg( -nBack, std::ios::cur );
	m_FileSizeLeft += nBack;
}

//---------------------------------------------------------------------------
// End Send
//---------------------------------------------------------------------------
void		
SendFileInfo::EndSend()
{
	m_Mode = REQUEST_FILE_MODE_AFTER;

	m_FileStream.close();
}

//---------------------------------------------------------------------------
//
//				RequestFileManager
//
//---------------------------------------------------------------------------
RequestFileManager::RequestFileManager()
{
}

RequestFileManager::~RequestFileManager()
{
	Release();
}

//---------------------------------------------------------------------------
// Release
//---------------------------------------------------------------------------
void			
RequestFileManager::Release()
{
	//------------------------------------------------------------
	// MyRequest
	//------------------------------------------------------------
	REQUEST_RECEIVE_MAP::iterator iMy = m_MyRequests.begin();

	while (iMy != m_MyRequests.end())
	{
		RequestReceiveInfo* pInfo = iMy->second;

		delete pInfo;

		iMy++;
	}

	m_MyRequests.clear();

	//------------------------------------------------------------
	// OtherRequest
	//------------------------------------------------------------
	REQUEST_SEND_MAP::iterator iOther = m_OtherRequests.begin();

	while (iOther != m_OtherRequests.end())
	{
		RequestSendInfo* pInfo = iOther->second;

		delete pInfo;

		iOther++;
	}

	m_OtherRequests.clear();
}

//---------------------------------------------------------------------------
// Add MyRequest
//---------------------------------------------------------------------------
bool
RequestFileManager::AddMyRequest(RequestReceiveInfo* pInfo)
{
	REQUEST_RECEIVE_MAP::iterator iMy = m_MyRequests.find( pInfo->GetRequestUser() );

	if (iMy == m_MyRequests.end())
	{
		m_MyRequests[pInfo->GetRequestUser()] = pInfo;

		return true;
	}

	delete pInfo;

	return false;
}

//---------------------------------------------------------------------------
// Remove MyRequest
//---------------------------------------------------------------------------
bool			
RequestFileManager::RemoveMyRequest(const std::string& name)
{
	REQUEST_RECEIVE_MAP::iterator iMy = m_MyRequests.find( name );

	if (iMy != m_MyRequests.end())
	{
		RequestReceiveInfo* pInfo = iMy->second;

		delete pInfo;

		m_MyRequests.erase( iMy );

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
// Has MyRequest
//---------------------------------------------------------------------------
bool
RequestFileManager::HasMyRequest(const std::string& name) const
{
	REQUEST_RECEIVE_MAP::const_iterator iMy = m_MyRequests.find( name );

	if (iMy != m_MyRequests.end())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
// Receive MyRequest
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
bool
RequestFileManager::ReceiveMyRequest(const std::string& name, RequestClientPlayer* pRequestClientPlayer)
	throw (ConnectException)
{
	REQUEST_RECEIVE_MAP::iterator iMy = m_MyRequests.find( name );

	if (iMy == m_MyRequests.end())
	{
		return false;
	}

	RequestReceiveInfo* pInfo = iMy->second;

	ReceiveFileInfo* pFileInfo = pInfo->GetFront();

	if (pFileInfo!=NULL)
	{
		switch (pFileInfo->GetMode())
		{
			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			case REQUEST_FILE_MODE_BEFORE :
			{
				//pFileInfo->StartReceive();
			}
			return false;

			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			case REQUEST_FILE_MODE_RECEIVE :
			{
				Socket* pSocket = pRequestClientPlayer->getSocket();

				char buf[MAX_BUFFER+1];	// 10k
				
				//------------------------------------------------------------------
				
				//------------------------------------------------------------------
				DWORD length = pRequestClientPlayer->getInputStreamLength();
				
				while ( length > 0 ) 
				{	
					
					
					int nReceive = min(length, pFileInfo->GetFileSizeLeft());
					nReceive = min(MAX_BUFFER, nReceive);					

					int nReceived = pRequestClientPlayer->readInputStream( buf , nReceive );

					pFileInfo->Receive( buf, nReceived );

					length -= nReceived;

					
					if (pFileInfo->GetFileSizeLeft()==0)
					{
						break;
					}
				}

				//------------------------------------------------------------------
				
				//------------------------------------------------------------------
				/*
				DWORD available = pSocket->available();

				while ( available > 0 ) 
				{				
					int nReceive = min(MAX_BUFFER, available);
					int nReceived = pSocket->receive( buf , nReceive , 0 );

					pInfo->Receive( buf, nReceived );

					available -= nReceived;
				}
				*/

				//------------------------------------------------------------------
				
				//------------------------------------------------------------------
				if (pFileInfo->GetFileSizeLeft()==0)
				{
					pFileInfo->EndReceive( pInfo->GetRequestUser() );

					
					pInfo->DeleteFront();
					
					
					if (pInfo->IsEnd())
					{
						delete pInfo;

						m_MyRequests.erase( iMy );
						
						
						//throw ConnectException("Receive Done");
					}					
				}
			}
			return true;

			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			case REQUEST_FILE_MODE_AFTER :
			return false;
		}
	}
	else
	{
		
		delete pInfo;

		m_MyRequests.erase( iMy );
		
		//throw ConnectException("Receive Done");		
	}

	return false;
}

//---------------------------------------------------------------------------
// Add OtherRequest
//---------------------------------------------------------------------------
bool
RequestFileManager::AddOtherRequest(RequestSendInfo* pInfo)
{
	REQUEST_SEND_MAP::iterator iOther = m_OtherRequests.find( pInfo->GetRequestUser() );

	if (iOther == m_OtherRequests.end())
	{
		m_OtherRequests[pInfo->GetRequestUser()] = pInfo;

		return true;
	}

	delete pInfo;

	return false;
}

//---------------------------------------------------------------------------
// Remove OtherRequest
//---------------------------------------------------------------------------
bool
RequestFileManager::RemoveOtherRequest(const std::string& name)
{
	REQUEST_SEND_MAP::iterator iOther = m_OtherRequests.find( name );

	if (iOther != m_OtherRequests.end())
	{
		RequestSendInfo* pInfo = iOther->second;

		delete pInfo;

		m_OtherRequests.erase( iOther );

		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
// Has OtherRequest
//---------------------------------------------------------------------------
bool
RequestFileManager::HasOtherRequest(const std::string& name) const
{
	REQUEST_SEND_MAP::const_iterator iOther = m_OtherRequests.find( name );

	if (iOther != m_OtherRequests.end())
	{
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
// Send OtherRequest
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool			
RequestFileManager::SendOtherRequest(const std::string& name, RequestServerPlayer* pRequestServerPlayer)
{
	REQUEST_SEND_MAP::iterator iOther = m_OtherRequests.find( name );

	if (iOther == m_OtherRequests.end())
	{
		return false;
	}

	RequestSendInfo* pInfo = iOther->second;	

	SendFileInfo* pFileInfo = pInfo->GetFront();

	if (pFileInfo!=NULL)
	{
		switch (pFileInfo->GetMode())
		{
			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			case REQUEST_FILE_MODE_BEFORE :
			{
				pFileInfo->StartSend();

				if (pInfo->GetSize()!=0)
				{
					RCRequestedFile rcRequestedFile;

					pInfo->MakeRCRequestedFilePacket(rcRequestedFile);
					
					pRequestServerPlayer->sendPacket( &rcRequestedFile );
				}
				else
				{
					throw ConnectException("No File to Send");
				}
			}
			return false;

			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			case REQUEST_FILE_MODE_SEND :
			{
				char buf[MAX_BUFFER+1];	// 10k
				
				DWORD nRead = pFileInfo->Send(buf);

				if (nRead > 0)
				{
					DWORD nSent = pRequestServerPlayer->send( buf , nRead );

					if (nSent!=nRead)
					{
						
						// socketInputStream
						pFileInfo->SendBack( nRead - nSent );
					}
				}

				if (nRead < MAX_BUFFER)	
				{
					if (pFileInfo->GetFileSizeLeft()==0)
					{
						pFileInfo->EndSend();

						pInfo->DeleteFront();

						if (pInfo->IsEnd())
						{
							
							RCRequestVerify rcRequestVerify;
							rcRequestVerify.setCode( REQUEST_VERIFY_PROFILE_DONE );

							pRequestServerPlayer->sendPacket( &rcRequestVerify );

							
							delete pInfo;
							m_OtherRequests.erase( iOther );
						}
						else
						{
							//pFileInfo = pInfo->GetFront();

							
						}
					}
					else
					{
						DEBUG_ADD_FORMAT("[Error] FileSizeLeft = %d", pFileInfo->GetFileSizeLeft());
					}
				}			
			}
			return true;
			
			//------------------------------------------------------------------
			
			//------------------------------------------------------------------
			case REQUEST_FILE_MODE_AFTER :
			return false;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
// Update
//---------------------------------------------------------------------------
// m_MyRequest
//---------------------------------------------------------------------------
void			
RequestFileManager::Update()
{
}
