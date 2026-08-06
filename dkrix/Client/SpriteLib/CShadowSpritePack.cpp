//----------------------------------------------------------------------
// CShadowSpritePack.cpp
//----------------------------------------------------------------------

#include "CSpriteSurface.h"
#include "CShadowSprite.h"
#include <cstdint>
#include "CShadowSpritePack.h"
#include <fstream>

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CShadowSpritePack::CShadowSpritePack()
{
	m_nSprites = 0;
	m_pSprites = NULL;
}

CShadowSpritePack::~CShadowSpritePack()
{
	
	Release();
	
	m_listLoad.clear();
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
CShadowSpritePack::Init(TYPE_SPRITEID count)
{
	
	if (count==0) 
		return;

	
	Release();

	
	m_nSprites = count;

	m_pSprites = new CShadowSprite [m_nSprites];
}


//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CShadowSpritePack::Release()
{
	if (m_pSprites != NULL)
	{
		
		delete [] m_pSprites;
		m_pSprites = NULL;
		
		m_nSprites = 0;

		m_listLoad.clear();
	}
}

//----------------------------------------------------------------------
// Release Part
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void			
CShadowSpritePack::ReleasePart(TYPE_SPRITEID firstSpriteID, TYPE_SPRITEID lastSpriteID)
{
	
	if (m_pSprites==NULL
		|| firstSpriteID >= m_nSprites)
		return;

	int last = min(lastSpriteID, m_nSprites-1);

	for (TYPE_SPRITEID id=firstSpriteID; id<=last; id++)
	{
		m_pSprites[id].Release();

		
		
	}
}

//----------------------------------------------------------------------
// Release Loaded
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSpritePack::ReleaseLoaded()
{
	INT_LIST::iterator iID = m_listLoad.begin();

	while (iID != m_listLoad.end())
	{
		m_pSprites[*iID].Release();

		iID ++;
	}

	m_listLoad.clear();
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CShadowSpritePack::SaveToFile(ofstream& spkFile, ofstream& indexFile)
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
CShadowSpritePack::SaveToFileSpriteOnly(ofstream& spkFile, int32_t &filePosition)
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
CShadowSpritePack::LoadFromFile(ifstream& file)
{
	
	Release();

	
	file.read((char*)&m_nSprites, SIZE_SPRITEID);

	
	Init(m_nSprites);

	
	for (TYPE_SPRITEID i=0; i<m_nSprites; i++)
	{			
		m_pSprites[i].LoadFromFile(file);	

		
		//m_listLoad.push_back( id );
	}
}

//----------------------------------------------------------------------
// Load From File Part
//----------------------------------------------------------------------

// 


//----------------------------------------------------------------------
void			
CShadowSpritePack::LoadFromFilePart(ifstream& file, int32_t filePosition,
							  TYPE_SPRITEID firstSpriteID, TYPE_SPRITEID lastSpriteID)
{
	if (firstSpriteID==SPRITEID_NULL || lastSpriteID==SPRITEID_NULL)
		return;

	
	if (m_pSprites==NULL)
		return;

	
	file.seekg(static_cast<std::streamoff>(filePosition), ios::beg);

	
	for (TYPE_SPRITEID id=firstSpriteID; id<=lastSpriteID; id++)
	{
		m_pSprites[id].LoadFromFile( file );

		
		m_listLoad.push_back( id );
	}
}

//----------------------------------------------------------------------
// Load From File Part
//----------------------------------------------------------------------

// 


//
//----------------------------------------------------------------------
void			
CShadowSpritePack::LoadFromFilePart(std::ifstream& spkFile, const CSpriteFilePositionArray& fpArray)
{
	
	if (m_pSprites==NULL)
		return;

	for (int i=0; i<fpArray.GetSize(); i++)
	{
		
		spkFile.seekg(static_cast<std::streamoff>(fpArray[i].FilePosition), ios::beg);

		
		m_pSprites[fpArray[i].SpriteID].LoadFromFile( spkFile );

		
		m_listLoad.push_back( fpArray[i].SpriteID );
	}
}

//----------------------------------------------------------------------
// Init( IndexSpritepack )
//----------------------------------------------------------------------
// IndexSpritePack --> ShadowSpritePack
//----------------------------------------------------------------------
void			
CShadowSpritePack::InitPart(CIndexSpritePack& ISPK, TYPE_SPRITEID firstShadowSpriteID, TYPE_SPRITEID lastShadowSpriteID)
{
	
	for (int i=firstShadowSpriteID; i<lastShadowSpriteID; i++)
	{
		m_pSprites[i].SetPixel( ISPK[i] );
	}
}
