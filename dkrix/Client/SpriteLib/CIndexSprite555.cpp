//----------------------------------------------------------------------
// CIndexSprite555.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CSpriteSurface.h"
#include "CIndexSprite.h"
#include "CIndexSprite555.h"

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CIndexSprite555::SaveToFile(ofstream& file)
{
	
	file.write((const char*)&m_Width , 2);
	file.write((const char*)&m_Height, 2);

	
	if (m_Pixels==NULL || m_Width==0 || m_Height==0)
		return false;
	
	
	WORD index;	

	int colorCount, transPair, indexCount;

	register int i;
	register int j;
	register int k;

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		transPair = m_Pixels[i][0];
				
		index	= 1;

		
		for (j=0; j<transPair; j++)
		{			
			// transCount = m_Pixels[i][index];
			index++;	
			indexCount = m_Pixels[i][index++];	

			index += indexCount;	

			
			colorCount = m_Pixels[i][index++];

			// m_Pixels[i][index] ~ m_Pixels[i][index+colorCount-1]
			
			for (k=0; k<colorCount; k++)								
			{
				m_Pixels[i][index] = ColorDraw::Convert555to565(m_Pixels[i][index]);
				index++;
			}
		}

		
		file.write((const char*)&index, 2);
		file.write((const char*)m_Pixels[i], index<<1);

		//-------------------------------------------------------
		
		
		
		//-------------------------------------------------------

		
		transPair = m_Pixels[i][0];
				
		WORD	index	= 1;

		for (j=0; j<transPair; j++)
		{			
			// transCount = m_Pixels[i][index];
			index++;	
			indexCount = m_Pixels[i][index++];	

			index += indexCount;	

			
			colorCount = m_Pixels[i][index++];

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
CIndexSprite555::LoadFromFile(ifstream& file)
{
	
	Release();

	
	file.read((char*)&m_Width , 2);
	file.read((char*)&m_Height, 2);	

	
	if (m_Width==0 || m_Height==0) 
	{	
		m_bInit = true;

		return true;
	}
	
	m_Pixels = NULL;
	m_Pixels = new WORD* [m_Height];	
	WORD len;

	register int i;
	register int j;
	register int k;
	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		file.read((char*)&len, 2);
		
		m_Pixels[i] = NULL;
		m_Pixels[i] = new WORD [len];		

		file.read((char*)m_Pixels[i], len<<1);

		
		//-------------------------------------------------------
		
		
		
		//-------------------------------------------------------

		int transPair, colorCount, indexCount;

		
		transPair = m_Pixels[i][0];
				
		WORD	index	= 1;

		for (j=0; j<transPair; j++)
		{			
			// transCount = m_Pixels[i][index];
			index++;	
			indexCount = m_Pixels[i][index++];	

			index += indexCount;	

			
			colorCount = m_Pixels[i][index++];

			// m_Pixels[i][index] ~ m_Pixels[i][index+colorCount-1]
			
			for (k=0; k<colorCount; k++)								
			{
				m_Pixels[i][index] = ColorDraw::Convert565to555(m_Pixels[i][index]);
				index++;
			}
		}
	}	
	
	m_bInit = true;

	return true;
}