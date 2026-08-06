//----------------------------------------------------------------------
// CServerInformation.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__SERVERINFORMATION_H__
#define	__SERVERINFORMATION_H__

#pragma warning(disable:4786)

#include "CTypeMap2.h"
#include "MString.h"

#include <fstream>
using namespace std;

//----------------------------------------------------------------------
// SERVER_INFO
//----------------------------------------------------------------------
class SERVER_INFO {
	public :
		/*
		enum SERVER_STATUS 
		{
			SERVER_FREE,
			SERVER_NORMAL,
			SERVER_BUSY,
			SERVER_VERY_BUSY,
			SERVER_FULL,
			SERVER_DOWN
		};
		*/

	public :
		SERVER_INFO() { ServerStatus = 0; }
		~SERVER_INFO() {}
		
	public :
		MString			ServerName;
		int				ServerStatus;
};

//----------------------------------------------------------------------
// ServerGroup
//----------------------------------------------------------------------


//----------------------------------------------------------------------
/*
	enum ServerStatus 
	{
		SERVER_FREE,
		SERVER_NORMAL,
		SERVER_BUSY,
		SERVER_VERY_BUSY,
		SERVER_FULL,
		SERVER_DOWN
	};
*/
class ServerGroup : public CTypeMap2<SERVER_INFO> {
	public :
		ServerGroup();
		~ServerGroup() {}

		//--------------------------------------------------------------
		// Group Name
		//--------------------------------------------------------------
		void		SetGroupName(const char* pName)	{ m_GroupName = pName; }
		const char*	GetGroupName() const			{ return m_GroupName.GetString(); }

		//--------------------------------------------------------------
		
		//--------------------------------------------------------------
		void		SetGroupStatus(int status)		{ m_GroupStatus = status; }
		int			GetGroupStatus() const			{ return m_GroupStatus; }

	protected :
		MString			m_GroupName;
		int				m_GroupStatus;
};

//----------------------------------------------------------------------
// CServerInformation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
class CServerInformation : public CTypeMap2<ServerGroup> {
	public :
		typedef CTypeMap2<ServerGroup>		SERVER_GROUP_MAP;

	public :
		CServerInformation();
		~CServerInformation();

		void			Release();

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		bool			SetServerGroupID(unsigned int id);
		bool			SetServerID(unsigned int id);

		unsigned int	GetServerGroupID()		{ return m_ServerGroupID; }
		unsigned int	GetServerID()			{ return m_ServerID; }

		
		void			SetServerGroupName(const char* pName)	{ m_ServerGroupName = pName; }
		void			SetServerName(const char* pName)		{ m_ServerName = pName; }

		const char*		GetServerGroupName()	{ return m_ServerGroupName.GetString(); }
		const char*		GetServerName()			{ return m_ServerName.GetString(); }

		void			SetServerGroupStatus(int status)		{ m_ServerGroupStatus = status; }
		int				GetServerGroupStatus()					{ return m_ServerGroupStatus; }

		void			SetServerStatus(int status)				{ m_ServerStatus = status; }
		int				GetServerStatus()						{ return m_ServerStatus; }
		
	protected :
		unsigned int	m_ServerGroupID;
		unsigned int	m_ServerID;

		MString			m_ServerGroupName;
		int				m_ServerGroupStatus;

		MString			m_ServerName;
		int				m_ServerStatus;
		
};

extern	CServerInformation*	g_pServerInformation;

#endif

