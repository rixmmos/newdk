//---------------------------------------------------------------------------
// MNPCScriptTable.h
//---------------------------------------------------------------------------
 
//---------------------------------------------------------------------------

#ifndef __MNPCSCRIPTTABLE_H__
#define	__MNPCSCRIPTTABLE_H__

#include "CTypeTable.h"
#include "CTypeMap2.h"
#include "MString.h"
#include "Packet/ScriptParameter.h"

typedef std::map<std::string,ScriptParameter*>			HashMapScriptParameter;


//---------------------------------------------------------------------------
// NPC_SCRIPT
//---------------------------------------------------------------------------
class NPC_SCRIPT {
	public :
		typedef CTypeTable<MString>		STRING_TABLE;		

	public :		
		MString				OwnerID;

		STRING_TABLE		SubjectTable;	
		STRING_TABLE		ContentTable;

	public :
		int					GetSubjectSize() const	{ return SubjectTable.GetSize(); }
		int					GetContentSize() const	{ return ContentTable.GetSize(); }

		void				SaveToFile(std::ofstream& file);
		void				LoadFromFile(std::ifstream& file);
};

//---------------------------------------------------------------------------
// MNPCScriptTable
//---------------------------------------------------------------------------
class MNPCScriptTable : public CTypeMap2<NPC_SCRIPT> {
	public :

		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);

		int			GetSubjectSize(int scriptID) const;
		int			GetContentSize(int scriptID) const;
	
		const char*	GetSubject(int scriptID, int subjectID) const;
		const char*	GetContent(int scriptID, int contentID) const;

		
		void GetContentParameter(int scriptID, int contentID,HashMapScriptParameter para,std::string &str);
		void GetSubjectParameter(int scriptID, int subjectID,HashMapScriptParameter para,std::string &str);
};


//---------------------------------------------------------------------------
// global
//---------------------------------------------------------------------------
extern MNPCScriptTable*		g_pNPCScriptTable;

#endif
