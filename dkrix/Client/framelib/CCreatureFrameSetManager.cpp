//----------------------------------------------------------------------
// CCreatureFrameSetManager.cpp
//----------------------------------------------------------------------
#include "Frame_PCH.h"
#include "CFrame.h"
#include "CCreatureFrameSetManager.h"
#include "CFramePack.h"
#include "CSpriteSetManager.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CCreatureFrameSetManager::CCreatureFrameSetManager()
{
}

CCreatureFrameSetManager::~CCreatureFrameSetManager()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Save SpriteSet Index
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
CCreatureFrameSetManager::SaveSpriteSetIndex(CCreatureFramePack* pCreatureFramePack, 
											 ofstream& setIndex, ifstream& packIndex)
{
	CSpriteSetManager ssm;

	
	ACTION_FRAME_ARRAY*		pActionArray;
	DIRECTION_FRAME_ARRAY*	pDirectionArray ;
	FRAME_ARRAY*			pFrameArray;

	DATA_LIST::iterator iData = m_List.begin();

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	while (iData != m_List.end())
	{
		
		pActionArray = &((*pCreatureFramePack)[*iData]);

		//--------------------------------------------------------------
		
		
		
		
		//--------------------------------------------------------------
		for (int action=0; action<pActionArray->GetSize(); action++)
		{
			pDirectionArray = &((*pActionArray)[action]);
			for (int direction=0; direction<pDirectionArray->GetSize(); direction++)
			{
				pFrameArray = &((*pDirectionArray)[direction]);
				for (int frame=0; frame<pFrameArray->GetSize(); frame++)
				{
					ssm.Add( (*pActionArray)[action][direction][frame].GetSpriteID() );
				}
			}
		}

		iData++;
	}

	//------------------------------------------------------------------
	
	
	
	//------------------------------------------------------------------
	return ssm.SaveSpriteSetIndex(setIndex, packIndex);
}

