//----------------------------------------------------------------------
// CSpritePackList565.cpp
//----------------------------------------------------------------------

#include "CSprite565.h"
#include "CSpritePackList.h"
#include "CSpritePackList565.h"
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
CSpritePackList565::LoadFromFile(ifstream& file)
{
	
	Release();

	
	TYPE_SPRITEID size;
	file.read((char*)&size, SIZE_SPRITEID);


	CSprite* pSprite;

	
	for (TYPE_SPRITEID i=0; i<size; i++)	
	{			
		
		pSprite = new CSprite565;

		pSprite->LoadFromFile(file);	

		
		AddSprite(pSprite);
	}
}

