//----------------------------------------------------------------------
// CStorageSurface.cpp
//----------------------------------------------------------------------
#include "client_PCH.h"
#include "CSpriteSurface.h"
#include "CStorageSurface.h"
//#include "DebugInfo.h"

//----------------------------------------------------------------------
//
// constructor/destructor
//
//----------------------------------------------------------------------
CStorageSurface::CStorageSurface()
{
	m_Size				= 0;
	m_pPoint			= NULL;
	m_pStorageSurface	= NULL;	
}

CStorageSurface::~CStorageSurface()
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

//----------------------------------------------------------------------
void
CStorageSurface::Init(int size, int width, int height)
{
	
	if (size==0 || width==0 || height==0)
		return;

	
	Release();	

	
	m_Size = size;
	m_pPoint = new POINT [m_Size];
	m_pStorageSurface = new CSpriteSurface [m_Size];

	
	for (int i=0; i<m_Size; i++)	
	{
		
		m_pPoint[i].x = 0;
		m_pPoint[i].y = 0;

		
		m_pStorageSurface[i].InitOffsurface(width, height);
		m_pStorageSurface[i].SetTransparency(0);
	}
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CStorageSurface::Release()
{
	if (m_pStorageSurface!=NULL)
	{
		delete [] m_pStorageSurface;
		m_pStorageSurface = NULL;
	}

	if (m_pPoint!=NULL)
	{
		delete [] m_pPoint;
		m_pPoint = NULL;
	}

	m_Size = 0;
}

//----------------------------------------------------------------------
// Store
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
CStorageSurface::Store(int i, CSpriteSurface* pSurface, POINT* pPoint)
{
	
	if (i<0 || i>=m_Size)
		return;

	POINT	origin = {0, 0};

	int width = pPoint->x + m_pStorageSurface[i].GetWidth();
	int height = pPoint->y + m_pStorageSurface[i].GetHeight();

	// clipping
	if (pPoint->x < 0)
	{
		width += -pPoint->x;
		pPoint->x = 0;		
	}
	else if (width > pSurface->GetWidth())
	{
		pPoint->x -= width - pSurface->GetWidth();
		width = pSurface->GetWidth();
	}	

	// clipping
	if (pPoint->y < 0)
	{
		height += -pPoint->y;
		pPoint->y = 0;
	}
	else if (height > pSurface->GetHeight())
	{
		pPoint->y -= height - pSurface->GetHeight();
		height = pSurface->GetHeight();
	}	

	
	RECT	rect = 
			{
				pPoint->x, 
				pPoint->y,
				width,
				height
			};	
	
	
	
	m_pPoint[i] = *pPoint;

	
	
	m_pStorageSurface[i].BltNoColorkey(&origin, pSurface, &rect);

	//DEBUG_ADD_FORMAT("Store : [%d] (%d, %d)", i, m_pPoint[i].x, m_pPoint[i].y);
}

//----------------------------------------------------------------------
// Restore
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void				
CStorageSurface::Restore(int i, CSpriteSurface* pSurface, POINT* pPoint) const
{
	
	if (i<0 || i>=m_Size)
		return;

	
	RECT	rect = 
			{
				0, 
				0, 
				m_pStorageSurface[i].GetWidth(),
				m_pStorageSurface[i].GetHeight()
			};


	
	
	if (pPoint==NULL)
	{
		POINT point = m_pPoint[i];
		pSurface->BltNoColorkey(&point, &m_pStorageSurface[i], &rect);
	}
	else
	{
		pSurface->BltNoColorkey(pPoint, &m_pStorageSurface[i], &rect);
	}

	//DEBUG_ADD_FORMAT("Restore : [%d] (%d, %d)", i, m_pPoint[i].x, m_pPoint[i].y);
}

