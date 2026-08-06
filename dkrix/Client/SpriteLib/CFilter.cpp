//----------------------------------------------------------------------
// CFilter.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CFilter.h"
//----------------------------------------------------------------------
//
// contructor/destructor
//
//----------------------------------------------------------------------
CFilter::CFilter()
{
	m_Width		= 0;
	m_Height	= 0;
	m_ppFilter	= NULL;
}

CFilter::~CFilter()
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
CFilter::Init(WORD width, WORD height)
{
	if (width==0 || height==0)
		return;

	
	Release();

	m_Width = width;
	m_Height = height;
	
	
	m_ppFilter = new BYTE* [m_Height];

	for (int i=0; i<m_Height; i++)
	{
		m_ppFilter[i] = new BYTE [m_Width];
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
CFilter::Release()
{
	if (m_ppFilter != NULL)
	{
		for (int i=0; i<m_Height; i++)
			delete [] m_ppFilter[i];

		delete [] m_ppFilter;
		m_ppFilter = NULL;

		m_Width = 0;
		m_Height = 0;
	}
}

//----------------------------------------------------------------------
// Set Filter
//----------------------------------------------------------------------



//


// 
//----------------------------------------------------------------------
void		
CFilter::SetFilter(WORD* pSurface, WORD pitch, WORD width, WORD height)
{
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	if (width!=m_Width || height!=m_Height)
		Init(width, height);

	
	register int i;
	register int j;
			

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	WORD*	pSurfaceTemp;
	BYTE*	pFilter;
	for (int i=0; i<height; i++)
	{
		pSurfaceTemp = pSurface;

		pFilter = m_ppFilter[i];

		j = width;
		do
		{
			*pFilter = (BYTE)(*pSurfaceTemp & 0x1F);	

			pFilter++;
			pSurfaceTemp++;
			
		} while (--j);

		
		pSurface = (WORD*)((BYTE*)pSurface + pitch);
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CFilter::SetFilter(const CFilter& filter)
{
	
	if (filter.m_Width==0 || filter.m_Height==0)
		return;

	
	Init( filter.m_Width, filter.m_Height );

	
	int width2 = m_Width << 1;

	for (int i=0; i<m_Height; i++)
	{
		memcpy((void*)m_ppFilter[i], (void*)filter.m_ppFilter[i], width2);		
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CFilter::SetFilterDarkness(const CFilter& filter, BYTE DarkBits)
{
	
	if (filter.m_Width==0 || filter.m_Height==0)
		return;

	
	Init( filter.m_Width, filter.m_Height );

	register int i;
	register int j;

	
	for (int i=0; i<m_Height; i++)
	{
		for (j=0; j<m_Width; j++)
		{
			m_ppFilter[i][j] = (filter.m_ppFilter[i][j] >> DarkBits);
		}
	}
}

//----------------------------------------------------------------------
// Save To File
//----------------------------------------------------------------------
bool		
CFilter::SaveToFile(ofstream& file)
{
	
	file.write((const char*)&m_Width, 2);
	file.write((const char*)&m_Height, 2);

	
	if (m_Width==0 || m_Height==0 || m_ppFilter==NULL)
		return false;

	
	for (int i=0; i<m_Height; i++)
	{
		file.write((const char*)m_ppFilter[i], m_Width);
	}
	return true;
}

//----------------------------------------------------------------------
// Load From File
//----------------------------------------------------------------------
bool		
CFilter::LoadFromFile(ifstream& file)
{
	// Size Load
	file.read((char*)&m_Width, 2);
	file.read((char*)&m_Height, 2);

	
	if (m_Width==0 || m_Height==0)
		return false;

	
	Init( m_Width, m_Height );	

	
	for (int i=0; i<m_Height; i++)
	{
		file.read((char*)m_ppFilter[i], m_Width);
	}
	return true;
}

//----------------------------------------------------------------------
// Blt Filter
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CFilter::BltFilter(int x, int y, const CFilter& filter)
{
	BYTE	*pDest;			
	BYTE	*pSource;		

	int startY, endY;						
	int sourceStartX, destStartX, width;	
	
	register int sourceIndex;
	register int destIndex;

	//-------------------------------------------------
	
	//-------------------------------------------------
	
	if (y > m_Height)
	{
		return;	
	}
	
	if (y < 0)
	{		
		
		if (y+filter.m_Height <= 0)
		{
			return;
		}

		
		startY = -y;

		
		if (y+filter.m_Height > m_Height)
		{
			endY = startY + m_Height;
		}
		else
		{
			endY = filter.m_Height;
		}

		destIndex = 0;
	}
	
	
	else if (y+filter.m_Height > m_Height)
	{
		startY = 0;
		endY = m_Height - y;
		destIndex = y;
	}	
	
	else 
	{
		startY = 0;
		endY = filter.m_Height;
		destIndex = y;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	
	if (x > m_Width)
	{
		return;	
	}
	
	if (x < 0)
	{		
		
		if (x+filter.m_Width <= 0)
		{
			return;
		}

		
		if (x+filter.m_Width > m_Width)
		{
			width = m_Width;
		}
		else
		{
			width = filter.m_Width + x;
		}

		
		sourceStartX = -x;
		destStartX = 0;		
	}
	
	
	else if (x+filter.m_Width > m_Width)
	{
		sourceStartX = 0;
		destStartX = x;
		width = m_Width - x;		
	}	
	
	else 
	{
		sourceStartX = 0;		
		destStartX = x;
		width = filter.m_Width;
	}


	//-------------------------------------------------
	
	//-------------------------------------------------
	for (sourceIndex=startY; sourceIndex<endY; destIndex++, sourceIndex++)
	{
		pSource = filter.m_ppFilter[sourceIndex] + sourceStartX;
		pDest	= m_ppFilter[destIndex] + destStartX;

		
		memcpy(pDest, pSource, width);		
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CFilter::BltFilterAdd(int x, int y, const CFilter& filter)
{
	BYTE	*pDest;			
	BYTE	*pSource;		

	int startY, endY;						
	int sourceStartX, destStartX, width;	
	
	register int sourceIndex;
	register int destIndex;

	//-------------------------------------------------
	
	//-------------------------------------------------
	
	if (y > m_Height)
	{
		return;	
	}
	
	if (y < 0)
	{		
		
		if (y+filter.m_Height <= 0)
		{
			return;
		}

		
		startY = -y;

		
		if (y+filter.m_Height > m_Height)
		{
			endY = startY + m_Height;
		}
		else
		{
			endY = filter.m_Height;
		}

		destIndex = 0;
	}
	
	
	else if (y+filter.m_Height > m_Height)
	{
		startY = 0;
		endY = m_Height - y;
		destIndex = y;
	}	
	
	else 
	{
		startY = 0;
		endY = filter.m_Height;
		destIndex = y;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	
	if (x > m_Width)
	{
		return;	
	}
	
	if (x < 0)
	{		
		
		if (x+filter.m_Width <= 0)
		{
			return;
		}

		
		if (x+filter.m_Width > m_Width)
		{
			width = m_Width;
		}
		else
		{
			width = filter.m_Width + x;
		}

		
		sourceStartX = -x;
		destStartX = 0;		
	}
	
	
	else if (x+filter.m_Width > m_Width)
	{
		sourceStartX = 0;
		destStartX = x;
		width = m_Width - x;		
	}	
	
	else 
	{
		sourceStartX = 0;		
		destStartX = x;
		width = filter.m_Width;
	}


	//int d, s, t;
	int t;
	//-------------------------------------------------
	
	//-------------------------------------------------
	for (sourceIndex=startY; sourceIndex<endY; destIndex++, sourceIndex++)
	{
		pSource = filter.m_ppFilter[sourceIndex] + sourceStartX;
		pDest	= m_ppFilter[destIndex] + destStartX;

		//------------------------------------------------------------	
		// Lighten : min(source, dest)
		
		//------------------------------------------------------------
		
		//memcpyLighten( pDest, pSource, width );		
		register int i = width;
		
		if (i>0)
		{
			do
			{
				///*
				//s = *pSource;
				//d = *pDest;	
				
				t = *pDest + *pSource;
				if (t > 31)
				{
					*pDest = 31;
				}
				else
				{
					*pDest = t;
				}		
					
				pDest++;
				pSource++;		
			} while(--i);
		}
	}

}

//----------------------------------------------------------------------
// Blt Filter Subtract
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CFilter::BltFilterSub(int x, int y, const CFilter& filter)
{
	BYTE	*pDest;			
	BYTE	*pSource;		

	int startY, endY;						
	int sourceStartX, destStartX, width;	
	
	register int sourceIndex;
	register int destIndex;

	//-------------------------------------------------
	
	//-------------------------------------------------
	
	if (y > m_Height)
	{
		return;	
	}
	
	if (y < 0)
	{		
		
		if (y+filter.m_Height <= 0)
		{
			return;
		}

		
		startY = -y;

		
		if (y+filter.m_Height > m_Height)
		{
			endY = startY + m_Height;
		}
		else
		{
			endY = filter.m_Height;
		}

		destIndex = 0;
	}
	
	
	else if (y+filter.m_Height > m_Height)
	{
		startY = 0;
		endY = m_Height - y;
		destIndex = y;
	}	
	
	else 
	{
		startY = 0;
		endY = filter.m_Height;
		destIndex = y;
	}

	//-------------------------------------------------
	
	//-------------------------------------------------
	
	if (x > m_Width)
	{
		return;	
	}
	
	if (x < 0)
	{		
		
		if (x+filter.m_Width <= 0)
		{
			return;
		}

		
		if (x+filter.m_Width > m_Width)
		{
			width = m_Width;
		}
		else
		{
			width = filter.m_Width + x;
		}

		
		sourceStartX = -x;
		destStartX = 0;		
	}
	
	
	else if (x+filter.m_Width > m_Width)
	{
		sourceStartX = 0;
		destStartX = x;
		width = m_Width - x;		
	}	
	
	else 
	{
		sourceStartX = 0;		
		destStartX = x;
		width = filter.m_Width;
	}


	//int d, s, t;
	int t, d, s;
	//-------------------------------------------------
	
	//-------------------------------------------------
	for (sourceIndex=startY; sourceIndex<endY; destIndex++, sourceIndex++)
	{
		pSource = filter.m_ppFilter[sourceIndex] + sourceStartX;
		pDest	= m_ppFilter[destIndex] + destStartX;

		//------------------------------------------------------------	
		// Lighten : min(source, dest)
		
		//------------------------------------------------------------
		
		//memcpyLighten( pDest, pSource, width );		
		register int i = width;
		
		if (i>0)
		{
			do
			{
				///*
				//s = *pSource;
				//d = *pDest;	
				d = *pDest;
				s = *pSource;
				
				t = d - s;
				if (t < 0)
				{
					*pDest = 0;
				}
				else
				{
					*pDest = (BYTE)t;
				}

				 

				//*pDest = min( *pDest, *pSource );				
					
				pDest++;
				pSource++;		
			} while(--i);
		}
	}

}

//----------------------------------------------------------------------
// Blt 4444
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
void
CFilter::Blt4444(WORD* pSurface, WORD pitch)
{
	register int i;
	register int j;

	BYTE* pFilter;
	WORD* pSurfaceTemp;

	for (int i=0; i<m_Height; i++)
	{
		pSurfaceTemp = pSurface;
		pFilter = m_ppFilter[i];

		j = m_Width;
		do 
		{
			
			//*pSurfaceTemp = (*pFilter << 12) | (*pSurfaceTemp & 0x0FFF);

			*pSurfaceTemp = (*pFilter << 12);
			pSurfaceTemp ++;
			pFilter ++;
		} while (--j);

		pSurface = (WORD*)((BYTE*)pSurface + pitch);
	}
}

//----------------------------------------------------------------------
// Blt 4444
//----------------------------------------------------------------------


//


//----------------------------------------------------------------------
void
CFilter::Blt4444Color(WORD* pSurface, WORD pitch, WORD color)
{
	
	color &= 0x0FFF;

	register int i;
	register int j;

	BYTE* pFilter;
	WORD* pSurfaceTemp;

	for (int i=0; i<m_Height; i++)
	{
		pSurfaceTemp = pSurface;
		pFilter = m_ppFilter[i];

		j = m_Width;
		do 
		{
			
			*pSurfaceTemp = (*pFilter << 12) | color;

			//*pSurfaceTemp = (*pFilter << 12);
			pSurfaceTemp ++;
			pFilter ++;
		} while (--j);

		pSurface = (WORD*)((BYTE*)pSurface + pitch);
	}
}