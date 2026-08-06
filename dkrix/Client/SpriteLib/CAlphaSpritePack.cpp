//----------------------------------------------------------------------
// CAlphaSpritePack.cpp
//----------------------------------------------------------------------

#include "CSpriteSurface.h"
#include "CAlphaSprite555.h"
#include "CAlphaSprite565.h"
#include "CAlphaSpritePack.h"
#include <fstream>
#include <cstdint>

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CAlphaSpritePack::CAlphaSpritePack()
{
	m_nSprites = 0;
	m_pSprites = NULL;
}

CAlphaSpritePack::~CAlphaSpritePack()
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
CAlphaSpritePack::Init(TYPE_SPRITEID count, bool b565)
{
	
	if (count==0) 
		return;

	
	Release();

	
	m_nSprites = count;

	if (b565)
	{
		m_pSprites = new CAlphaSprite565 [m_nSprites];
	}
	else
	{
		m_pSprites = new CAlphaSprite555 [m_nSprites];
	}
}


//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CAlphaSpritePack::Release()
{
	if (m_pSprites != NULL)
	{
		
		delete [] m_pSprites;
		m_pSprites = NULL;
		
		m_nSprites = 0;
	}
}

//----------------------------------------------------------------------
// Release Part
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
CAlphaSpritePack::ReleasePart(TYPE_SPRITEID firstSpriteID, TYPE_SPRITEID lastSpriteID)
{
	
	if (m_pSprites==NULL
		|| firstSpriteID >= m_nSprites)
		return;

	int last = min(lastSpriteID, m_nSprites-1);

	for (int id=firstSpriteID; id<=last; id++)
	{
		m_pSprites[id].Release();
	}
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CAlphaSpritePack::SaveToFile(ofstream& spkFile, ofstream& indexFile)
{
	
	if (m_nSprites==0 || m_pSprites==NULL)
		return false;
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	long*	pIndex = new long [m_nSprites];


	//--------------------------------------------------
	
	//--------------------------------------------------
	spkFile.write((const char *)&m_nSprites, SIZE_SPRITEID); 
	indexFile.write((const char *)&m_nSprites, SIZE_SPRITEID); 

	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	for (TYPE_SPRITEID i=0; i<m_nSprites; i++)
	{
		
		pIndex[i] = spkFile.tellp();

		
		
		

		m_pSprites[i].SaveToFile(spkFile);		
	}

	//--------------------------------------------------
	
	//--------------------------------------------------
	for (int i=0; i<m_nSprites; i++)
	{
		indexFile.write((const char*)&pIndex[i], 4);
	}

	delete [] pIndex;

	return true;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
CAlphaSpritePack::SaveToFileSpriteOnly(ofstream& spkFile, int32_t &filePosition)
{
	
	if (m_nSprites==0 || m_pSprites==NULL)
		return false;

	
	filePosition = spkFile.tellp();
	
	//--------------------------------------------------
	//
	
	//
	//--------------------------------------------------
	for (TYPE_SPRITEID i=0; i<m_nSprites; i++)
	{
		
		
		

		m_pSprites[i].SaveToFile(spkFile);		
	}

	return true;
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CAlphaSpritePack::LoadFromFile(ifstream& file)
{
	
	Release();

	
	file.read((char*)&m_nSprites, SIZE_SPRITEID);

	
	Init(m_nSprites, ColorDraw::Is565());

	
	for (TYPE_SPRITEID i=0; i<m_nSprites; i++)
	{			
		m_pSprites[i].LoadFromFile(file);	
	}
}


//----------------------------------------------------------------------
// Load From File Part
//----------------------------------------------------------------------

// 


//----------------------------------------------------------------------
void			
CAlphaSpritePack::LoadFromFilePart(ifstream& file, int32_t filePosition,
							  TYPE_SPRITEID firstSpriteID, TYPE_SPRITEID lastSpriteID)
{
	if (firstSpriteID==SPRITEID_NULL || lastSpriteID==SPRITEID_NULL)
		return;

	
	if (m_pSprites==NULL)
		return;

	
	file.seekg(static_cast<std::streamoff>(filePosition), ios::beg);

	
	for (TYPE_SPRITEID id=firstSpriteID; id<=lastSpriteID; id++)
	{
		
		//if (m_pSprites[id].IsInit())
		//	continue;
		

		m_pSprites[id].LoadFromFile( file );
	}
}

//----------------------------------------------------------------------
// LoadFromFile Sprite
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
CAlphaSpritePack::LoadFromFileSprite(int spriteID, int fileSpriteID, std::ifstream& spkFile, std::ifstream& indexFile)
{
	if (spriteID < 0 || spriteID >= m_nSprites)
	{
		return false;
	}

	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	TYPE_SPRITEID num;
	indexFile.read((char*)&num, SIZE_SPRITEID);

	if (fileSpriteID >= num)
	{
		return false;
	}

	//-------------------------------------------------------------------
	
	//-------------------------------------------------------------------
	int32_t fp = 0;
	indexFile.seekg( 2 + fileSpriteID*4 );		// 2(num) + spriteID * (4 bytes)
	indexFile.read((char*)&fp, 4);

	//-------------------------------------------------------------------
	// minimap sprite loading
	//-------------------------------------------------------------------
	spkFile.seekg(static_cast<std::streamoff>(fp));

	m_pSprites[spriteID].LoadFromFile( spkFile );

	return true;
}

//----------------------------------------------------------------------
// LoadFromFile Sprite
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
CAlphaSpritePack::LoadFromFileSprite(int spriteID, int fileSpriteID, const char* spkFilename, const char* indexFilename)
{
	if (spriteID < 0 || spriteID >= m_nSprites)
	{
		return false;
	}

	std::ifstream spkFile(spkFilename, ios::binary);

	if (!spkFile.is_open())
	{
		return false;
	}

	std::ifstream indexFile(indexFilename, ios::binary);

	if (!indexFile.is_open())
	{
		return false;
	}

	return LoadFromFileSprite( spriteID, fileSpriteID, spkFile, indexFile );
}
