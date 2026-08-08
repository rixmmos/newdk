//----------------------------------------------------------------------
// CSprite565.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CSprite.h"
#include "CSprite565.h"

//extern BYTE	LOADING_STATUS_NONE;
//extern BYTE	LOADING_STATUS_NOW;
//extern BYTE	LOADING_STATUS_LOADING;

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CSprite565::SaveToFile(ofstream& file)
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

			index += colorCount;	
		}

		
		file.write((const char*)&index, 2);
		file.write((const char*)m_Pixels[i], index<<1);
	}


	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CSprite565::LoadFromFile(ifstream& file)
{
	
	if (m_bLoading) 
	{	
		return false;
	}
	m_bLoading = true;

	// F --> T
	 

	
	m_bLoading = true;
	

	
	Release();

	//long start = file.tellg();

	
	file.read((char*)&m_Width , 2);
	file.read((char*)&m_Height, 2);	


	
	if (m_Width==0 || m_Height==0) 
	{	
		m_bInit = true;

		m_bLoading = false;

		return true;
	}

	//---------------------------------
	// for OLD version of CSprite
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

		// len is the number of WORDs, not bytes
		// Allocate len WORDs (len * sizeof(WORD) bytes)
		m_Pixels[i] = NULL;
		m_Pixels[i] = new WORD [len];
		//m_Pixels[i] = (WORD*)malloc(sizeof(WORD)*len);

		// Read len * sizeof(WORD) bytes (which is len << 1)
		// But validate len to prevent buffer overflow
		if (len > 0 && len <= 8192) {  // Sanity check: max 8192 WORDs per line
			file.read((char*)m_Pixels[i], len << 1);
		} else {
			// Invalid length - allocate but don't read
			memset(m_Pixels[i], 0, len * sizeof(WORD));
		}
	}	
	
	m_bInit = true;

	m_bLoading = false;

	return true;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
 