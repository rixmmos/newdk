///////////////////////////////////////////////////////////////////////////////////
//
//	created:	2003/11/05   14:23
//	filename: 	MHelpMessageManager.h

//	

//
///////////////////////////////////////////////////////////////////////////////////

#ifndef __MHELPMESSAGEMANAGER_H__
#define __MHELPMESSAGEMANAGER_H__

#include "MString.h"
#include "RaceType.h"
#include <vector>
#include "MString.h"
//#include "stdafx.h"
#include <fstream>
#include "RarFile.h"
#include "VS_UI_filepath.h"

enum HELPMESSAGE_ID
{
	HELPMESSAGEID_KEYWORD,
	HELPMESSAGEID_MAX
};



class MHelpMessage
{
public:
	// Variable

	enum MESSAGE_TYPE
	{
		MESSAGETYPE_NORMAL,
		MESSAGETYPE_OPEN,
		MESSAGETYPE_SAFESECTOR_OPEN,
	};

	int		m_messageType;
	MString	m_strKeyword;
	MString	m_strEvent;
	
	
	MString			m_strTitle[RACE_MAX];
	int				m_iLevelMax[RACE_MAX];
	int				m_iLevelLow[RACE_MAX];
	int				m_iDomainMax[RACE_MAX];
	int				m_iDomainLow[RACE_MAX];
	int				m_iAttrMax[RACE_MAX];
	int				m_iAttrLow[RACE_MAX];
	
	int				m_iSender[RACE_MAX];
	MString			m_strDetail[RACE_MAX];

public:
	MHelpMessage();
	virtual ~MHelpMessage();

	// file func
	void SaveToFile(std::ofstream &file);
	void LoadFromFile(std::ifstream &file);
	void SaveToFile(const char * filename);
	void LoadFromFile(const char * filename);
};
  


class MHelpMessageManager 
{
private:
	typedef std::vector<MHelpMessage>	MHELPMESSGAE_TYPE;
	typedef std::vector<MString>		SENDER_TYPE;
	MHELPMESSGAE_TYPE			m_MessageVector;
	SENDER_TYPE					m_SenderVector;

	MHelpMessageManager();


public:
	static MHelpMessageManager& Instance()
	{
		static MHelpMessageManager theInstance;
		return theInstance;
	}

	virtual ~MHelpMessageManager();

	

	// SENDER
	const MString&	getSender(int senderIndex) const	{ return m_SenderVector[senderIndex]; }
	void			addSender(MString& sender)			{ m_SenderVector.push_back(sender); }
	void			removeSender(int senderIndex)		{ m_SenderVector.erase(m_SenderVector.begin() + senderIndex); }
	size_t			getSenderSize() const				{ return m_SenderVector.size(); }

	// ID
	const MHelpMessage&	getMessage(int messageIndex) const	{ return m_MessageVector[messageIndex]; }
	void				addMessage(MHelpMessage& message)	{ m_MessageVector.push_back(message); }
	void				removeMessage(int messageIndex)		{ m_MessageVector.erase(m_MessageVector.begin() + messageIndex); }
	void				updateMessage(int messageIndex, MHelpMessage& message) { m_MessageVector[messageIndex] = message; }
	size_t				getMessageSize() const				{ return m_MessageVector.size(); }

	int							m_KeyCnt;; 
	int							m_SenderCnt;; 
	
	void SaveToFile(std::ofstream &file);
	void LoadFromFile(std::ifstream &file);
	void SaveToFile(const char * filename);
	void LoadFromFile(const char * filename);	
	CRarFile					m_pack_file;  
	bool LoadHelpMessageRpk(const char *helprpkfilename);
};


#endif
