//----------------------------------------------------------------------
// CAlphaSprite555.cpp
//----------------------------------------------------------------------
#include "CSpriteSurface.h"
#include "CAlphaSprite.h"
#include "CAlphaSprite555.h"

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
CAlphaSprite555::SaveToFile(ofstream& file)
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
			

			
			index++;
			for (k=0; k<colorCount; k++)								
			{
				m_Pixels[i][index] = ColorDraw::Convert555to565(m_Pixels[i][index]);

				
				
				index+=2;
			}
			index--;

			
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
			
			index++;
			for (k=0; k<colorCount; k++)								
			{					
				m_Pixels[i][index] = ColorDraw::Convert565to555(m_Pixels[i][index]);
				index+=2;
			}
			index--;

			
		}
	}

	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CAlphaSprite555::LoadFromFile(ifstream& file)
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
	
	m_Pixels = new WORD* [m_Height];

	WORD len;

	//--------------------------------
	// 5:5:5
	//--------------------------------
	
	int	count, index, colorCount;

	register int i;
	register int j;

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
			
			index++;	
			for (int j=0; j<colorCount; j++)								
			{					
				m_Pixels[i][index] = ColorDraw::Convert565to555(m_Pixels[i][index]);
				index+=2;
			}
			index--;

			
		}
	}

	m_bInit = true;

	return true;
}


