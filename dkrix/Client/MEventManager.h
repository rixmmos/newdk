//----------------------------------------------------------------------
// MEventManager.h
//----------------------------------------------------------------------

#ifndef __MEVENT_MANAGER_H__
#define __MEVENT_MANAGER_H__

#pragma warning(disable:4786)

#include "ctypetable.h"
#include "CDirectDrawSurface.h"
#include <map>
#include <vector>

#define	EVENTFLAG_SHOW_STRING				0x00000001		
#define EVENTFLAG_SHOW_DELAY				0x00000002		
#define	EVENTFLAG_SHOW_DELAY_STRING			0x00000003		
#define	EVENTFLAG_SHAKE_SCREEN				0x00000004		
#define	EVENTFLAG_FADE_SCREEN				0x00000008		
#define	EVENTFLAG_NOT_DRAW_BACKGROUND		0x00000010		
#define	EVENTFLAG_EVENT_BACKGROUND			0x00000020		
#define	EVENTFLAG_ONLY_EVENT_BACKGROUND		0x00000030		
#define EVENTFLAG_QUEST_INFO				0x00000040		
#define EVENTFLAG_NOT_DRAW_UI				0x00000080		
#define EVENTFLAG_NOT_DRAW_CREATURE			0x00000100		
#define EVENTFLAG_NOT_DRAW_INFORMATION		0x00000200		
#define EVENTFLAG_NOT_DRAW_CREATURE_SHADOW	0x00000400		
#define EVENTFLAG_NOT_DRAW_ITEM				0x00000800		
#define EVENTFLAG_NOT_DRAW_EFFECT			0x00001000		
#define EVENTFLAG_NOT_DRAW_MOUSE_POINTER	0x00002000		
#define EVENTFLAG_DENY_INPUT_MOUSE			0x00004000		
#define EVENTFLAG_DENY_INPUT_KEYBOARD		0x00008000		
#define EVENTFLAG_DENY_INPUT				0x0000c000		
#define EVENTFLAG_NOT_FADE_SCREEN			0x00010000		
#define EVENTFLAG_NOT_PLAY_SOUND			0x00020000		
#define EVENTFLAG_CLOUD_BACKGROUND			0x00040000		

enum EVENT_ID
{
	EVENTID_NULL,
	EVENTID_HALLUCINATION,			
	EVENTID_KICK_OUT_FROM_ZONE,		
	EVENTID_CONTINUAL_GROUND_ATTACK,	
	EVENTID_COMBAT_MASTER,				
	EVENTID_METEOR,						
	EVENTID_METEOR_SHAKE,				
	EVENTID_PREMIUM_HALF,				
	EVENTID_TAX_CHANGE,					
	EVENTID_LOGOUT,						
	EVENTID_LOVECHAIN,					
	EVENTID_FORCE_LOGOUT_BY_PREMIUM,	
	EVENTID_MONSTER_KILL_QUEST,			
	EVENTID_OUSTERS_FIN,
	EVENTID_QUEST_FIN,					
	EVENTID_POUR_ITEM,					
	EVENTID_RESURRECT,					
	EVENTID_GDR_PRESENT,				
	EVENTID_BG_CLOUD,					
	EVENTID_WAR_EFFECT,
	//add by zdj
	EVENTID_ADVANCEMENT_QUEST_ENDING,
	EVENTID_MAX,
};

enum EVENT_TYPE
{
	EVENTTYPE_NULL,		// 
	EVENTTYPE_ZONE,		
	EVENTTYPE_EFFECT,	

	EVENTTYPE_MAX
};	

enum EVENTBACKGROUND_ID
{
	EVENTBACKGROUNDID_COSMOS,
	EVENTBACKGROUNDID_OUSTERS_SLAYER,
	EVENTBACKGROUNDID_OUSTERS_VAMPIRE,
	EVENTBACKGROUNDID_QUEST_2,
	EVENTBACKGROUNDID_CLOUD,
	
	EVENTBACKGROUNDID_MAX,
};

class MEvent
{
	public:
		MEvent();
		~MEvent();

	public:
		EVENT_ID				eventID;
		EVENT_TYPE				eventType;
		DWORD					eventStartTickCount;
		int						eventDelay;			
		int						showTime;
		int						totalTime;
		DWORD					eventFlag;			
		DWORD					parameter1;			
		DWORD					parameter2;			
		DWORD					parameter3;			
		DWORD					parameter4;			
		std::vector<int>		m_StringsID;		

		const bool IsShowTime() const;
};

class MEventManager
{
	public:
		MEventManager();
		~MEventManager();

		void			ProcessEvent();

		void			AddEvent(MEvent &event);

		const bool		IsEmptyEvent() const				{ return m_Events.empty(); }
		const int		GetEventCount() const				{ return m_Events.size(); }
		const MEvent*	GetEvent(EVENT_ID id);

		const bool		IsEmptyEventByFlag(DWORD flag);
		const MEvent*	GetEventByFlag(DWORD flag, int count = 0);
		const int		GetEventCountByFlag(DWORD flag);

		bool			IsEvent(EVENT_ID id);

		void			RemoveEvent(EVENT_ID id);
		void			RemoveAllEvent();
		void			RemoveAllEventByType(EVENT_TYPE type);

		bool					AssertEventBackground(EVENTBACKGROUND_ID id);
		CDirectDrawSurface*		GetEventBackground(EVENTBACKGROUND_ID id) { AssertEventBackground(id); return &m_EventBackGround[id]; }
		
		typedef std::map<EVENT_ID, MEvent>		EVENT_MAP;
		
	protected :
		EVENT_MAP		m_Events;
		CTypeTable<CDirectDrawSurface>	m_EventBackGround;	
		
};

//----------------------------------------------------------------------
// global
//----------------------------------------------------------------------
extern MEventManager* g_pEventManager;

#endif
