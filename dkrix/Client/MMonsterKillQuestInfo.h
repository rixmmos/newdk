////////////////////////////////////////////////////////////////////////////////////
// 2003.4.10
// MMonsterKillQuestInfo
//
// 2003.4.21
// MQuestInfoManager
//



//
// by sonee
////////////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTER_KILL_QUEST_INFO_HEADER__
#define __MONSTER_KILL_QUEST_INFO_HEADER__

#pragma warning(disable:4786)

#include "CTypeTable.h"
#include "MQuestInfo.h"
#include "MString.h"

#include <list>
#include "CTypeMap.h"

#include <fstream>
using namespace std;

enum MINI_GAME_TYPE 
{	
	MINI_GAME_TYPE_MINE = 0,
	MINI_GAME_TYPE_NEMO,
	MINI_GAME_TYPE_PUSH,
	MINI_GAME_TYPE_ARROW,
};

class QUEST_INFO : public MQuestInfo
{
public :
	QUEST_INFO();
	~QUEST_INFO();

	DWORD			GetCreatureType()	{ return m_CreatureType; }
	DWORD			GetSpriteType()		{ return m_SpriteType;}
	DWORD			GetGoal()			{ return m_Goal; }
	
	
	DWORD			GetItemClass()		{ return m_CreatureType; }
	DWORD			GetItemType()		{ return m_SpriteType; }
	const char*		GetName()			{ return m_Name.GetString(); }

	MINI_GAME_TYPE	GetGameType()		{ return (MINI_GAME_TYPE)m_CreatureType; }
	
	bool			IsChief()		{ return m_Chief == 1;}
	
	void			LoadFromFile(std::ifstream& file);
	void			SaveToFile(std::ofstream& file);

	BYTE			GetQuestLevel() { return m_QuestLevel; }
	BYTE			GetEventQuest()	{ return m_EventQuest; }

	void			SetQuestLevel(BYTE level) { m_QuestLevel = level; }
	void			SetEventQuest(BYTE event) { m_EventQuest = event; }

	
	void			SetGoal(DWORD goal) { m_Goal = goal; }	
	void			SetName(const char* str) { m_Name = str; }
private :
	DWORD			m_CreatureType;
	DWORD			m_SpriteType;
	char			m_Chief;
	DWORD			m_Goal;
	BYTE			m_EventQuest;
	BYTE			m_QuestLevel;
	MString			m_Name;	
};

class MQuestInfoManager : public CTypeMap<QUEST_INFO>
{
public :
	MQuestInfoManager();
	~MQuestInfoManager();

	void					Init();
	QUEST_INFO*				GetInfo(DWORD id) ;
	void					SetInfo(DWORD id, DWORD goal, DWORD time, std::string str);
	void LoadFromFile(std::ifstream& file);

};

extern		MQuestInfoManager	*g_pQuestInfoManager;					

#endif