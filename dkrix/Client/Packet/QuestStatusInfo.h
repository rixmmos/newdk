#ifndef __QUEST_STATUS_INFO_H__
#define __QUEST_STATUS_INFO_H__

#include "Packet/Types.h"

#include "Packet.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

#include <list>
#include <VECTOR>
#define MAX_MISSION_NUM	100


struct UI_GMissionInfo{
	std::string		szMissionTitle;
	std::string		m_StrArg;
	DWORD			m_NumArg;
	BYTE			bStatus;
	BYTE			bCondition;	
	WORD			bIndex;		
	DWORD			dwTimeLimit;
};
struct UI_GQuestInfo{
	DWORD			dwQuestID;
	BYTE			bStatus;
	std::string		szQuestTitle;
	std::string		szQuestDescription;
	std::vector<UI_GMissionInfo*> vMissionList;
};

struct MissionInfo
{
	enum Status
	{
		HIDE = 0,			
		CURRENT,			
		SUCCESS,			
		FAIL,				
	};

	BYTE	m_Condition;	
	WORD	m_Index;		
	BYTE	m_Status;		

	string	m_StrArg;		
	DWORD	m_NumArg;		

	void read(SocketInputStream& iStream) throw(ProtocolException, Error)
	{
		iStream.read(m_Condition);
		iStream.read(m_Index);
		iStream.read(m_Status);

		BYTE szSTR;
		iStream.read(szSTR);
		if ( szSTR != 0 ) iStream.read(m_StrArg, szSTR);

		iStream.read(m_NumArg);
	}

	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error)
	{
		oStream.write(m_Condition);
		oStream.write(m_Index);
		oStream.write(m_Status);

		BYTE szSTR = m_StrArg.size();
		oStream.write( szSTR );
		if ( szSTR != 0 ) oStream.write( m_StrArg );

		oStream.write( m_NumArg );
	}

	PacketSize_t getSize() const { return szBYTE + szWORD + szBYTE + szBYTE + m_StrArg.size() + szDWORD; }
	static PacketSize_t getMaxSize() { return szBYTE + szWORD + szBYTE + szBYTE + 255 + szDWORD; }
};

class QuestStatusInfo
{
public:
	enum Status
	{
		CANNOT = 0,		
		CAN_ACCEPT,		
		DOING,			
		SUCCESS,		
		COMPLETE,		
		FAIL,			
		CAN_REPLAY,		
	};

	void read(SocketInputStream& iStream) throw(ProtocolException, Error);
	void write(SocketOutputStream& oStream) const throw(ProtocolException, Error);
	PacketSize_t getSize() const;
	static PacketSize_t getMaxSize();
	DWORD GetQuestID() {return m_QuestID;}
	BYTE  GetStatus() {return m_Status;}
	const std::list<MissionInfo>& GetMission() const {return m_Missions;}
private:
	DWORD	m_QuestID;
	BYTE	m_Status;	
	std::list<MissionInfo>	m_Missions;
};

#endif
