//----------------------------------------------------------------------
// CAlphaSpritePackList565.cpp
//----------------------------------------------------------------------

#include "CAlphaSprite565.h"
#include "CAlphaSpritePackList.h"
#include "CAlphaSpritePackList565.h"
#include <fstream>

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
CAlphaSpritePackList565::LoadFromFile(ifstream& file)
{
	
	Release();

	
	TYPE_SPRITEID size;
	file.read((char*)&size, SIZE_SPRITEID);


	CAlphaSprite* pSprite;

	
	for (TYPE_SPRITEID i=0; i<size; i++)	
	{			
		
		pSprite = new CAlphaSprite565;

		pSprite->LoadFromFile(file);	

		
		AddSprite(pSprite);
	}
}

