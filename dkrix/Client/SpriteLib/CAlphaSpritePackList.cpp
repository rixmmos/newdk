//----------------------------------------------------------------------
// CAlphaSpritePackList.cpp
//----------------------------------------------------------------------

#include "CAlphaSprite.h"
#include "CAlphaSpritePackList.h"
#include <fstream>

using namespace std;

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------

CAlphaSpritePackList::CAlphaSpritePackList()
{
}

CAlphaSpritePackList::~CAlphaSpritePackList()
{
	
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CAlphaSpritePackList::Release()
{
	if (!m_listSprite.empty())
	{
		
		ALPHASPRITE_LIST::iterator iSprite = m_listSprite.begin();

		
		while (iSprite != m_listSprite.end())
		{
			delete (*iSprite);
			iSprite++;
		}

		m_listSprite.clear();
	}
}

//----------------------------------------------------------------------
// Add Sprite to List
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void		
CAlphaSpritePackList::AddSprite(CAlphaSprite* pSprite)
{
	
	m_listSprite.insert(m_listSprite.end(), pSprite);
}

//----------------------------------------------------------------------
// Remove Sprite
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void		
CAlphaSpritePackList::RemoveSprite(TYPE_SPRITEID n)
{
	
	if ( n >= m_listSprite.size() )
		return;

	ALPHASPRITE_LIST::iterator	iSprite = m_listSprite.begin();

	
	for (TYPE_SPRITEID i=0; i<n; i++)
		iSprite++;

	
	delete (*iSprite);

	
	m_listSprite.erase(iSprite);
}

//----------------------------------------------------------------------
// Add Sprite
//----------------------------------------------------------------------

//

//----------------------------------------------------------------------
void		
CAlphaSpritePackList::AddSprite(TYPE_SPRITEID n, CAlphaSprite* pSprite)
{	
	
	if ( n > m_listSprite.size() )
		n = m_listSprite.size();

	ALPHASPRITE_LIST::iterator	iSprite = m_listSprite.begin();

	
	for (TYPE_SPRITEID i=0; i<n; i++)
		iSprite++;

	
	m_listSprite.insert(iSprite, pSprite);
}

//----------------------------------------------------------------------
// Get Sprite
//----------------------------------------------------------------------


//----------------------------------------------------------------------
CAlphaSprite*
CAlphaSpritePackList::GetSprite(TYPE_SPRITEID n) const
{
	
	if ( n >= m_listSprite.size() )
		return NULL;

	ALPHASPRITE_LIST::const_iterator	iSprite = m_listSprite.begin();

	
	for (TYPE_SPRITEID i=0; i<n; i++)
		iSprite++;

	
	return (*iSprite);
}

//----------------------------------------------------------------------
// Change Sprite
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CAlphaSpritePackList::ChangeSprite(TYPE_SPRITEID n, TYPE_SPRITEID m)
{
	
	if (m==n) return;

	
	if ( n >= m_listSprite.size() || m >= m_listSprite.size())
		return;

	TYPE_SPRITEID temp;

	//------------------------------------------------------
	
	//------------------------------------------------------
	if (n > m) 
	{
		temp = n;
		n = m;
		m = temp;
	}

	ALPHASPRITE_LIST::iterator	iFirstSprite = m_listSprite.begin();	

	//------------------------------------------------------
	
	//------------------------------------------------------
	for (TYPE_SPRITEID i=0; i<n; i++)
		iFirstSprite++;

	//------------------------------------------------------
	
	//------------------------------------------------------
	temp = m-n;
	ALPHASPRITE_LIST::iterator	iSecondSprite = iFirstSprite;
	for (int i=n; i<temp; i++)
		iSecondSprite++;
	
	//------------------------------------------------------
	
	//------------------------------------------------------
	CAlphaSprite*	pSprite = *iFirstSprite;
	*iFirstSprite = *iSecondSprite;
	*iSecondSprite = pSprite;
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CAlphaSpritePackList::SaveToFile(ofstream& spkFile, ofstream& indexFile)
{
	
	if (m_listSprite.empty())
		return FALSE;

	
	TYPE_SPRITEID size = m_listSprite.size();
	spkFile.write((const char *)&size, SIZE_SPRITEID); 
	indexFile.write((const char *)&size, SIZE_SPRITEID);

	//--------------------------------------------------
	
	//--------------------------------------------------
	long*	pIndex = new long [size];


	
	ALPHASPRITE_LIST::iterator iSprite = m_listSprite.begin();

	//--------------------------------------------------
	
	//--------------------------------------------------
	int i = 0;
	while (iSprite != m_listSprite.end())
	{
		
		pIndex[i++] = spkFile.tellp();

		
		(*iSprite)->SaveToFile(spkFile);		
		iSprite ++;
	}
	
	//--------------------------------------------------
	
	//--------------------------------------------------
	for (int i=0; i<size; i++)
	{
		indexFile.write((const char*)&pIndex[i], 4);
	}

	delete [] pIndex;

	return true;
}
