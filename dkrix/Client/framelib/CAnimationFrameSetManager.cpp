//----------------------------------------------------------------------
// CAnimationFrameSetManager.cpp
//----------------------------------------------------------------------
#include "Frame_PCH.h"
#include "CFrame.h"
#include "CAnimationFrameSetManager.h"
#include "CFramePack.h"
#include "CSpriteSetManager.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CAnimationFrameSetManager::CAnimationFrameSetManager()
{
}

CAnimationFrameSetManager::~CAnimationFrameSetManager()
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
CAnimationFrameSetManager::SaveSpriteSetIndex(CAnimationFramePack* pAnimationFramePack, 
										  ofstream& setIndex, ifstream& packIndex)
{
	CSpriteSetManager ssm;

	
	FRAME_ARRAY*	pFrameArray;

	DATA_LIST::iterator iData = m_List.begin();

	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	while (iData != m_List.end())
	{
		
		pFrameArray = &((*pAnimationFramePack)[*iData]);

		//--------------------------------------------------------------
		
		
		//--------------------------------------------------------------
		for (int i=0; i<pFrameArray->GetSize(); i++)
		{
			ssm.Add( (*pFrameArray)[i].GetSpriteID() );
		}

		iData++;
	}

	//------------------------------------------------------------------
	
	
	
	//------------------------------------------------------------------
	return ssm.SaveSpriteSetIndex(setIndex, packIndex);
}

