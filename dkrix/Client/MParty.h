//----------------------------------------------------------------------
// MParty.h
//----------------------------------------------------------------------



//



//

//----------------------------------------------------------------------

#ifndef __MParty_H__
#define __MParty_H__

#pragma warning(disable:4786)

#include <string>
#include <vector>

#include "MString.h"
#include "MTypeDef.h"

class PARTY_INFO
{
public :
	MString			Name;		
	TYPE_OBJECTID	ID;			// object ID
	bool			bMale;
	BYTE			hairStyle;

	// guildID
	MString			guildName;
	WORD			guildID;
	
	
	int				zoneID;
	BYTE			zoneX;
	BYTE			zoneY;

	
	WORD			MaxHP;
	WORD			HP;
	
	// IP
	std::string		IP;
	bool			bInSight;

public :
	PARTY_INFO();
};

class MParty {
	public :
		typedef std::vector<PARTY_INFO*>	PARTY_VECTOR;

	public :
		MParty();
		~MParty();

		void		Release();

		//--------------------------------------------------------
		// SetJoinTime
		//--------------------------------------------------------
		DWORD		GetJoinTime() const		{ return m_JoinTime; }		
		void		SetJoinTime(DWORD time)	{ m_JoinTime = time; }
		void		SetJoinTime();		
		bool		IsKickAvailableTime() const;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		int			GetSize() const		{ return m_pInfo.size(); }

		//--------------------------------------------------------
		// Add / Remove
		//--------------------------------------------------------
		bool		AddMember(PARTY_INFO* pInfo);
		bool		RemoveMember(const char* pName);
		bool		RemoveMember(int creatureID);	
		void		UnSetPlayerParty() const;

		//--------------------------------------------------------
		// Accept/Refuse Invite
		//--------------------------------------------------------
		bool		IsAcceptInvite() const		{ return m_bAccept; }
		void		AcceptInvite()				{ m_bAccept = true; }
		void		RefuseInvite()				{ m_bAccept = false; }

		//--------------------------------------------------------
		// Get ID / Name
		//--------------------------------------------------------
		PARTY_INFO*	GetMemberInfo(int n) const;	
		PARTY_INFO*	GetMemberInfo(const char* pName) const;	
		PARTY_INFO* GetMemberInfoByIP(const char* pIP) const;

		bool		HasMember(const char* pName) const;

	protected :
		int							m_Size;			
		PARTY_VECTOR				m_pInfo;		

		bool						m_bAccept;		

		DWORD						m_JoinTime;
};

extern MParty*	g_pParty;

#endif

