//--------------------------------------------------------------------------
// TalkBox.h
//--------------------------------------------------------------------------
// 
// PCTalkBox
//
// NPCTalkBox
//
//--------------------------------------------------------------------------

#ifndef __TALKBOX_H__
#define __TALKBOX_H__

#include "MStringList.h"
#include <vector>

class PCTalkBox : public MStringList
{
	public :
		enum TALKBOX_TYPE
		{
			NORMAL,
			SKILL_LEARN,
			SELECT_QUEST,
			BLOOD_BIBLE_SIGN,
		};

	public :
		PCTalkBox();

		//----------------------------------------------------------------
		// Set Type
		//----------------------------------------------------------------
		void				SetType( TALKBOX_TYPE type )		{ m_Type = type; }
		TALKBOX_TYPE		GetType() const						{ return m_Type; }

		//----------------------------------------------------------------
		// content
		//----------------------------------------------------------------
		void				SetContent(const char* content)		{ m_Content = content; }
		const char*			GetContent() const					{ return m_Content.GetString(); }

		//----------------------------------------------------------------
		// NPC ID
		//----------------------------------------------------------------
		void				SetNPCID(unsigned int id)			{ m_NPCID = id; }
		unsigned int		GetNPCID() const					{ return m_NPCID; }

		//----------------------------------------------------------------
		// NPC ID
		//----------------------------------------------------------------
		void				SetCreatureType(unsigned int id)			{ m_CreatureType = id; }
		unsigned int		GetCreatureType() const					{ return m_CreatureType; }

		//----------------------------------------------------------------
		// Script ID
		//----------------------------------------------------------------
		void				SetScriptID(unsigned int sid)		{ m_ScriptID = sid; }
		unsigned int		GetScriptID() const					{ return m_ScriptID; }

		//----------------------------------------------------------------
		// Answer ID
		//----------------------------------------------------------------
		void				SetAnswerID(unsigned int id)		{ m_AnswerID = id; }
		unsigned int		GetAnswerID() const					{ return m_AnswerID; }
			
		std::vector<int>	m_AnswerIDMap;	

	protected :
		TALKBOX_TYPE		m_Type;

		MString				m_Content;		

		unsigned int		m_NPCID;		
		unsigned int		m_CreatureType;	
		unsigned int		m_ScriptID;		
		unsigned int		m_AnswerID;		
		
};

extern PCTalkBox*		g_pPCTalkBox;

extern MStringList*		g_pNPCTalkBox;

#endif