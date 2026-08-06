//---------------------------------------------------------------------------
// MHelpManager.cpp
//---------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MHelpManager.h"

#ifdef __GAME_CLIENT__
	#include "ClientDef.h"
	#include "MCompareManager.h"
	#include "MHelpDisplayer.h"
#include "VS_UI.h"
#endif


//---------------------------------------------------------------------------
// Global
//---------------------------------------------------------------------------
MHelpManager*		g_pHelpManager = NULL;

//-----------------------------------------------------------------------------
// Execute Help Event
//-----------------------------------------------------------------------------
void		
ExecuteHelpEvent(HELP_EVENT he)
{
	#if defined(__GAME_CLIENT__)// && defined(OUTPUT_DEBUG) && defined(_DEBUG)
		if (g_Mode==MODE_GAME)
		{
			gC_vs_ui.AddHelpMail(he);//, g_pPlayer->IsInSafeSector());
		//	g_pHelpManager->ExecuteEvent( he );
		}
	#endif
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
MHelpNode::FUNCTION_NEWNODE
MHelpNode::s_NewNodeClassTable[MAX_NODE_TYPE] =
{
	NULL,
	MCompareHelpNode::NewNode,
	MOutputHelpNode::NewNode
};

//---------------------------------------------------------------------------
//
//							MHelpNode
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// New Node
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
MHelpNode*		
MHelpNode::NewNode(MHelpNode::NODE_TYPE nodeType)
{
	if (s_NewNodeClassTable[nodeType]==NULL)
	{
		return NULL;
	}

	return (MHelpNode*)(*s_NewNodeClassTable[nodeType])();
};


//---------------------------------------------------------------------------
//
//							MCompareHelpNode
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Get Next
//---------------------------------------------------------------------------
MHelpNode*
MCompareHelpNode::GetNext() const
{
	
	

#ifdef __GAME_CLIENT__
	if (g_pCompareManager!=NULL 
		&& g_pCompareManager->Compare( m_pCompareType ))
	{
		return m_pTrue;
	}
#endif	
	
	return m_pFalse;	
}

//---------------------------------------------------------------------------
// Set True
//---------------------------------------------------------------------------
void				
MCompareHelpNode::SetTrue(MHelpNode* pNode)
{
	if (m_pTrue!=NULL)
	{
		delete m_pTrue;
	}

	m_pTrue = pNode;
}

//---------------------------------------------------------------------------
// Set False
//---------------------------------------------------------------------------
void				
MCompareHelpNode::SetFalse(MHelpNode* pNode)
{
	if (m_pFalse!=NULL)
	{
		delete m_pFalse;
	}

	m_pFalse = pNode;
}

//---------------------------------------------------------------------------
// Save To File
//---------------------------------------------------------------------------
void		
MCompareHelpNode::SaveToFile(std::ofstream& file)
{
	//---------------------------------------------------
	
	//---------------------------------------------------
	WORD compareType = m_pCompareType;			
	file.write((const char*)&compareType, 2);
	
	BYTE classType;

	//---------------------------------------------------
	
	//---------------------------------------------------
	if (m_pTrue==NULL)
	{
		classType = 0;
		file.write((const char*)&classType, 1);
	}
	else
	{
		classType = m_pTrue->GetType();
		file.write((const char*)&classType, 1);

		m_pTrue->SaveToFile( file );
	}
	
	//---------------------------------------------------
	
	//---------------------------------------------------
	if (m_pFalse==NULL)
	{
		classType = 0;
		file.write((const char*)&classType, 1);
	}
	else
	{
		classType = m_pFalse->GetType();
		file.write((const char*)&classType, 1);

		m_pFalse->SaveToFile( file );
	}
}

//---------------------------------------------------------------------------
// Load From File
//---------------------------------------------------------------------------
void		
MCompareHelpNode::LoadFromFile(std::ifstream& file)
{
	//---------------------------------------------------
	
	//---------------------------------------------------
	WORD compareType;
	file.read((char*)&compareType, 2);
	m_pCompareType = (HELP_COMPARE)compareType;

	BYTE classType;

	//---------------------------------------------------
	
	//---------------------------------------------------
	if (m_pTrue!=NULL)	{ delete m_pTrue; }
	if (m_pFalse!=NULL)	{ delete m_pFalse; }

	//---------------------------------------------------
	
	//---------------------------------------------------
	file.read((char*)&classType, 1);

	m_pTrue = MHelpNode::NewNode( (MHelpNode::NODE_TYPE)classType );

	if (m_pTrue!=NULL)
	{
		m_pTrue->LoadFromFile( file );
	}
	
	//---------------------------------------------------
	
	//---------------------------------------------------
	file.read((char*)&classType, 1);

	m_pFalse = MHelpNode::NewNode( (MHelpNode::NODE_TYPE)classType );

	if (m_pFalse!=NULL)
	{
		m_pFalse->LoadFromFile( file );
	}
}

//---------------------------------------------------------------------------
//
//							MOutputHelpNode
//
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Get OutputType
//---------------------------------------------------------------------------
HELP_OUTPUT			
MOutputHelpNode::GetOutputType() const
{
	int numOutputID = size();

	if (numOutputID == 0)
	{
		return HELP_OUTPUT_NULL;
	}

	int select = rand()%numOutputID;
	
	const_iterator	iID = begin();

	
	for (int i=0; i<select; i++)
	{
		iID++;
	}

	return *iID;
}

//---------------------------------------------------------------------------
// Save To File
//---------------------------------------------------------------------------
void		
MOutputHelpNode::SaveToFile(std::ofstream& file)
{
	//-----------------------------------------------
	
	//-----------------------------------------------
	int numID = size();
	file.write((const char*)&numID, 4);

	//-----------------------------------------------
	
	//-----------------------------------------------
	iterator iID = begin();
	while (iID != end())
	{
		WORD id = *iID;

		file.write((const char*)&id, 2);

		iID++;
	}
}

//---------------------------------------------------------------------------
// Load From File
//---------------------------------------------------------------------------
void		
MOutputHelpNode::LoadFromFile(std::ifstream& file)
{
	//-----------------------------------------------
	
	//-----------------------------------------------
	int numID;
	file.read((char*)&numID, 4);

	//-----------------------------------------------
	
	//-----------------------------------------------
	WORD id;

	clear();

	for (int i=0; i<numID; i++)
	{		
		file.read((char*)&id, 2);

		push_back( (HELP_OUTPUT)id );
	}
}

//---------------------------------------------------------------------------
//
//							MHelpManager
//
//---------------------------------------------------------------------------
MHelpManager::MHelpManager()
{
}

MHelpManager::~MHelpManager()
{
	Release();
}

//---------------------------------------------------------------------------
// Init
//---------------------------------------------------------------------------
void		
MHelpManager::Init(int size)
{
	//-------------------------------------------------------
	// base class's Init
	//-------------------------------------------------------
	CTypeTable<MHelpNode*>::Init( size );

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	m_EventOccured.Init( size );

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	for (int i=0; i<m_Size; i++)
	{
		m_pTypeInfo[i] = NULL;

		m_EventOccured[i] = false;
	}
	
}

//---------------------------------------------------------------------------
// Release
//---------------------------------------------------------------------------
void		
MHelpManager::Release()
{
	for (int i=0; i<m_Size; i++)
	{
		if (m_pTypeInfo[i] != NULL)
		{
			delete m_pTypeInfo[i];
			m_pTypeInfo[i] = NULL;
		}
	}

	// base class's Release
	CTypeTable<MHelpNode*>::Release();

	m_EventOccured.Release();
}

//---------------------------------------------------------------------------
// Clear EventOccured
//---------------------------------------------------------------------------
void		
MHelpManager::ClearEventOccured()
{
	for (int i=0; i<m_EventOccured.GetSize(); i++)
	{
		m_EventOccured[i] = false;
	}
}

//---------------------------------------------------------------------------
// Save To File
//---------------------------------------------------------------------------
void		
MHelpManager::SaveToFile(std::ofstream& file)
{
	//---------------------------------------------------------
	
	//---------------------------------------------------------
	file.write((const char*)&m_Size, 4);

	
	if (m_pTypeInfo==NULL)
		return;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	BYTE classType;

	for (int i=0; i<m_Size; i++)
	{
		MHelpNode* pNode = m_pTypeInfo[i];

		if (pNode==NULL)
		{
			classType = 0;
			file.write((const char*)&classType, 1);
		}
		else
		{
			classType = pNode->GetType();
			file.write((const char*)&classType, 1);

			pNode->SaveToFile( file );
		}		
	}
}

//---------------------------------------------------------------------------
// Load From File
//---------------------------------------------------------------------------
void		
MHelpManager::LoadFromFile(std::ifstream& file)
{
	int size;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	file.read((char*)&size, 4);

	
	if (m_Size != size)
	{
		
		Release();

		
		Init( size );
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	BYTE classType;
	for (int i=0; i<m_Size; i++)
	{
		file.read((char*)&classType, 1);

		MHelpNode* pNode = MHelpNode::NewNode( (MHelpNode::NODE_TYPE)classType );

		if (pNode!=NULL)
		{
 			pNode->LoadFromFile( file );

			m_pTypeInfo[i] = pNode;
		}
	}
}


//---------------------------------------------------------------------------
// Execute Event ( HELP_EVENT )
//---------------------------------------------------------------------------
void
MHelpManager::ExecuteEvent(HELP_EVENT he)
{
	if (m_pTypeInfo[he]!=NULL)
	{
		MHelpNode* pHelpNode = m_pTypeInfo[he];

		while (pHelpNode!=NULL)
		{
			switch (pHelpNode->GetType())
			{
				//------------------------------------------------------------
				
				//------------------------------------------------------------
				case MHelpNode::TYPE_COMPARE :
				{
					
					pHelpNode = ((MCompareHelpNode*)pHelpNode)->GetNext();			
				}
				break;

				//------------------------------------------------------------
				
				//------------------------------------------------------------
				case MHelpNode::TYPE_OUTPUT :
				{
					HELP_OUTPUT ho = ((MOutputHelpNode*)pHelpNode)->GetOutputType();

					#ifdef __GAME_CLIENT__
						g_pHelpDisplayer->OutputHelp( ho );					
					#endif

					pHelpNode = NULL;	
				}
				break;
			}
		}
	}
}

