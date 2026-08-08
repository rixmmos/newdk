//---------------------------------------------------------------------------
// RequestFileManager.h 
//---------------------------------------------------------------------------

//


//



//



//
//

//


//


//
//


//		SendOtherRequest(RequestServerPlayer*)
//

//---------------------------------------------------------------------------

#ifndef __REQUEST_FILE_MANAGER_H__
#define __REQUEST_FILE_MANAGER_H__

#pragma warning(disable:4786)

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif
#include <string>
#include <list>
#include <map>
#include <fstream>
#include "Packet/Types/RequestTypes.h"
#include "Packet/Exception.h"
class RequestClientPlayer;
class RequestServerPlayer;
class RCRequestedFile;

//---------------------------------------------------------------------------
// REQUEST_FILE_MODE
//---------------------------------------------------------------------------
enum REQUEST_FILE_MODE
{
	REQUEST_FILE_MODE_BEFORE,
	REQUEST_FILE_MODE_SEND,
	REQUEST_FILE_MODE_RECEIVE,
	REQUEST_FILE_MODE_AFTER,
};

//---------------------------------------------------------------------------
// SendFileInfo
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class SendFileInfo
{
	private :
		REQUEST_FILE_MODE		m_Mode;

		
		std::string				m_Filename;		
		REQUEST_FILE_TYPE		m_FileType;		
		
		
		std::ifstream			m_FileStream;	
		DWORD					m_FileSizeLeft;

	public :
		SendFileInfo(const char* pFilename, REQUEST_FILE_TYPE fileType);
		~SendFileInfo();

		void		StartSend();
		bool		IsSendMode() const	{ return m_Mode==REQUEST_FILE_MODE_SEND; }
		DWORD		Send(char* pBuffer);		
		void		SendBack(DWORD nBack);
		void		EndSend();

		// Get
		REQUEST_FILE_MODE	GetMode() const			{ return m_Mode; }
		REQUEST_FILE_TYPE	GetFileType() const		{ return m_FileType; }
		const std::string&	GetFilename() const		{ return m_Filename; }
		DWORD			GetFileSizeLeft() const	{ return m_FileSizeLeft; }
};

//---------------------------------------------------------------------------
// ReceiveFileInfo
//---------------------------------------------------------------------------
class ReceiveFileInfo
{
	public :
		REQUEST_FILE_MODE		m_Mode;

		std::string				m_Filename;		
		REQUEST_FILE_TYPE		m_FileType;		
		
		
		std::string				m_FilenameTemp;	
		std::ofstream				m_FileStream;		
		DWORD					m_FileSizeLeft;

	public :
		ReceiveFileInfo(const char* pFilename, REQUEST_FILE_TYPE fileType);
		~ReceiveFileInfo();
		
		void		StartReceive(DWORD filesize);
		bool		IsReceiveMode() const	{ return m_Mode==REQUEST_FILE_MODE_RECEIVE; }
		void		Receive(const char* pBuffer, DWORD size);
		void		EndReceive(const std::string& requestUser);

		// Get
		REQUEST_FILE_MODE	GetMode() const			{ return m_Mode; }
		REQUEST_FILE_TYPE	GetFileType() const		{ return m_FileType; }
		const std::string&	GetFilename() const		{ return m_Filename; }
		DWORD			GetFileSizeLeft() const	{ return m_FileSizeLeft; }
};

//---------------------------------------------------------------------------
// RequestReceiveInfo
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class RequestReceiveInfo
{
	private :
		
		std::string				m_RequestUser;	

		std::list<ReceiveFileInfo*>	m_FileInfos;

	public :
		RequestReceiveInfo(const char* pRequestUser);
		~RequestReceiveInfo();

		void				AddReceiveFileInfo(ReceiveFileInfo* pInfo)	{ m_FileInfos.push_back( pInfo ); }

		const std::string&	GetRequestUser() const	{ return m_RequestUser; }

		DWORD				GetSize() const		{ return m_FileInfos.size(); }
		ReceiveFileInfo*	GetFront()			{ return (m_FileInfos.empty()? NULL : m_FileInfos.front()); }
		void				DeleteFront()		{ if (!m_FileInfos.empty()) { delete m_FileInfos.front(); m_FileInfos.pop_front(); } }		
		bool				IsEnd()	const		{ return m_FileInfos.empty(); }

};

//---------------------------------------------------------------------------
// RequestSendInfo
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class RequestSendInfo
{
	private :
		std::string				m_RequestUser;	

		std::list<SendFileInfo*>	m_FileInfos;

	public :
		RequestSendInfo(const char* pRequestUser);
		~RequestSendInfo();
		
		void				AddSendFileInfo(SendFileInfo* pInfo)	{ m_FileInfos.push_back( pInfo ); }

		const std::string&	GetRequestUser() const	{ return m_RequestUser; }

		DWORD				GetSize() const		{ return m_FileInfos.size(); }
		SendFileInfo*		GetFront()			{ return (m_FileInfos.empty()? NULL : m_FileInfos.front()); }
		void				DeleteFront()		{ if (!m_FileInfos.empty()) { delete m_FileInfos.front(); m_FileInfos.pop_front(); } }		
		bool				IsEnd()	const		{ return m_FileInfos.empty(); }

		void				MakeRCRequestedFilePacket(RCRequestedFile& packet) const;
};


//---------------------------------------------------------------------------
// RequestFileManager
//---------------------------------------------------------------------------
class RequestFileManager {
	public :
		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		typedef std::map<std::string, RequestReceiveInfo*>		REQUEST_RECEIVE_MAP;

		//-----------------------------------------------------------------
		
		//-----------------------------------------------------------------
		typedef std::map<std::string, RequestSendInfo*>			REQUEST_SEND_MAP;

	public :
		RequestFileManager();
		~RequestFileManager();

		//--------------------------------------------------------------
		// Release
		//--------------------------------------------------------------
		void			Release();

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool			AddMyRequest(RequestReceiveInfo* pInfo);
		bool			RemoveMyRequest(const std::string& name);
		bool			HasMyRequest(const std::string& name) const;
		bool			ReceiveMyRequest(const std::string& name, RequestClientPlayer* pRequestClientPlayer) throw (ConnectException);

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		bool			AddOtherRequest(RequestSendInfo* pInfo);
		bool			RemoveOtherRequest(const std::string& name);
		bool			HasOtherRequest(const std::string& name) const;
		bool			SendOtherRequest(const std::string& name, RequestServerPlayer* pRequestServerPlayer);

		//--------------------------------------------------------------
		// Update
		//--------------------------------------------------------------
		void			Update();

	protected :
		REQUEST_RECEIVE_MAP			m_MyRequests;		
		REQUEST_SEND_MAP		m_OtherRequests;	
};

extern RequestFileManager* g_pRequestFileManager;

#endif


