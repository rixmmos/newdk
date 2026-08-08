//----------------------------------------------------------------------
// CSprite555.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CSpriteSurface.h"
#include "CSprite.h"
#include "CSprite555.h"

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
CSprite555::SaveToFile(ofstream& file)
{
	
	file.write((const char*)&m_Width , 2);
	file.write((const char*)&m_Height, 2);

	
	if (m_Pixels==NULL || m_Width==0 || m_Height==0)
		return false;
	
	
	WORD index;	

	register int i;
	register int j;
	register int k;

	//--------------------------------
	// 5:5:5
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

			// m_Pixels[i][index] ~ m_Pixels[i][index+colorCount-1]
			
			for (k=0; k<colorCount; k++)								
			{
				m_Pixels[i][index] = ColorDraw::Convert555to565(m_Pixels[i][index]);
				index++;
			}

			
		}

		
		file.write((const char*)&index, 2);			
		file.write((const char*)m_Pixels[i], index<<1);			


		
		index	= 1;
			
		for (j=0; j<count; j++)
		{
			//transCount = m_Pixels[i][index];
			colorCount = m_Pixels[i][index+1];				

			index+=2;	

			// m_Pixels[i][index] ~ m_Pixels[i][index+colorCount-1]
			
			for (k=0; k<colorCount; k++)								
			{					
				m_Pixels[i][index] = ColorDraw::Convert565to555(m_Pixels[i][index]);
				index++;
			}

			
		}
	}

	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CSprite555::LoadFromFile(ifstream& file)
{
	
	if (m_bLoading) 
	{	
		return false;
	}
	m_bLoading = true;

	// F --> T
	 
	
	m_bLoading = true;
	

	

	
	Release();

	
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
	
	m_Pixels = new WORD* [m_Height];

	WORD len;

	//--------------------------------
	// 5:5:5
	//--------------------------------
	
	WORD index;
	int	count, colorCount;

	register int i;
	register int j;
	register int k;

	for (int i=0; i<m_Height; i++)
	{			
		
		file.read((char*)&len, 2);
		m_Pixels[i] = new WORD [len];
		file.read((char*)m_Pixels[i], len<<1);

		count = m_Pixels[i][0];			
		index = 1;

		for (j=0; j<count; j++)
		{
			//transCount = m_Pixels[i][index];
			colorCount = m_Pixels[i][index+1];				

			index+=2;	

			// m_Pixels[i][index] ~ m_Pixels[i][index+colorCount-1]
			
			for (k=0; k<colorCount; k++)								
			{					
				m_Pixels[i][index] = ColorDraw::Convert565to555(m_Pixels[i][index]);
				index++;
			}

			
		}
	}

	m_bInit = true;

	m_bLoading = false;
//	LoadingStatus = LOADING_STATUS_NONE;

	return true;
}


//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

