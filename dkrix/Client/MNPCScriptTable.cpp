//---------------------------------------------------------------------------
// MNPCScriptTable.cpp
//---------------------------------------------------------------------------

#include "Client_PCH.h"
#include "MNPCScriptTable.h"

#include "DebugInfo.h"
#include "Packet/Packet.h"
//#include "Packet/ScriptParameter.h"


#define		SCRIPT_MARK_START	"%("
#define		SCRIPT_MARK_END		")"
//---------------------------------------------------------------------------
// global
//---------------------------------------------------------------------------
MNPCScriptTable*		g_pNPCScriptTable = NULL;

//---------------------------------------------------------------------------
// 
// constructor / destructor
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//
// member functions
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Save To File
//---------------------------------------------------------------------------
void				
NPC_SCRIPT::SaveToFile(std::ofstream& file)
{
	//file.write((const char*)&ScriptID, 4);
	OwnerID.SaveToFile( file );

	SubjectTable.SaveToFile( file );
	ContentTable.SaveToFile( file );
}

//---------------------------------------------------------------------------
// Load From File
//---------------------------------------------------------------------------
void				
NPC_SCRIPT::LoadFromFile(std::ifstream& file)
{
	//file.read((char*)&ScriptID, 4);
	OwnerID.LoadFromFile( file );

	SubjectTable.LoadFromFile( file );
	ContentTable.LoadFromFile( file );
}


//---------------------------------------------------------------------------
//
// MNPCScriptTable
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Get Subject Size
//---------------------------------------------------------------------------
int			
MNPCScriptTable::GetSubjectSize(int scriptID) const
{
	const NPC_SCRIPT* pScript = GetData( scriptID );
	
	if (pScript==NULL)
	{
		return 0;
	}

	return pScript->SubjectTable.GetSize();
}

//---------------------------------------------------------------------------
// Get Content Size
//---------------------------------------------------------------------------
int			
MNPCScriptTable::GetContentSize(int scriptID) const
{
	const NPC_SCRIPT* pScript = GetData( scriptID );

	if (pScript==NULL)
	{
		return 0;
	}

	return pScript->ContentTable.GetSize();
}

//---------------------------------------------------------------------------
// Get Subject
//---------------------------------------------------------------------------
const char*	
MNPCScriptTable::GetSubject(int scriptID, int subjectID) const
{
	const NPC_SCRIPT* pScript = GetData( scriptID );

	if (pScript==NULL)
	{
		return NULL;
	}

	if (subjectID >= pScript->GetSubjectSize())
	{
		return NULL;
	}

	return pScript->SubjectTable[subjectID].GetString();
}

//---------------------------------------------------------------------------
// Get Content
//---------------------------------------------------------------------------
const char*	
MNPCScriptTable::GetContent(int scriptID, int contentID) const
{
	const NPC_SCRIPT* pScript = GetData( scriptID );

	if (pScript==NULL)
	{
		return NULL;
	}

	if (contentID >= pScript->GetContentSize())
	{
		return NULL;
	}

	return pScript->ContentTable[contentID].GetString();
}


//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
void		
MNPCScriptTable::SaveToFile(std::ofstream& file)
{
	TYPE_MAP::iterator iData = begin();

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	int infoSize = size();
	file.write((const char*)&infoSize, 4);

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	while (iData != end())
	{
		unsigned int	id		= (*iData).first;
		NPC_SCRIPT*		pData	= (*iData).second;		

		file.write((const char*)&id, 4);	
		pData->SaveToFile( file );			

		iData ++;
	}
}
		
//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
void		
MNPCScriptTable::LoadFromFile(std::ifstream& file)
{
	//-----------------------------------------------------
	
	//-----------------------------------------------------
	Release();

	//-----------------------------------------------------
	// size 
	//-----------------------------------------------------
	int infoSize;
	file.read((char*)&infoSize, 4);

	//-----------------------------------------------------
	
	//-----------------------------------------------------
	unsigned int id;
	for (int i=0; i<infoSize; i++)
	{		
		file.read((char*)&id, 4);
		NPC_SCRIPT*	pData = new NPC_SCRIPT;

		pData->LoadFromFile( file );

		//-----------------------------------------------------
		
		//-----------------------------------------------------
		if (!AddData( id, pData ))
		{
			delete pData;
		}
	}
}

void
MNPCScriptTable::GetContentParameter(int scriptID, int contentID,HashMapScriptParameter para,std::string& str)
{
	str = GetContent(scriptID, contentID);	

	if(str.empty())
		return;
	
	
 

	int i=0;
	HashMapScriptParameter::const_iterator itr = para.begin();

	while(itr != para.end())
	{
		std::string key;
		key = "%(";
		key+= (*itr).first;
		key+= ")";

		while((i=str.find(key)) != -1)
			str.replace(str.begin()+i,str.begin()+i+key.size(),(*itr).second->getValue());
		itr++;
	}
}

void
MNPCScriptTable::GetSubjectParameter(int scriptID, int subjectID,HashMapScriptParameter para,std::string &str)
{
	str = GetSubject(scriptID, subjectID);	

	if(str.empty())
		return;
	
	 

	int i=0;
	HashMapScriptParameter::const_iterator itr = para.begin();

	while(itr != para.end())
	{
		std::string key;
		key = "%(";
		key+= (*itr).first;
		key+= ")";

		while((i=str.find(key)) != -1)
			str.replace(str.begin()+i,str.begin()+i+key.size(),(*itr).second->getValue());
		itr++;
	}
}
