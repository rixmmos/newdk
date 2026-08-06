//----------------------------------------------------------------------
// CAlphaSprite565.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CAlphaSprite.h"
#include "CAlphaSprite565.h"
#include <fstream>

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CAlphaSprite565::SaveToFile(ofstream& file)
{
	
	file.write((const char*)&m_Width , 2);
	file.write((const char*)&m_Height, 2);

	
	if (m_Pixels==NULL || m_Width==0 || m_Height==0)
		return false;
	
	
	WORD index;	

	register int i;
	register int j;

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		int	count = m_Pixels[i][0], 
				colorCount;
		index	= 1;

		
		for (j=0; j<count; j++)
		{
			//transCount = m_Pixels[i][index];
			colorCount = m_Pixels[i][index+1];

			index+=2;	

			index += (colorCount<<1);	
		}

		
		file.write((const char*)&index, 2);
		file.write((const char*)m_Pixels[i], index<<1);
	}


	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CAlphaSprite565::LoadFromFile(ifstream& file)
{
	
	Release();

	
	file.read((char*)&m_Width , 2);
	file.read((char*)&m_Height, 2);	

	
	if (m_Width==0 || m_Height==0) 
	{	
		m_bInit = true;

		return true;
	}

	//---------------------------------
	// for OLD version of CAlphaSprite
	//---------------------------------
	//BOOL dummy;
	//file.read((char*)&dummy, 1);	
	//---------------------------------
	
	m_Pixels = NULL;
	m_Pixels = new WORD* [m_Height];	
	//m_Pixels = (WORD**)malloc(sizeof(WORD*)*m_Height);

	WORD len;


	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		file.read((char*)&len, 2);
		
		m_Pixels[i] = NULL;
		m_Pixels[i] = new WORD [len];		
		//m_Pixels[i] = (WORD*)malloc(sizeof(WORD)*len);		

		file.read((char*)m_Pixels[i], len<<1);
	}	
	
	m_bInit = true;

	return true;
}

