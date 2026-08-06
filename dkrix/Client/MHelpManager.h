//---------------------------------------------------------------------------
// MHelpManager.h
//---------------------------------------------------------------------------
 
//---------------------------------------------------------------------------

#ifndef __MHELPMANAGER_H__
#define	__MHELPMANAGER_H__

#pragma warning(disable:4786)

#include "CTypeTable.h"
#include "MHelpDef.h"
#include "Client_PCH.h"
#include <fstream>
#include <list>
using namespace std;
#ifdef __GAME_CLIENT__
	#include "UserOption.h"
#endif


//---------------------------------------------------------------------------
//
//							MHelpNode
//
//---------------------------------------------------------------------------
class MHelpNode {
	public :
		enum NODE_TYPE
		{
			TYPE_NULL = 0,
			TYPE_COMPARE,
			TYPE_OUTPUT,

			MAX_NODE_TYPE
		};

	public :
		MHelpNode() {}
		virtual ~MHelpNode() {}
	
		virtual NODE_TYPE	GetType() const	= 0;

		//------------------------------------------------------
		// File I/O
		//------------------------------------------------------
		virtual void		SaveToFile(std::ofstream& file) = 0;
		virtual void		LoadFromFile(std::ifstream& file) = 0;

	public :
		
		typedef MHelpNode*	(*FUNCTION_NEWNODE)();

		//-------------------------------------------------------
		
		//-------------------------------------------------------
		static MHelpNode*	NewNode(MHelpNode::NODE_TYPE nodeType);

		
		static FUNCTION_NEWNODE		s_NewNodeClassTable[MAX_NODE_TYPE];
};

//---------------------------------------------------------------------------
//
//							MCompareHelpNode
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class MCompareHelpNode : public MHelpNode {
	public :
		MCompareHelpNode(HELP_COMPARE ct=HC_RANDOM)
		{
			m_pCompareType = ct;
			m_pTrue = m_pFalse = NULL;
		}	

		virtual ~MCompareHelpNode()
		{
			if (m_pTrue!=NULL)	{ delete m_pTrue; }
			if (m_pFalse!=NULL)	{ delete m_pFalse; }
		}

		//-----------------------------------------------------------
		// Get Type
		//-----------------------------------------------------------
		virtual MHelpNode::NODE_TYPE	GetType() const	{ return MHelpNode::TYPE_COMPARE; }

		//-----------------------------------------------------------
		// Get Next
		//-----------------------------------------------------------
		MHelpNode*			GetNext() const;

		//-----------------------------------------------------------
		// Get/Set Compare Node
		//-----------------------------------------------------------
		HELP_COMPARE		GetCompareType() const		{ return m_pCompareType; }
		void				SetTrue(MHelpNode* pNode);
		void				SetFalse(MHelpNode* pNode);
		MHelpNode*			GetTrue() const				{ return m_pTrue; }
		MHelpNode*			GetFalse() const			{ return m_pFalse; }

		//------------------------------------------------------
		// File I/O
		//------------------------------------------------------
		virtual void		SaveToFile(std::ofstream& file);
		virtual void		LoadFromFile(std::ifstream& file);	

	public :
		static MHelpNode*	NewNode()					{ return new MCompareHelpNode; }

	protected :
		HELP_COMPARE		m_pCompareType;			
		MHelpNode*			m_pTrue;				
		MHelpNode*			m_pFalse;				
};

//---------------------------------------------------------------------------
//
//							MOutputHelpNode
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class MOutputHelpNode : public MHelpNode, public std::list<HELP_OUTPUT> {
	public :
		typedef std::list<HELP_OUTPUT>		OUTPUT_LIST;

	public :
		MOutputHelpNode() {}
		virtual ~MOutputHelpNode() {}

		//-----------------------------------------------------------
		// Get Type
		//-----------------------------------------------------------
		virtual MHelpNode::NODE_TYPE	GetType() const		{ return MHelpNode::TYPE_OUTPUT; }

		//-----------------------------------------------------------
		// Get 
		//-----------------------------------------------------------
		HELP_OUTPUT			GetOutputType() const;
		
		//------------------------------------------------------
		// File I/O
		//------------------------------------------------------
		virtual void		SaveToFile(std::ofstream& file);
		virtual void		LoadFromFile(std::ifstream& file);	

	public :
		static MHelpNode*	NewNode()					{ return new MOutputHelpNode; }
};


//---------------------------------------------------------------------------
//
//							MHelpManager
//
//---------------------------------------------------------------------------
class MHelpManager : public CTypeTable<MHelpNode*> {
	public :
		MHelpManager();
		~MHelpManager();

		//------------------------------------------------------
		// Init / Release
		//------------------------------------------------------
		void		Init(int size);		
		void		Release();
		
		//------------------------------------------------------
		// Event Occured
		//------------------------------------------------------
		void		ClearEventOccured();
		bool		IsEventOccured(HELP_EVENT he)		{ return m_EventOccured[he]; }

		//------------------------------------------------------
		// Execute Event
		//------------------------------------------------------
		void		ExecuteEvent(HELP_EVENT he);

		//------------------------------------------------------
		// File I/O
		//------------------------------------------------------
		void		SaveToFile(std::ofstream& file);
		void		LoadFromFile(std::ifstream& file);	

	protected :
		CTypeTable<bool>		m_EventOccured;
};

//---------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------
extern MHelpManager*		g_pHelpManager;

//-----------------------------------------------------------------------------
// Execute Help Event
//-----------------------------------------------------------------------------
extern void		ExecuteHelpEvent(HELP_EVENT he);

#if defined(__GAME_CLIENT__) && defined(OUTPUT_DEBUG)
	#define __BEGIN_HELP_EVENT 		if (g_pUserOption!=NULL && g_pUserOption->UseHelpEvent) {
	#define __END_HELP_EVENT		}
	#define __USE_HELP_EVENT
#else
	#define __BEGIN_HELP_EVENT 		((void)0);
	#define __END_HELP_EVENT 		((void)0);
#endif

#endif

