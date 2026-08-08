//----------------------------------------------------------------------
// CSpriteSet.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CSpriteSurface.h"
#include "CSprite565.h"
#include <cstdint>
#include "CSprite555.h"
#include "CSpritePack.h"
#include "CSpriteSet.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CSpriteSet::CSpriteSet()
{
	m_nSprites		= 0;
	m_pSprites		= NULL;
}

CSpriteSet::~CSpriteSet()
{
	
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
CSpriteSet::Init(TYPE_SPRITEID count)
{
	
	if (count==0) 
		return;

	
	Release();

	
	m_nSprites = count;

	if (ColorDraw::Is565())
	{
		m_pSprites = new CSprite565 [m_nSprites];
	}
	else
	{
		m_pSprites = new CSprite555 [m_nSprites];
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CSpriteSet::Release()
{
	if (m_pSprites != NULL)
	{
		
		delete [] m_pSprites;
		m_pSprites = NULL;

		m_nSprites = 0;
	}
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------


//----------------------------------------------------------------------
bool		
CSpriteSet::LoadFromFile(ifstream& indexFile, ifstream& packFile)
{
	TYPE_SPRITEID	count;
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	indexFile.read((char*)&count, SIZE_SPRITEID);


	int32_t* pIndex = new int32_t [count];	// file position (32-bit)

	//------------------------------------------------------
	
	//------------------------------------------------------
	for (TYPE_SPRITEID i=0; i<count; i++)
	{
		pIndex[i] = 0;
		indexFile.read((char*)&pIndex[i], 4);
	}

	
	
	


	//------------------------------------------------------
	
	//------------------------------------------------------
	Init( count );

	//------------------------------------------------------
	
	
	//------------------------------------------------------
	for (int i=0; i<count; i++)
	{
		packFile.seekg(static_cast<std::streamoff>(pIndex[i]), ios::beg);
		m_pSprites[i].LoadFromFile( packFile );
	}

	delete [] pIndex;

	return true;
}

