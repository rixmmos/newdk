//----------------------------------------------------------------------
// CFrameSetManager.cpp
//----------------------------------------------------------------------
#include "Frame_PCH.h"
#include "CFrameSetManager.h"
#include <cstdint>
//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CFrameSetManager::CFrameSetManager()
{
}

CFrameSetManager::~CFrameSetManager()
{
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Save FrameSet Index
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CFrameSetManager::SaveFrameSetIndex(ofstream& setIndex, ifstream& packIndex)
{
	
	if (m_List.size() == 0)
		return false;


	TYPE_FRAMEID	count;

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	packIndex.read((char*)&count, SIZE_FRAMEID);

	
	int32_t* pIndex = new int32_t [count];

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	for (TYPE_FRAMEID i=0; i<count; i++)
	{
		pIndex[i] = 0;
		packIndex.read((char*)&pIndex[i], 4);
	}

	//---------------------------------------------------------------
	
	
	//---------------------------------------------------------------
	DATA_LIST::iterator iData = m_List.begin();

	
	count = m_List.size();
	setIndex.write((const char*)&count, SIZE_FRAMEID);

	
	while (iData != m_List.end())
	{
		
		setIndex.write((const char*)&pIndex[(*iData)], 4);

		iData ++;
	}

	delete [] pIndex;

	return true;
}
