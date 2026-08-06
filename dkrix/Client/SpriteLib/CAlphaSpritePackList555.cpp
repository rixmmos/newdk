//----------------------------------------------------------------------
// CAlphaSpritePackList555.cpp
//----------------------------------------------------------------------

#include "CAlphaSprite555.h"
#include "CAlphaSpritePackList.h"
#include "CAlphaSpritePackList555.h"
#include <fstream>

using namespace std;

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CAlphaSpritePackList555::LoadFromFile(ifstream& file)
{
	
	Release();

	
	TYPE_SPRITEID size;
	file.read((char*)&size, SIZE_SPRITEID);


	CAlphaSprite* pSprite;

	
	for (TYPE_SPRITEID i=0; i<size; i++)	
	{			
		
		pSprite = new CAlphaSprite555;

		pSprite->LoadFromFile(file);	

		
		AddSprite(pSprite);
	}
}

