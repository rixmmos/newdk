//----------------------------------------------------------------------
// CSpriteSetManager.cpp
//----------------------------------------------------------------------		
#include "client_PCH.h"
#include "CSpriteSetManager.h"
#include <cstdint>
//----------------------------------------------------------------------		
//
// member functions
//
//----------------------------------------------------------------------		
CSpriteSetManager::CSpriteSetManager()
{
}

CSpriteSetManager::~CSpriteSetManager()
{
}

//----------------------------------------------------------------------		
//
// member functions
//
//----------------------------------------------------------------------		

//----------------------------------------------------------------------
// Save FilePosition
//----------------------------------------------------------------------				

//


//


//----------------------------------------------------------------------
bool		
CSpriteSetManager::SaveSpriteSetIndex(ofstream& setIndex, ifstream& spkIndex)
{
	
	if (m_List.size() == 0)
		return false;


	TYPE_SPRITEID	count;

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	spkIndex.read((char*)&count, SIZE_SPRITEID);

	
	int32_t* pIndex = new int32_t [count];

	//---------------------------------------------------------------
	
	//---------------------------------------------------------------
	for (TYPE_SPRITEID i=0; i<count; i++)
	{
		pIndex[i] = 0;
		spkIndex.read((char*)&pIndex[i], 4);
	}

	//---------------------------------------------------------------
	
	
	//---------------------------------------------------------------
	DATA_LIST::iterator iData = m_List.begin();

	
	count = m_List.size();
	setIndex.write((const char*)&count, SIZE_SPRITEID);

	
	while (iData != m_List.end())
	{
		
		setIndex.write((const char*)&pIndex[(*iData)], 4);

		iData ++;
	}

	delete [] pIndex;

	return true;
}

