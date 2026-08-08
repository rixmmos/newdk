#include "Client_PCH.h"
#include "CSpritePalBase.h"
#include "CSpriteSurface.h"


BYTE CSpritePalBase::s_Colorkey = 0xFF;

CSpritePalBase::CSpritePalBase()
{
	m_Width = 0;		
	m_Height = 0;		
	m_Size = 0;			
	
	m_pPixels = NULL;		// pixels point array
	m_pData = NULL;			// data
	
	m_bInit = false;		
	m_bLoading = false;		
}

CSpritePalBase::~CSpritePalBase()
{
	Release();
}

void CSpritePalBase::Release()
{
	m_Width = 0;		
	m_Height = 0;		
	m_Size = 0;			

	if(m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;			// data
		m_pPixels = NULL;
	}
	
	m_bInit = false;		
	m_bLoading = false;		
}

void CSpritePalBase::SetEmptySprite()
{
	Release();
	m_bInit = true;
}

bool CSpritePalBase::LoadFromFile(std::ifstream &file)
{
	Release();
	
	file.read((char *)&m_Size, 4);


//	if(m_Size == 0)
//		return true;

	file.read((char *)&m_Width, 2);
	file.read((char *)&m_Height, 2);

	m_pData = new BYTE[m_Size+sizeof(BYTE *)*m_Height];
	m_pPixels = (BYTE **)(m_pData+m_Size);

	file.read((char *)m_pData, m_Size);

	register int i;
	
	WORD *indexArray = new WORD[m_Height];

	file.read((char *)indexArray, m_Height<<1);

	BYTE *tempData = m_pData;

	for (int i=0; i<m_Height; i++)
	{
		m_pPixels[i] = tempData;
		tempData += indexArray[i];
	}

	delete []indexArray;

	m_bInit = true;
	
	return true;
}

bool CSpritePalBase::SaveToFile(std::ofstream &file)
{
	if(IsNotInit())
	{
		MessageBox(NULL, "   -_-", "CSpritePalBase", MB_OK);
		return false;
	}

	if(IsEmptySprite())
	{
		return false;
	}

	file.write((const char *)&m_Size, 4);


//	if(m_Size == 0)
//		return true;

	file.write((const char *)&m_Width, 2);
	file.write((const char *)&m_Height, 2);
	file.write((const char *)m_pData, m_Size);

	register int i;

	WORD index;

	for (int i=0; i<m_Height; i++)
	{
		if(i == m_Height -1)
		{
			index = (m_pData+m_Size) - m_pPixels[i];
		}
		else
			index = m_pPixels[i+1] - m_pPixels[i];
		
		file.write((const char*)&index, 2);
	}

	return true;
}

void CSpritePalBase::operator = (const CSpritePalBase& sprite)
{
	
	Release();

	m_Size = sprite.m_Size;
	m_Width = sprite.m_Width;
	m_Height = sprite.m_Height;
	m_bInit = true;

	m_pData = new BYTE[m_Size+sizeof(BYTE *)*m_Height];
	m_pPixels = (BYTE **)(m_pData+m_Size);
	
	memcpy(m_pData, sprite.m_pData, m_Size);

	register int i;
	for(i = 0; i < m_Height; i++)
	{
		m_pPixels[i] = m_pData + (sprite.m_pPixels[i]-sprite.m_pData);
	}
}
