//----------------------------------------------------------------------
// CSpritePackList555.cpp
//----------------------------------------------------------------------

#include "CSprite555.h"
#include "CSpritePackList.h"
#include "CSpritePackList555.h"
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
CSpritePackList555::LoadFromFile(ifstream& file)
{
	
	Release();

	
	TYPE_SPRITEID size;
	file.read((char*)&size, SIZE_SPRITEID);


	CSprite* pSprite;

	
	for (TYPE_SPRITEID i=0; i<size; i++)	
	{			
		
		pSprite = new CSprite555;

		pSprite->LoadFromFile(file);	

		
		AddSprite(pSprite);
	}
}

