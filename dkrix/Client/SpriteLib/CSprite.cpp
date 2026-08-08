//----------------------------------------------------------------------
// CSprite.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CFilter.h"
#include "CSprite.h"


//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
WORD	CSprite::s_Colorkey = 0;
DWORD	CSprite::s_Value1 = 1;
DWORD	CSprite::s_Value2 = 31;
DWORD	CSprite::s_Value3 = 1;

short		CSprite::s_X = 0;
short		CSprite::s_Y = 0;
CFilter*	CSprite::s_pFilter = NULL;


//----------------------------------------------------------------------
// Init Static member
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//
// constructor/destructore
//
//----------------------------------------------------------------------

CSprite::CSprite()
{
	m_Width		= 0;
	m_Height	= 0;
	m_Pixels	= NULL;
	m_bInit		= false;
	m_bLoading	= false;

#ifdef SPRITELIB_BACKEND_SDL
	m_backend_sprite = SPRITECTL_INVALID_SPRITE;
	m_backend_dirty = false;
#endif
}

CSprite::~CSprite()
{
	Release();
}

//----------------------------------------------------------------------
//
// member functions
//
//----------------------------------------------------------------------

 

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CSprite::Release()
{
#ifdef SPRITELIB_BACKEND_SDL
	if (m_backend_sprite != SPRITECTL_INVALID_SPRITE) {
		spritectl_destroy_sprite(m_backend_sprite);
		m_backend_sprite = SPRITECTL_INVALID_SPRITE;
		m_backend_dirty = false;
	}
#endif
	for (int i=0; i<m_Height; i++)
		delete [] m_Pixels[i];
		//free(m_Pixels[i]);
	delete [] m_Pixels;
	//free(m_Pixels);

	//s_Delete++;

	m_Pixels	= NULL;
	m_Width		= 0;
	m_Height	= 0;

	m_bInit		= false;
}

//----------------------------------------------------------------------
// Assign Operator
//----------------------------------------------------------------------
void
CSprite::operator = (const CSprite& Sprite)
{
		
	Release();


	
	if (Sprite.m_Pixels==NULL || Sprite.m_Width==0 || Sprite.m_Height==0)
		return;

	
	m_Width = Sprite.m_Width;
	m_Height = Sprite.m_Height;
	
	
	int index;	
	register int i;
	register int j;

	
	m_Pixels = new WORD* [m_Height];

	for (int i=0; i<m_Height; i++)
	{
		
		int	count = Sprite.m_Pixels[i][0], 
				colorCount;
		index	= 1;

		
		for (j=0; j<count; j++)
		{
			//transCount = m_Pixels[i][index];
			colorCount = Sprite.m_Pixels[i][index+1];

			index+=2;	

			index += colorCount;	
		}

		
		m_Pixels[i] = new WORD [index];
		memcpy(m_Pixels[i], Sprite.m_Pixels[i], index<<1);		
	}

	
	m_bInit = true;
}

//----------------------------------------------------------------------
// Load From Buffer
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//

//

//



//
//----------------------------------------------------------------------





//----------------------------------------------------------------------
void
CSprite::SetPixel(WORD *pSource, WORD pitch, WORD width, WORD height)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	WORD*	data = new WORD[m_Width*2+10];

	int		index,				
			lastColorIndex;		
	int		count;				
	int		trans,				
			color;				

	BOOL	bCheckTrans;		

	WORD	*pSourceTemp;


	
	m_Pixels = new WORD* [height];

	register int i;
	register int j;

	for (int i=0; i<height; i++)
	{
		index = 0;
		count = 0;
		trans = 0;
		color = 0;
		bCheckTrans = TRUE;

		pSourceTemp = pSource;

		
		for (j=0; j<width; j++)
		{
			
			if (*pSourceTemp==s_Colorkey)
			{
				
				if (!bCheckTrans)
				{
					
					count++;
					
					data[lastColorIndex] = color;
					color = 0;

					bCheckTrans = TRUE;
				}
				
				trans++;				
			}
			else
			{
				
				if (bCheckTrans)
				{						
					data[index++] = trans;		
					trans = 0;

					lastColorIndex=index++;			

					bCheckTrans = FALSE;
				}

				data[index++] = *pSourceTemp;	

				color++;								
			}

			pSourceTemp++;
		}
		
		
		if (bCheckTrans)
		{
			
		}	
		
		else
		{			
			count++;
			data[lastColorIndex] = color;
		}
		
		
		m_Pixels[i] = new WORD [index+1];

		
		
		m_Pixels[i][0] = count;
		memcpy(m_Pixels[i]+1, data, index<<1);

		pSource = (WORD*)((BYTE*)pSource + pitch);
	}

	delete [] data;

	m_bInit = true;
}

//----------------------------------------------------------------------
// Set Pixel No Colorkey
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::SetPixelNoColorkey(WORD *pSource, WORD pitch, WORD width, WORD height)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	WORD*	data = new WORD[m_Width*2+10];

	int	index,				
			lastColorIndex;		
	int	count;				
	int	trans,				
			color;				

	BOOL	bCheckTrans;		

	WORD	*pSourceTemp;


	
	m_Pixels = new WORD* [height];

	register int i;
	register int j;

	for (int i=0; i<height; i++)
	{
		index = 0;
		count = 0;
		trans = 0;
		color = 0;
		bCheckTrans = TRUE;

		pSourceTemp = pSource;

		
		for (j=0; j<width; j++)
		{
			
			 
			{
				
				if (bCheckTrans)
				{						
					data[index++] = trans;		
					trans = 0;

					lastColorIndex=index++;			

					bCheckTrans = FALSE;
				}

				data[index++] = *pSourceTemp;	

				color++;								
			}

			pSourceTemp++;
		}
		
		
		if (bCheckTrans)
		{
			
		}	
		
		else
		{			
			count++;
			data[lastColorIndex] = color;
		}
		
		
		m_Pixels[i] = new WORD [index+1];

		
		
		m_Pixels[i][0] = count;
		memcpy(m_Pixels[i]+1, data, index<<1);

		pSource = (WORD*)((BYTE*)pSource + pitch);
	}

	delete [] data;

	m_bInit = true;
}

/*
void		
CSprite::Uncompress()
{
	m_bCompress = FALSE;
}
*/

//----------------------------------------------------------------------
// Get ColorRect
//----------------------------------------------------------------------

//



//----------------------------------------------------------------------
//
//	[ Example ]
//

//	width = 13, height = 9
//
//
//		0123456789012
//  0	xxxxxxxxxxxxx
//	1	xxxOOOOxxxxxx
//  2	xxOOOxxOOxOxx
//  3	xOOxOOxOOOxxx
//  4	xxOOOOOOOxxxx
//  5	xxxOOxxxOOxxx
//  6	xxxxxxxOOxxxx
//  7	xxxxxxxxxxxxx
//  8	xxxxxxxxxxxxx
//
//

//
//		0123456789012
//  0	xxxxxxxxxxxxx
//	1	x+-OOOO---+xx
//  2	x|OOOxxOOxOxx
//  3	xOOxOOxOOO|xx
//  4	x|OOOOOOOx|xx
//  5	x|xOOxxxOO|xx
//  6	x+-----OO-+xx
//  7	xxxxxxxxxxxxx
//  8	xxxxxxxxxxxxx
//
//      rect.left	= 1
//      rect.top	= 1
//      rect.right	= 10 + 1 = 11
//      rect.bottom	= 6 + 1 = 7
//
//

//

//	newWidth	= rect.right - rect.left
//	newHeight	= rect.bottom - rect.top
//
//----------------------------------------------------------------------
void
CSprite::GetTightColorRect(WORD *pSource, WORD pitch, WORD width, WORD height, WORD colorkey, RECT& rect)
{
	WORD	*pSourceTemp, *pSourceTemp2;
	register int i, j;

	rect.left = 0;
	rect.right = 0;
	rect.top = 0;
	rect.bottom = 0;

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	pSourceTemp = pSource;
	for (j=0; j<width; j++)
	{
		pSourceTemp2 = pSourceTemp;

		for (int i=0; i<height; i++)
		{
			if (*pSourceTemp2!=colorkey)
			{
				rect.left = j;

				j = width;
				break;
			}

			pSourceTemp2 = (WORD*)((BYTE*)pSourceTemp2 + pitch);
		}

		pSourceTemp++;
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	pSourceTemp = pSource;
	for (int i=0; i<height; i++)
	{
		pSourceTemp2 = pSourceTemp;

		for (j=0; j<width; j++)
		{
			if (*pSourceTemp2!=colorkey)
			{
				rect.top = i;

				i = height;
				break;
			}

			pSourceTemp2++;
		}

		pSourceTemp = (WORD*)((BYTE*)pSourceTemp + pitch);
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	pSourceTemp = (WORD*)((BYTE*)pSource + ((width-1)<<1));
	for (j=width-1; j>=0; j--)
	{
		pSourceTemp2 = pSourceTemp;

		for (int i=0; i<height; i++)
		{
			if (*pSourceTemp2!=colorkey)
			{
				rect.right = j + 1;	

				j = 0;
				break;
			}

			pSourceTemp2 = (WORD*)((BYTE*)pSourceTemp2 + pitch);
		}

		pSourceTemp--;
	}

	//-------------------------------------------------------
	
	//-------------------------------------------------------
	pSourceTemp = (WORD*)((BYTE*)pSource + (height-1)*pitch);
	for (int i=height-1; i>=0; i--)
	{
		pSourceTemp2 = pSourceTemp;

		for (j=0; j<width; j++)
		{
			if (*pSourceTemp2!=colorkey)
			{
				rect.bottom = i + 1;	

				i = 0;
				break;
			}

			pSourceTemp2++;
		}
		pSourceTemp = (WORD*)((BYTE*)pSourceTemp - pitch);
	}
}

//----------------------------------------------------------------------
// Is ColorPixel ?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CSprite::IsColorPixel(short x, short y)
{
	
	if (m_bInit)
	{

		
		if (x<0 || y<0 || x>=m_Width || y>=m_Height)
			return false;

		
		WORD	*pPixels = m_Pixels[y];

		
		int	count = *pPixels++;

		int	transCount, 
				colorCount,
				index = 0;

		if (count > 0)
		{
			register int i = count;
			do {		
				transCount = *pPixels++;
				colorCount = *pPixels++;

				index += transCount;

				
				if (x < index+colorCount)
				{
					
					if (x < index)
					{
						return false;
					}

					
					return true;
				}

				pPixels += colorCount;
				index += colorCount;
			} while (--i);
		}
	}

	return false;
}

//----------------------------------------------------------------------
// Get Pixel ?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
WORD		
CSprite::GetPixel(int x, int y) const
{
	
	if (m_bInit)
	{
		
		if (x<0 || y<0 || x>=m_Width || y>=m_Height)
			return 0;

		
		WORD	*pPixels = m_Pixels[y];

		
		int	count = *pPixels++;

		int	transCount, 
				colorCount,
				index = 0;

		if (count > 0)
		{
			register int i = count;
			do {		
				transCount = *pPixels++;
				colorCount = *pPixels++;

				index += transCount;

				
				if (x < index+colorCount)
				{
					
					if (x < index)
					{
						return 0;
					}

					
					
					return pPixels[x-index];					
				}

				pPixels += colorCount;
				index += colorCount;
			} while (--i);
		}
	}

	return 0;
}

//----------------------------------------------------------------------
// Is Intersect Filter
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool
CSprite::IsIntersectFilter()
{
	
	if (IsNotInit() || s_pFilter->IsNotInit())
		return false;

	if (
		s_X < m_Width && s_Y < m_Height
		
		 &&	s_X+s_pFilter->GetWidth() > 0 && s_Y+s_pFilter->GetHeight() > 0)
		{
			return true;
		}
	
	return false;
}

//----------------------------------------------------------------------
// BltClip
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltClip(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------	
	
	//--------------------------------------------
	pDest += pitch * pRect->top + pRect->left;
	//WORD width = ((pRect->right - pRect->left)<<1);

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			colorCount,
			index,
			dist;

	BOOL	bPut;		

	register int i;
	register int j;
	int rectBottom = pRect->bottom;	

	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		bPut = (pRect->left==0)? TRUE:FALSE;
		index = 0;
			
		
		
		
		if (count > 0)
		{
			j = count;
			do 
			{				
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				//lpSurfaceTemp += transCount;
				index += transCount;

				
				if (bPut)
				{
					
					

					
					if (index > pRect->right)
						break;

					pDestTemp += transCount;

					
					if (index+colorCount > pRect->right)
					{							
						
						memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
						break;
					}						

					
					memcpy(pDestTemp, pPixels, colorCount<<1);
					pDestTemp += colorCount;
				}				
				
				else
				{
					
					if (index > pRect->left)
					{	
						pDestTemp += index - pRect->left;
						memcpy(pDestTemp, pPixels, colorCount<<1);
						pDestTemp += colorCount;
						bPut = TRUE;
					}
					else
					if (index+colorCount > pRect->left)
					{
						dist = pRect->left - index;

						
						memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);
						pDestTemp += colorCount-dist;

						
						bPut = TRUE;
					}
				}				

				
				pPixels += colorCount;		

				index += colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::Blt(WORD *pDest, WORD pitch)
{
	int	count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		
 			
			if (count > 0)
			{	
				j = count;
				do {
					pDestTemp += *pPixels++;			
					colorCount = *pPixels++;		

					
					memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);

					//int colorCount2 = colorCount;
					/*
					if (colorCount >= 4) 
					{

						_asm
						{					
							mov eax, colorCount2
							shr	eax, 2

							mov edi, pDestTemp
							mov esi, pPixels
							mov	ecx, eax

						_loop_first_:
							movq		mm0, [esi]
							movq		[edi], mm0
							add			esi, 8
							add			edi, 8
							loop		_loop_first_

							emms // Empty MMX state
						}				
					}
					*/
			
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// Blt ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						memcpy(pDestTemp, pPixels, colorCount<<1);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			//---------------------------------------------	
			if (--j > 0)
			{			
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					memcpy(pDestTemp, pPixels, colorCount<<1);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				
				} while (--j);
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltClipRight(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)		
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						memcpy(pDestTemp, pPixels, (rectRight - index)<<1);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpy(pDestTemp, pPixels, colorCount<<1);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{		
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								//memset((void*)pDestTemp, 0, (rectRight - index)<<1);
								memcpy(pDestTemp, pPixels, (rectRight - index)<<1);
							}

							j = 0;
							
							break;
						}

						
						memcpy(pDestTemp, pPixels, colorCount<<1);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							memcpy(pDestTemp, pPixels+dist, (rectRight - rectLeft)<<1);

							j = 0;						
							break;
						}		

						
						memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						pDestTemp += colorCount-dist;
						pPixels += colorCount;		
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							memcpy(pDestTemp, pPixels, (rectRight - index)<<1);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpy(pDestTemp, pPixels, colorCount<<1);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}



//----------------------------------------------------------------------
// memcpy Darker
//----------------------------------------------------------------------
// Darker : Dest = Source - Filter
//----------------------------------------------------------------------
void	
CSprite::memcpyDarkerFilter(WORD* pDest, WORD* pSource, BYTE* pFilter, WORD pixels)
{
	register int i = pixels;
	
	///*
	BYTE	darker;			
			
	while (i--)
	{
		darker = *pFilter;		

		//if (darker==0)
			//continue;
		
		*pDest = 
			(((((*pSource >> ColorDraw::s_bSHIFT_R) & 0x1F) * darker) >> 5) << ColorDraw::s_bSHIFT_R)
			| (((((*pSource >> ColorDraw::s_bSHIFT_G) & 0x1F) * darker) >> 5) << ColorDraw::s_bSHIFT_G)
			| (((*pSource & 0x1F) * darker) >> 5);
			
		pDest++;
		pSource++;
		pFilter++;		
	}
}



//----------------------------------------------------------------------
// Alpha Copy
//----------------------------------------------------------------------

//
// s_Value1 = 1~32
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 

//----------------------------------------------------------------------
// BltHalf
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltHalf(WORD *pDest, WORD pitch)
{
	int		count,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i=0;
	register int j=0;
	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{		
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltHalf ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltHalfClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{		
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyHalf(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		
			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltHalf ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltHalfClipRight(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						CSpriteSurface::memcpyHalf(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltHalf ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltHalfClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{		
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								CSpriteSurface::memcpyHalf(pDestTemp, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyHalf(pDestTemp, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}					

						
						CSpriteSurface::memcpyHalf(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyHalf(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltHalf Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltHalfClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyHalf(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}



//----------------------------------------------------------------------
// BltAlpha
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltAlpha(WORD *pDest, WORD pitch, BYTE alpha)
{
	
	CSpriteSurface::s_Value1 = alpha;
	CSpriteSurface::s_Value2 = 32 - alpha;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltAlpha ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltAlphaClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	
	CSpriteSurface::s_Value1 = alpha;
	CSpriteSurface::s_Value2 = 32 - alpha;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);		

			//---------------------------------------------
			
			//---------------------------------------------	
			if (--j > 0)			
			{			
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltAlpha ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltAlphaClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	
	CSpriteSurface::s_Value1 = alpha;
	CSpriteSurface::s_Value2 = 32 - alpha;
		
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{		
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltAlpha ClipWidth
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltAlphaClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	
	CSpriteSurface::s_Value1 = alpha;
	CSpriteSurface::s_Value2 = 32 - alpha;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								//memset((void*)pDestTemp, 0, (rectRight - index)<<1);
								CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyAlpha(pDestTemp, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltAlpha Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	
	CSpriteSurface::s_Value1 = alpha;
	CSpriteSurface::s_Value2 = 32 - alpha;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}


//----------------------------------------------------------------------
// BltColor
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltColor(WORD *pDest, WORD pitch, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{						
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltColor ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltColorClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColor ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltColorClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColor ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltColorClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								CSpriteSurface::memcpyColor(pDestTemp, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyColor(pDestTemp, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyColor(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColor Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltColorClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltScale
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltScale(WORD *pDest, WORD pitch, BYTE scale)
{
	
	CSpriteSurface::s_Value1 = scale;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;
	for (int i=0; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{
				pDestTemp += (*pPixels * scale);		
				pPixels++;
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);
				
				pDestTemp	+= colorCount*scale;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch*scale);
	}
}

//----------------------------------------------------------------------
// BltScale ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltScaleClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE scale)
{
	
	CSpriteSurface::s_Value1 = scale;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += (index - rectLeft)*scale;

						
						CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);
						pDestTemp += (colorCount*scale);
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels+dist, colorCount-dist);
						pDestTemp += (colorCount-dist)*scale;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount*scale;			
					
					
					CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);

					
					pDestTemp += colorCount*scale;
					pPixels += colorCount;			
				} while (--j);
			}
		}
			

		pDest = (WORD*)((BYTE*)pDest + pitch*scale);
	}
}

//----------------------------------------------------------------------
// BltScale ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltScaleClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE scale)
{
	
	CSpriteSurface::s_Value1 = scale;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount*scale;
					
						
						CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount*scale;

				
				CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);
				pDestTemp += colorCount*scale;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch*scale);
	}
}

//----------------------------------------------------------------------
// BltScale ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltScaleClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE scale)
{
	
	CSpriteSurface::s_Value1 = scale;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += (index - rectLeft)*scale;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);
						pDestTemp += (colorCount*scale);
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels+dist, colorCount-dist);
						pDestTemp += (colorCount-dist)*scale;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount*scale;
						
							
							CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount*scale;

					
					CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);
					pDestTemp += colorCount*scale;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch*scale);
	}
}

//----------------------------------------------------------------------
// BltScale Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltScaleClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE scale)
{
	
	CSpriteSurface::s_Value1 = scale;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{
				pDestTemp += *pPixels * scale;		
				pPixels++;
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyScale(pDestTemp, pitch, pPixels, colorCount);
				
				pDestTemp	+= colorCount*scale;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch*scale);
	}
}


//----------------------------------------------------------------------
// BltDarkness
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltDarkness(WORD *pDest, WORD pitch, BYTE DarkBits)
{
	
	CSpriteSurface::s_Value1 = DarkBits;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltDarkness ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltDarknessClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkness ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltDarknessClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkness ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltDarknessClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkness Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	CSpriteSurface::s_Value1 = DarkBits;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{				
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColorSet
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltColorSet(WORD *pDest, WORD pitch, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltColorSet ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltColorSetClipLeft(WORD* pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColorSet ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltColorSetClipRight(WORD* pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColorSet ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltColorSetClipWidth(WORD* pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltColorSet Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltColorSetClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{				
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltEffect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltEffect(WORD *pDest, WORD pitch)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp += *pPixels++;			
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltEffect ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltEffectClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels+dist, colorCount-dist);					
						pDestTemp += colorCount-dist;
						pPixels += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp += transCount;			
					
					
					CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltEffect ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltEffectClipRight(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltEffect ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CSprite::BltEffectClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								CSpriteSurface::memcpyEffect(pDestTemp, pPixels, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels+dist, colorCount-dist);					
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}		
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltEffect Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltEffectClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{				
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
} 


//----------------------------------------------------------------------
// BltAlphaFilter
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 


//----------------------------------------------------------------------
// Blt AlphaFilter
//----------------------------------------------------------------------

//
//----------------------------------------------------------------------
void
CSprite::BltAlphaFilter(WORD *pDest, WORD pitch)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;
	BYTE	*pFilter;
	int		dist;

	register int	yIndex = -s_Y + m_Height - 1;
	register int	xIndex;
	register int i;
	register int j;

	int rectBottom = m_Height;
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			//------------------------------------------
			
			//------------------------------------------
			if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
			{
				
				count	= *pPixels++;		
 				
				if (count > 0)
				{
					j = count;
					do 
					{			
						pDestTemp += *pPixels++;			
						colorCount = *pPixels++;		

						
						memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);

						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
					} while (--j);
				}
			}
			//------------------------------------------
			
			//------------------------------------------
			else
			{
				
				pFilter = s_pFilter->GetFilter( yIndex );
				pFilter += -s_X;
				xIndex = -s_X;		

				
				count	= *pPixels++;		
 				
				if (count > 0)
				{
					j = count;
					do 
					{				
						pDestTemp	+= *pPixels;			
						xIndex		+= *pPixels;
						pFilter		+= *pPixels;
						pPixels ++;
						colorCount = *pPixels++;			

						//------------------------------------------
						
						//------------------------------------------
						if (xIndex >= 0)
						{			
							dist  = s_pFilter->GetWidth() - xIndex;
							if (dist > 0)
							{						
								
								if (dist >= colorCount)
								{
									
									memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
								}
								
								else
								{
									
									memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist);
									memcpy((void*)(pDestTemp+dist), (void*)(pPixels+dist), (colorCount-dist)<<1);
								}
							}
							else
							{
								memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
							}
						}
						
						else
						{					
							dist = s_pFilter->GetWidth() - xIndex ;

							
							if (xIndex+colorCount > 0)
							{							
								
								if (s_pFilter->GetWidth() < xIndex+colorCount)
								{	
									memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
									
									memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
									
									// - -;;
									memcpy((void*)(pDestTemp+dist), (void*)(pPixels+dist), (colorCount-dist)<<1);
								}
								
								else
								{							
									memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
									
									memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
								}						
							}
							else
							{
								
								memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
							}
						}

						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						xIndex		+= colorCount;
						pFilter		+= colorCount;
					} while (--j);
				}
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);

			yIndex--;
		} while (i--);
	}
}

//----------------------------------------------------------------------
// Blt AlphaFilter Clip Left
//----------------------------------------------------------------------
void		
CSprite::BltAlphaFilterClipLeft(WORD *pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;
			

	BYTE	*pFilter;
	int		dist, dist2;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;	
		
		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;

							
							memcpy(pDestTemp, pPixels, colorCount<<1);
							pDestTemp += colorCount;
							pPixels += colorCount;

							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{
							dist = rectLeft - index;

							
							memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
							pDestTemp += colorCount-dist;
							pPixels += colorCount;

							
							break;
						}
					}					

					
					pPixels += colorCount;
					index += colorCount;
				} while (--j);
			

				//---------------------------------------------
				
				//---------------------------------------------		
				if (--j > 0)
				{
					do 
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						pDestTemp += transCount;			
						
						
						memcpy(pDestTemp, pPixels, colorCount<<1);

						
						pDestTemp += colorCount;
						pPixels += colorCount;			
					} while (--j);
				}
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;

			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;				
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;
							xIndex += transCount;//index - rectLeft;
							pFilter += transCount;//index - rectLeft;

							
							//memcpy(pDestTemp, pPixels, colorCount<<1);
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount)
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
									}
									
									else
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
									}
								}
								else
								{
									memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
								}
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+colorCount)
									{	
										memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
									}
									
									else
									{							
										memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
									}						
								}
								else
								{
									
									memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
								}
							}
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;
							xIndex		+= colorCount;
							pFilter		+= colorCount;


							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{	
							dist = rectLeft - index;

							xIndex	+= transCount + dist;
							pFilter	+= transCount + dist;

							
							//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
							//pDestTemp += colorCount-dist;
							//pPixels += colorCount;
							
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount-dist)
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels+dist, pFilter, colorCount-dist);
									}
									
									else
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels+dist, pFilter, dist2);
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist+dist2), (colorCount-dist - dist2)<<1);
									}								
								}
								else
								{
									memcpy((void*)pDestTemp, (void*)(pPixels+dist), (colorCount-dist)<<1);
								}							
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount-dist > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+(colorCount-dist))
									{	
										memcpy((void*)pDestTemp, (void*)(pPixels+dist), (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist+dist2), (colorCount-dist - dist2)<<1);
									}
									
									else
									{							
										memcpy((void*)pDestTemp, (void*)(pPixels+dist), (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, (colorCount-dist)+xIndex);
									}						
								}
								else
								{
									
									memcpy((void*)pDestTemp, (void*)(pPixels+dist), (colorCount-dist)<<1);
								}
							}

							pDestTemp += colorCount-dist;
							pPixels += colorCount;
							xIndex	+= colorCount-dist;
							pFilter += colorCount-dist;
							
							
							break;
						}
						
							
					}	
					
					
					xIndex += transCount;
					pFilter += transCount;

					
					pPixels += colorCount;
					index	+= colorCount;
					xIndex	+= colorCount;
					pFilter += colorCount;
				} while (--j);
			
			
				//---------------------------------------------
				
				//---------------------------------------------		
				if (--j > 0)
				{
					do
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						pDestTemp	+= transCount;			
						xIndex		+= transCount;
						pFilter		+= transCount;

						//------------------------------------------
						
						//------------------------------------------
						if (xIndex >= 0)
						{			
							dist2  = s_pFilter->GetWidth() - xIndex;
							if (dist2 > 0)
							{						
								
								if (dist2 >= colorCount)
								{
									
									memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
								}
								
								else
								{
									
									memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
									memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
								}
							}
							else
							{
								memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
							}
						}
						
						else
						{					
							dist2 = s_pFilter->GetWidth() - xIndex ;

							
							if (xIndex+colorCount > 0)
							{							
								
								if (s_pFilter->GetWidth() < xIndex+colorCount)
								{	
									memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
									
									memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
									
									// - -;;
									memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
								}
								
								else
								{							
									memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
									
									memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
								}						
							}
							else
							{
								
								memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
							}
						}

						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;		
						xIndex		+= colorCount;
						pFilter		+= colorCount;
					} while (--j);
				}	
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex ++;
	}
}

//----------------------------------------------------------------------
// Blt AlphaFilter Clip Right
//----------------------------------------------------------------------
void		
CSprite::BltAlphaFilterClipRight(WORD *pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	BYTE	*pFilter;
	int		dist, dist2;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;	
		
		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count = *pPixels++;		

			
			index = 0;
				
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							memcpy((void*)pDestTemp, (void*)pPixels, (rectRight-index)<<1);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;		

			
			count = *pPixels++;		

			
			index = 0;
				
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp	+= transCount;
							xIndex		+= transCount;
							pFilter		+= transCount;

							dist		= rectRight - index;
						
							
							//memcpy((void*)pDestTemp, (void*)pPixels, (rectRight - index)<<1);
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{	
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= dist)
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist);
									}
									
									else
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (dist - dist2)<<1);
									}
								}
								else
								{
									memcpy((void*)pDestTemp, (void*)pPixels, dist<<1);
								}
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex + dist > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+colorCount)
									{	
										memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										int di = dist - dist2;
										if (di>0)
										{
											memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), di<<1);
										}
									}
									
									else
									{							
										memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, dist + xIndex);
									}						
								}
								else
								{
									
									memcpy((void*)pDestTemp, (void*)pPixels, dist<<1);
								}
							}
							break;
						}
					}

					
					pDestTemp	+= transCount;
					xIndex		+= transCount;
					pFilter		+= transCount;

					
					//------------------------------------------
					
					//------------------------------------------
					if (xIndex >= 0)
					{			
						dist2  = s_pFilter->GetWidth() - xIndex;
						if (dist2 > 0)
						{						
							
							if (dist2 >= colorCount)
							{
								
								memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
							}
							
							else
							{
								
								memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
								memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
							}
						}
						else
						{
							memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						}
					}
					
					else
					{					
						dist2 = s_pFilter->GetWidth() - xIndex ;

						
						if (xIndex+colorCount > 0)
						{							
							
							if (s_pFilter->GetWidth() < xIndex+colorCount)
							{	
								memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
								
								memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
								
								// - -;;
								memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
							}
							
							else
							{							
								memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
								
								memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
							}						
						}
						else
						{
							
							memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						}
					}

					pDestTemp	+= colorCount;
					pPixels		+= colorCount;	
					xIndex		+= colorCount;
					pFilter		+= colorCount;
					index		+= colorCount;
				} while (--j);
			}
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex ++;
	}
}


//----------------------------------------------------------------------
// Blt AlphaFilter Clip Width
//----------------------------------------------------------------------
//
// [BUG]
//


//
//----------------------------------------------------------------------

void		
CSprite::BltAlphaFilterClipWidth(WORD *pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;
			

	BYTE	*pFilter;
	int		dist, dist2;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;	
		
		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;

							
							
							if (index+colorCount > rectRight)
							{							
								
								if (index > rectRight)
								{
								}
								else
								{
									//memset((void*)pDestTemp, 0, (rectRight - index)<<1);
									memcpy(pDestTemp, pPixels, (rectRight - index)<<1);
								}

								j = 0;
								
								break;
							}

							
							memcpy(pDestTemp, pPixels, colorCount<<1);
							pDestTemp += colorCount;
							pPixels += colorCount;
							index += colorCount;

							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{
							dist = rectLeft - index;

							
							if (index+colorCount > rectRight)
							{
								memcpy(pDestTemp, pPixels+dist, (rectRight - rectLeft)<<1);

								j = 0;						
								break;
							}		

							
							memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
							pDestTemp += colorCount-dist;
							pPixels += colorCount;
							index += colorCount;

							
							break;
						}
					}					

					
					pPixels += colorCount;
					index += colorCount;
				} while (--j);
			
				//---------------------------------------------
				
				
				//---------------------------------------------
				
				//---------------------------------------------
				if (--j > 0)
				{
					do 
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						index += transCount;
						
						
						

						

						//---------------------------------------------
						
						//---------------------------------------------			
						if (index+colorCount > rectRight)
						{
							
							if (index > rectRight)
							{
								break;
							}
							
							else
							{
								pDestTemp += transCount;
							
								
								memcpy((void*)pDestTemp, (void*)pPixels, (rectRight-index)<<1);
								break;
							}
						}

						
						pDestTemp += transCount;

						
						memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						pDestTemp += colorCount;
						pPixels += colorCount;			
						index += colorCount;
					} while (--j);
				}
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;

			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;				
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							//---------------------------------------------
							
							//---------------------------------------------
							//if (index+colorCount)


							
							pDestTemp += index - rectLeft;
							xIndex += transCount;//index - rectLeft;
							pFilter += transCount;//index - rectLeft;							

							
							//memcpy(pDestTemp, pPixels, colorCount<<1);
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount)
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
									}
									
									else
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
									}
								}
								else
								{
									memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
								}
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+colorCount)
									{	
										memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
									}
									
									else
									{							
										memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
									}						
								}
								else
								{
									
									memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
								}
							}
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;
							xIndex		+= colorCount;
							pFilter		+= colorCount;
							index		+= colorCount;

							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{	
							dist = rectLeft - index;

							xIndex	+= transCount + dist;
							pFilter	+= transCount + dist;

							
							//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
							//pDestTemp += colorCount-dist;
							//pPixels += colorCount;
							
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount-dist)
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels+dist, pFilter, colorCount-dist);
									}
									
									else
									{
										
										memcpyAlphaFilter(pDestTemp, pPixels+dist, pFilter, dist2);
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist+dist2), (colorCount-dist - dist2)<<1);
									}								
								}
								else
								{
									memcpy((void*)pDestTemp, (void*)(pPixels+dist), (colorCount-dist)<<1);
								}							
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount-dist > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+(colorCount-dist))
									{	
										memcpy((void*)pDestTemp, (void*)(pPixels+dist), (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist+dist2), (colorCount-dist - dist2)<<1);
									}
									
									else
									{							
										memcpy((void*)pDestTemp, (void*)(pPixels+dist), (-xIndex)<<1);
										
										memcpyAlphaFilter(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, (colorCount-dist)+xIndex);
									}						
								}
								else
								{
									
									memcpy((void*)pDestTemp, (void*)(pPixels+dist), (colorCount-dist)<<1);
								}
							}

							pDestTemp += colorCount-dist;
							pPixels		+= colorCount;
							xIndex		+= colorCount-dist;
							pFilter		+= colorCount-dist;
							index		+= colorCount;
							
							
							break;
						}
					}	
					
					
					xIndex += transCount;
					pFilter += transCount;

					
					pPixels += colorCount;
					index	+= colorCount;
					xIndex	+= colorCount;
					pFilter += colorCount;
				} while (--j);
				
			
				//---------------------------------------------
				
				
				//---------------------------------------------
				
				//---------------------------------------------
				if (--j > 0)
				{
					do 
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						index += transCount;
						
						
						

						

						//---------------------------------------------
						
						//---------------------------------------------			
						if (index+colorCount > rectRight)
						{
							
							if (index > rectRight)
							{
								break;
							}
							
							else
							{
								pDestTemp	+= transCount;
								xIndex		+= transCount;
								pFilter		+= transCount;

								dist		= rectRight - index;
							
								
								//memcpy((void*)pDestTemp, (void*)pPixels, (rectRight - index)<<1);
								//------------------------------------------
								
								//------------------------------------------
								if (xIndex >= 0)
								{	
									dist2  = s_pFilter->GetWidth() - xIndex;
									if (dist2 > 0)
									{						
										
										if (dist2 >= dist)
										{
											
											memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist);
										}
										
										else
										{
											
											memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
											memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (dist - dist2)<<1);
										}
									}
									else
									{
										memcpy((void*)pDestTemp, (void*)pPixels, dist<<1);
									}
								}
								
								else
								{					
									dist2 = s_pFilter->GetWidth() - xIndex ;

									
									if (xIndex + dist > 0)
									{							
										
										if (s_pFilter->GetWidth() < xIndex+colorCount)
										{	
											memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
											
											memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
											
											// - -;;
											memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (dist - dist2)<<1);
										}
										
										else
										{							
											memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
											
											memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, dist + xIndex);
										}						
									}
									else
									{
										
										memcpy((void*)pDestTemp, (void*)pPixels, dist<<1);
									}
								}
								break;
							}
						}

						
						pDestTemp	+= transCount;
						xIndex		+= transCount;
						pFilter		+= transCount;

						
						//------------------------------------------
						
						//------------------------------------------
						if (xIndex >= 0)
						{			
							dist2  = s_pFilter->GetWidth() - xIndex;
							if (dist2 > 0)
							{						
								
								if (dist2 >= colorCount)
								{
									
									memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
								}
								
								else
								{
									
									memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist2);
									memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
								}
							}
							else
							{
								memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
							}
						}
						
						else
						{					
							dist2 = s_pFilter->GetWidth() - xIndex ;

							
							if (xIndex+colorCount > 0)
							{							
								
								if (s_pFilter->GetWidth() < xIndex+colorCount)
								{	
									memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
									
									memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
									
									// - -;;
									memcpy((void*)(pDestTemp+dist2), (void*)(pPixels+dist2), (colorCount-dist2)<<1);
								}
								
								else
								{							
									memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
									
									memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
								}						
							}
							else
							{
								
								memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
							}
						}

						pDestTemp	+= colorCount;
						pPixels		+= colorCount;	
						xIndex		+= colorCount;
						pFilter		+= colorCount;
						index		+= colorCount;
					} while (--j);
				}
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex ++;
	}
}

//----------------------------------------------------------------------
// Blt AlphaFilter Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltAlphaFilterClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;
	BYTE	*pFilter;
	int		dist;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{				
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;		

			
			count	= *pPixels++;		
 			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp	+= *pPixels;			
					xIndex		+= *pPixels;
					pFilter		+= *pPixels;
					pPixels ++;
					colorCount = *pPixels++;			

					//------------------------------------------
					
					//------------------------------------------
					if (xIndex >= 0)
					{			
						dist  = s_pFilter->GetWidth() - xIndex;
						if (dist > 0)
						{						
							
							if (dist >= colorCount)
							{
								
								memcpyAlphaFilter(pDestTemp, pPixels, pFilter, colorCount);
							}
							
							else
							{
								
								memcpyAlphaFilter(pDestTemp, pPixels, pFilter, dist);
								memcpy((void*)(pDestTemp+dist), (void*)(pPixels+dist), (colorCount-dist)<<1);
							}
						}
						else
						{
							memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						}
					}
					
					else
					{					
						dist = s_pFilter->GetWidth() - xIndex ;

						
						if (xIndex+colorCount > 0)
						{							
							
							if (s_pFilter->GetWidth() < xIndex+colorCount)
							{	
								memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
								
								memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
								
								// - -;;
								memcpy((void*)(pDestTemp+dist), (void*)(pPixels+dist), (colorCount-dist)<<1);
							}
							
							else
							{							
								memcpy((void*)pDestTemp, (void*)pPixels, (-xIndex)<<1);
								
								memcpyAlphaFilter(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
							}						
						}
						else
						{
							
							memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						}
					}

					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
					xIndex		+= colorCount;
					pFilter		+= colorCount;
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex++;
	}
}


//----------------------------------------------------------------------
// memcpy Filter
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::memcpyAlphaFilter(WORD* pDest, WORD* pSource, BYTE* pFilter, WORD pixels)
{
	//register WORD		sTemp,dTemp;
	//register int		sr,sg,sb,dr,dg,db;
	//static WORD		temp, temp2;

	register int j = pixels;
	
	//BYTE alpha;

	// Alpha Channel Blending
	
	while (j--)
	{			
		//alpha = *pFilter;		

		
		/*
		sTemp = *pSource;
		dTemp = *pDest;

		sr = ColorDraw::Red(sTemp);
		sg = ColorDraw::Green(sTemp);
		sb = ColorDraw::Blue(sTemp);

		dr = ColorDraw::Red(dTemp);
		dg = ColorDraw::Green(dTemp);
		db = ColorDraw::Blue(dTemp);		
	
		*pDest = ((alpha * (sb - db) >> 5) + db |
					((alpha * (sg - dg) >> 5) + dg) << ColorDraw::s_bSHIFT_G |
					((alpha * (sr - dr) >> 5) + dr) << ColorDraw::s_bSHIFT_R);
		*/

		///*
		//if (*pFilter > 26) 
		if (*pFilter) 
		{
			*pDest = *pSource; 
		}
		/*
		/*
		else
		{
			*pDest = 
				((*pDest >> 1) & ColorDraw::s_wMASK_SHIFT[1]) +
				((*pSource >> 1) & ColorDraw::s_wMASK_SHIFT[1]);
		}
		//*/
		

		pDest++;
		pSource++;
		pFilter++;
	}
}


//----------------------------------------------------------------------
// Blt AlphaFilter Darkness
//----------------------------------------------------------------------

//
//----------------------------------------------------------------------
void
CSprite::BltAlphaFilterDarkness(WORD *pDest, WORD pitch, BYTE DarkBits)
{
	
	s_Value1 = DarkBits;
	CSpriteSurface::s_Value1 = DarkBits;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;
	BYTE	*pFilter;
	int		dist;

	register int	yIndex = -s_Y + m_Height - 1;
	register int	xIndex;

	register int i;
	register int j;

	int rectBottom = m_Height;
	for (int i=0; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count	= *pPixels++;		
 			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp += *pPixels++;			
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);

					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;		

			
			count	= *pPixels++;		
 			
			if (count > 0)
			{
				j = count;
				do 
				{			
					pDestTemp	+= *pPixels;			
					xIndex		+= *pPixels;
					pFilter		+= *pPixels;
					pPixels ++;
					colorCount = *pPixels++;			

					//------------------------------------------
					
					//------------------------------------------
					if (xIndex >= 0)
					{			
						dist  = s_pFilter->GetWidth() - xIndex;
						if (dist > 0)
						{						
							
							if (dist >= colorCount)
							{
								
								memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
							}
							
							else
							{
								
								memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist);
								CSpriteSurface::memcpyDarkness(pDestTemp+dist, pPixels+dist, colorCount-dist);
							}
						}
						else
						{
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						}
					}
					
					else
					{					
						dist = s_pFilter->GetWidth() - xIndex ;

						
						if (xIndex+colorCount > 0)
						{							
							
							if (s_pFilter->GetWidth() < xIndex+colorCount)
							{	
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
								
								memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
								
								// - -;;
								CSpriteSurface::memcpyDarkness(pDestTemp+dist, pPixels+dist, colorCount-dist);
							}
							
							else
							{							
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
								
								memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
							}						
						}
						else
						{
							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						}
					}

					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
					xIndex		+= colorCount;
					pFilter		+= colorCount;				
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);

		yIndex--;
	}
}

//----------------------------------------------------------------------
// Blt AlphaFilter Darkness Clip Left
//----------------------------------------------------------------------
void		
CSprite::BltAlphaFilterDarknessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	s_Value1 = DarkBits;
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;
			

	BYTE	*pFilter;
	int		dist, dist2;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;	
		
		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;

							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
							pDestTemp += colorCount;
							pPixels += colorCount;

							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{
							dist = rectLeft - index;

							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);					
							pDestTemp += colorCount-dist;
							pPixels += colorCount;

							
							break;
						}
					}					

					
					pPixels += colorCount;
					index += colorCount;
				} while (--j);

				//---------------------------------------------
				
				//---------------------------------------------		
				if (--j > 0)
				{
					do
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						pDestTemp += transCount;			
						
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);

						
						pDestTemp += colorCount;
						pPixels += colorCount;			
					} while (--j);
				}
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;

			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;				
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;
							xIndex += transCount;//index - rectLeft;
							pFilter += transCount;//index - rectLeft;

							
							//memcpy(pDestTemp, pPixels, colorCount<<1);
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount)
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
									}
									
									else
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
									}
								}
								else
								{
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
								}
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+colorCount)
									{	
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
									}
									
									else
									{							
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
									}						
								}
								else
								{
									
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
								}
							}
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;
							xIndex		+= colorCount;
							pFilter		+= colorCount;


							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{	
							dist = rectLeft - index;

							xIndex	+= transCount + dist;
							pFilter	+= transCount + dist;

							
							//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
							//pDestTemp += colorCount-dist;
							//pPixels += colorCount;
							
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount-dist)
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels+dist, pFilter, colorCount-dist);
									}
									
									else
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels+dist, pFilter, dist2);
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist+dist2, colorCount-dist - dist2);
									}								
								}
								else
								{
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
								}							
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount-dist > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+(colorCount-dist))
									{	
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist+dist2, colorCount-dist - dist2);
									}
									
									else
									{							
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, (colorCount-dist)+xIndex);
									}						
								}
								else
								{
									
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
								}
							}

							pDestTemp += colorCount-dist;
							pPixels += colorCount;
							xIndex	+= colorCount-dist;
							pFilter += colorCount-dist;
							
							
							break;
						}
						
							
					}	
					
					
					xIndex += transCount;
					pFilter += transCount;

					
					pPixels += colorCount;
					index	+= colorCount;
					xIndex	+= colorCount;
					pFilter += colorCount;
				} while (--j);
			
				//---------------------------------------------
				
				//---------------------------------------------		
				if (--j > 0)
				{
					do 
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						pDestTemp	+= transCount;			
						xIndex		+= transCount;
						pFilter		+= transCount;

						//------------------------------------------
						
						//------------------------------------------
						if (xIndex >= 0)
						{			
							dist2  = s_pFilter->GetWidth() - xIndex;
							if (dist2 > 0)
							{						
								
								if (dist2 >= colorCount)
								{
									
									memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
								}
								
								else
								{
									
									memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
									CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
								}
							}
							else
							{
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
							}
						}
						
						else
						{					
							dist2 = s_pFilter->GetWidth() - xIndex ;

							
							if (xIndex+colorCount > 0)
							{							
								
								if (s_pFilter->GetWidth() < xIndex+colorCount)
								{	
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
									
									memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
									
									// - -;;
									CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
								}
								
								else
								{							
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
									
									memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
								}						
							}
							else
							{
								
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
							}
						}

						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;		
						xIndex		+= colorCount;
						pFilter		+= colorCount;
					} while (--j);
				}					
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex ++;
	}
}

//----------------------------------------------------------------------
// Blt AlphaFilter Darkness Clip Right
//----------------------------------------------------------------------
void		
CSprite::BltAlphaFilterDarknessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	s_Value1 = DarkBits;
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	BYTE	*pFilter;
	int		dist, dist2;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;	
		
		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count = *pPixels++;		

			
			index = 0;
				
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight-index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += colorCount;			
					index += colorCount;
				} while (--j);
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;		

			
			count = *pPixels++;		

			
			index = 0;
				
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp	+= transCount;
							xIndex		+= transCount;
							pFilter		+= transCount;

							dist		= rectRight - index;
						
							
							//memcpy((void*)pDestTemp, (void*)pPixels, (rectRight - index)<<1);
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{	
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= dist)
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist);
									}
									
									else
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, dist - dist2);
									}
								}
								else
								{
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, dist);
								}
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex + dist > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+colorCount)
									{	
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, dist - dist2);
									}
									
									else
									{							
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, dist + xIndex);
									}						
								}
								else
								{
									
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, dist);
								}
							}
							break;
						}
					}

					
					pDestTemp	+= transCount;
					xIndex		+= transCount;
					pFilter		+= transCount;

					
					//------------------------------------------
					
					//------------------------------------------
					if (xIndex >= 0)
					{			
						dist2  = s_pFilter->GetWidth() - xIndex;
						if (dist2 > 0)
						{						
							
							if (dist2 >= colorCount)
							{
								
								memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
							}
							
							else
							{
								
								memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
								CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
							}
						}
						else
						{
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						}
					}
					
					else
					{					
						dist2 = s_pFilter->GetWidth() - xIndex ;

						
						if (xIndex+colorCount > 0)
						{							
							
							if (s_pFilter->GetWidth() < xIndex+colorCount)
							{	
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
								
								memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
								
								// - -;;
								CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
							}
							
							else
							{							
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
								
								memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
							}						
						}
						else
						{
							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						}
					}

					pDestTemp	+= colorCount;
					pPixels		+= colorCount;	
					xIndex		+= colorCount;
					pFilter		+= colorCount;
					index		+= colorCount;
				} while (--j);
			}
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex ++;
	}
}


//----------------------------------------------------------------------
// Blt AlphaFilter Darkness Clip Width
//----------------------------------------------------------------------
void		
CSprite::BltAlphaFilterDarknessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	s_Value1 = DarkBits;
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;
			

	BYTE	*pFilter;
	int		dist, dist2;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;	
		
		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{						
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;

							
							
							if (index+colorCount > rectRight)
							{							
								
								if (index > rectRight)
								{
								}
								else
								{
									//memset((void*)pDestTemp, 0, (rectRight - index)<<1);
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, (rectRight - index)<<1);									
								}

								j = 0;
								
								break;
							}

							
							memcpy(pDestTemp, pPixels, colorCount<<1);
							pDestTemp += colorCount;
							pPixels += colorCount;
							index += colorCount;

							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{
							dist = rectLeft - index;

							
							if (index+colorCount > rectRight)
							{
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, rectRight - rectLeft);

								j = 0;						
								break;
							}		

							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
							pDestTemp += colorCount-dist;
							pPixels += colorCount;
							index += colorCount;

							
							break;
						}
					}					

					
					pPixels += colorCount;
					index += colorCount;
				} while (--j);

				//---------------------------------------------
				
				
				//---------------------------------------------
				
				//---------------------------------------------
				if (--j > 0)
				{
					do
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						index += transCount;
						
						
						

						

						//---------------------------------------------
						
						//---------------------------------------------			
						if (index+colorCount > rectRight)
						{
							
							if (index > rectRight)
							{
								break;
							}
							
							else
							{
								pDestTemp += transCount;
							
								
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight-index);
								break;
							}
						}

						
						pDestTemp += transCount;

						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += colorCount;			
						index += colorCount;
					} while (--j);
				}
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;

			
			count = *pPixels++;		

			
			index = 0;
			
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (count > 0)
			{
				j = count;
				do 
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;				
				
					//---------------------------------------------
					
					//---------------------------------------------
					if (index+colorCount > rectLeft)
					{
						//---------------------------------------------
						
						//---------------------------------------------
						if (index > rectLeft)
						{	
							
							pDestTemp += index - rectLeft;
							xIndex += transCount;//index - rectLeft;
							pFilter += transCount;//index - rectLeft;

							
							//memcpy(pDestTemp, pPixels, colorCount<<1);
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount)
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
									}
									
									else
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
									}
								}
								else
								{
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
								}
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+colorCount)
									{	
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
									}
									
									else
									{							
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
									}						
								}
								else
								{
									
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
								}
							}
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;
							xIndex		+= colorCount;
							pFilter		+= colorCount;
							index		+= colorCount;

							
							break;
						}
						//---------------------------------------------
						
						
						//---------------------------------------------
						else
						{	
							dist = rectLeft - index;

							xIndex	+= transCount + dist;
							pFilter	+= transCount + dist;

							
							//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
							//pDestTemp += colorCount-dist;
							//pPixels += colorCount;
							
							//------------------------------------------
							
							//------------------------------------------
							if (xIndex >= 0)
							{			
								dist2  = s_pFilter->GetWidth() - xIndex;
								if (dist2 > 0)
								{						
									
									if (dist2 >= colorCount-dist)
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels+dist, pFilter, colorCount-dist);
									}
									
									else
									{
										
										memcpyAlphaFilterDarkness(pDestTemp, pPixels+dist, pFilter, dist2);
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist+dist2, colorCount-dist - dist2);
									}								
								}
								else
								{
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
								}							
							}
							
							else
							{					
								dist2 = s_pFilter->GetWidth() - xIndex ;

								
								if (xIndex+colorCount-dist > 0)
								{							
									
									if (s_pFilter->GetWidth() < xIndex+(colorCount-dist))
									{	
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
										
										// - -;;
										CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist+dist2, colorCount-dist - dist2);
									}
									
									else
									{							
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, -xIndex);
										
										memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels+dist-xIndex, pFilter-xIndex, (colorCount-dist)+xIndex);
									}						
								}
								else
								{
									
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount-dist);
								}
							}

							pDestTemp += colorCount-dist;
							pPixels		+= colorCount;
							xIndex		+= colorCount-dist;
							pFilter		+= colorCount-dist;
							index		+= colorCount;
							
							
							break;
						}
					}	
					
					
					xIndex += transCount;
					pFilter += transCount;

					
					pPixels += colorCount;
					index	+= colorCount;
					xIndex	+= colorCount;
					pFilter += colorCount;
				} while (--j);
			
				//---------------------------------------------
				
				
				//---------------------------------------------
				
				//---------------------------------------------
				if (--j > 0)
				{
					do
					{
						transCount = *pPixels++;		
						colorCount = *pPixels++;		
								
						
						index += transCount;
						
						
						

						

						//---------------------------------------------
						
						//---------------------------------------------			
						if (index+colorCount > rectRight)
						{
							
							if (index > rectRight)
							{
								break;
							}
							
							else
							{
								pDestTemp	+= transCount;
								xIndex		+= transCount;
								pFilter		+= transCount;

								dist		= rectRight - index;
							
								
								//memcpy((void*)pDestTemp, (void*)pPixels, (rectRight - index)<<1);
								//------------------------------------------
								
								//------------------------------------------
								if (xIndex >= 0)
								{	
									dist2  = s_pFilter->GetWidth() - xIndex;
									if (dist2 > 0)
									{						
										
										if (dist2 >= dist)
										{
											
											memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist);
										}
										
										else
										{
											
											memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
											CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, dist - dist2);
										}
									}
									else
									{
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, dist);
									}
								}
								
								else
								{					
									dist2 = s_pFilter->GetWidth() - xIndex ;

									
									if (xIndex + dist > 0)
									{							
										
										if (s_pFilter->GetWidth() < xIndex+colorCount)
										{	
											CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
											
											memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
											
											// - -;;
											CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, dist - dist2);
										}
										
										else
										{							
											CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
											
											memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, dist + xIndex);
										}						
									}
									else
									{
										
										CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, dist);
									}
								}
								break;
							}
						}

						
						pDestTemp	+= transCount;
						xIndex		+= transCount;
						pFilter		+= transCount;

						
						//------------------------------------------
						
						//------------------------------------------
						if (xIndex >= 0)
						{			
							dist2  = s_pFilter->GetWidth() - xIndex;
							if (dist2 > 0)
							{						
								
								if (dist2 >= colorCount)
								{
									
									memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
								}
								
								else
								{
									
									memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist2);
									CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
								}
							}
							else
							{
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
							}
						}
						
						else
						{					
							dist2 = s_pFilter->GetWidth() - xIndex ;

							
							if (xIndex+colorCount > 0)
							{							
								
								if (s_pFilter->GetWidth() < xIndex+colorCount)
								{	
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
									
									memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
									
									// - -;;
									CSpriteSurface::memcpyDarkness(pDestTemp+dist2, pPixels+dist2, colorCount-dist2);
								}
								
								else
								{							
									CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
									
									memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
								}						
							}
							else
							{
								
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
							}
						}

						pDestTemp	+= colorCount;
						pPixels		+= colorCount;	
						xIndex		+= colorCount;
						pFilter		+= colorCount;
						index		+= colorCount;
					} while (--j);
				}
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex ++;
	}
}

//----------------------------------------------------------------------
// Blt AlphaFilter Darkness Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltAlphaFilterDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	
	s_Value1 = DarkBits;
	CSpriteSurface::s_Value1 = DarkBits;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;
	BYTE	*pFilter;
	int		dist;

	register int	yIndex = -s_Y + pRect->top;
	register int	xIndex;

	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		//------------------------------------------
		
		//------------------------------------------
		if (yIndex < 0 || yIndex >= s_pFilter->GetHeight())
		{
			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{				
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}
		}
		//------------------------------------------
		
		//------------------------------------------
		else
		{
			
			pFilter = s_pFilter->GetFilter( yIndex );
			pFilter += -s_X;
			xIndex = -s_X;		

			
			count	= *pPixels++;		
 			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp	+= *pPixels;			
					xIndex		+= *pPixels;
					pFilter		+= *pPixels;
					pPixels ++;
					colorCount = *pPixels++;			

					//------------------------------------------
					
					//------------------------------------------
					if (xIndex >= 0)
					{			
						dist  = s_pFilter->GetWidth() - xIndex;
						if (dist > 0)
						{						
							
							if (dist >= colorCount)
							{
								
								memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, colorCount);
							}
							
							else
							{
								
								memcpyAlphaFilterDarkness(pDestTemp, pPixels, pFilter, dist);
								CSpriteSurface::memcpyDarkness(pDestTemp+dist, pPixels+dist, colorCount-dist);
							}
						}
						else
						{
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						}
					}
					
					else
					{					
						dist = s_pFilter->GetWidth() - xIndex ;

						
						if (xIndex+colorCount > 0)
						{							
							
							if (s_pFilter->GetWidth() < xIndex+colorCount)
							{	
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
								
								memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, s_pFilter->GetWidth());
								
								// - -;;
								CSpriteSurface::memcpyDarkness(pDestTemp+dist, pPixels+dist, colorCount-dist);
							}
							
							else
							{							
								CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, -xIndex);
								
								memcpyAlphaFilterDarkness(pDestTemp-xIndex, pPixels-xIndex, pFilter-xIndex, colorCount+xIndex);
							}						
						}
						else
						{
							
							CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						}
					}

					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
					xIndex		+= colorCount;
					pFilter		+= colorCount;
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
		yIndex++;
	}
}


//----------------------------------------------------------------------
// memcpy Filter Darkness
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::memcpyAlphaFilterDarkness(WORD* pDest, WORD* pSource, BYTE* pFilter, WORD pixels)
{
	register WORD		sTemp,dTemp;
	register int		sr,sg,sb,dr,dg,db;
	//static WORD		temp, temp2;

	register int j = pixels;

	BYTE alpha;

	// Alpha Channel Blending
	
	while (j--)
	{			
		alpha = *pFilter;		

		
		///*
		sTemp = *pSource;
		dTemp = *pDest;

		sr = ColorDraw::Red(sTemp);
		sg = ColorDraw::Green(sTemp);
		sb = ColorDraw::Blue(sTemp);

		dr = ColorDraw::Red(dTemp);
		dg = ColorDraw::Green(dTemp);
		db = ColorDraw::Blue(dTemp);		
		
		*pDest = ((alpha * (sb - db) >> 5) + db |
					((alpha * (sg - dg) >> 5) + dg) << ColorDraw::s_bSHIFT_G |
					((alpha * (sr - dr) >> 5) + dr) << ColorDraw::s_bSHIFT_R);

		*pDest = (*pDest >> s_Value1) & ColorDraw::s_wMASK_SHIFT[s_Value1];
		//*/

		//if (alpha > 26) 
		{
			//*pDest = *pSource; 
		//	*pDest = (*pSource >> s_Value1) & ColorDraw::s_wMASK_SHIFT[s_Value1];
		}


		pDest++;
		pSource++;
		pFilter++;
	}
}


//----------------------------------------------------------------------
// BltDarkerFilter
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltDarkerFilter(WORD *pDest, WORD pitch)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;
	BYTE	*pFilter;

	register int i;
	register int j;

	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pFilter		= s_pFilter->GetFilter( i );
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{
				j = count;
				do 
				{
					pDestTemp	+= *pPixels;		
					pFilter		+= *pPixels;
					pPixels++;
					colorCount = *pPixels++;		

					
					memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
					pFilter		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltDarkerFilter ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltDarkerFilterClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;
	BYTE	*pFilter;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pFilter		= s_pFilter->GetFilter( i );
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp	+= index - rectLeft;
						pFilter		+= index - rectLeft;

						
						memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						pFilter		+= colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyDarkerFilter(pDestTemp, pPixels+dist, pFilter+dist, colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += colorCount;
						pFilter	+= colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					pDestTemp	+= transCount;
					pFilter		+= transCount;
					
					
					memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);

					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;	
					pFilter		+= colorCount;
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkerFilter ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltDarkerFilterClipRight(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;
	BYTE	*pFilter;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index;

	register int	i;
	register int	j;

	int rectBottom = pRect->bottom;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pFilter		= s_pFilter->GetFilter( i );
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp	+= transCount;
						pFilter		+= transCount;
					
						
						memcpyDarkerFilter(pDestTemp, pPixels, pFilter, rectRight - index);
						break;
					}
				}

				
				pDestTemp	+= transCount;
				pFilter		+= transCount;

				
				memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
				pFilter		+= colorCount;
				index		+= colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkerFilter ClipWidth
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CSprite::BltDarkerFilterClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;
	BYTE	*pFilter;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register int i;
	register int j;

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;
	for (int i=pRect->top; i<rectBottom; i++)
	{
		pPixels = m_Pixels[i];
		pFilter		= s_pFilter->GetFilter( i );
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		if (count > 0)
		{
			j = count;
			do 
			{
				transCount = *pPixels++;		
				colorCount = *pPixels++;		
						
				
				index += transCount;
				
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp	+= index - rectLeft;
						pFilter		+= index - rectLeft;

						
						
						if (index+colorCount > rectRight)
						{							
							
							if (index > rectRight)
							{
							}
							else
							{
								//memset((void*)pDestTemp, 0, (rectRight - index)<<1);
								memcpyDarkerFilter(pDestTemp, pPixels, pFilter, rectRight - index);
							}

							j = 0;
							
							break;
						}

						
						memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						pFilter		+= colorCount;
						index		+= colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						if (index+colorCount > rectRight)
						{
							memcpyDarkerFilter(pDestTemp, pPixels+dist, pFilter+dist, rectRight - rectLeft);

							j = 0;						
							break;
						}		

						
						memcpyDarkerFilter(pDestTemp, pPixels+dist, pFilter+dist, colorCount-dist);
						pDestTemp	+= colorCount-dist;
						pPixels		+= colorCount;
						pFilter		+= colorCount;
						index		+= colorCount;

						
						break;
					}
				}					

				
				pPixels += colorCount;
				pFilter	+= colorCount;
				index += colorCount;
			} while (--j);

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do
				{
					transCount = *pPixels++;		
					colorCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp	+= transCount;
							pFilter		+= transCount;
						
							
							memcpyDarkerFilter(pDestTemp, pPixels, pFilter, rectRight - index);
							break;
						}
					}

					
					pDestTemp	+= transCount;
					pFilter		+= transCount;

					
					memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;			
					pFilter		+= colorCount;
					index		+= colorCount;				
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkerFilter Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltDarkerFilterClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;
	BYTE	*pFilter;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pFilter		= s_pFilter->GetFilter( i );
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do 
			{				
				pDestTemp	+= *pPixels;		
				pFilter		+= *pPixels;
				pPixels++;
				colorCount = *pPixels++;		

				
				memcpyDarkerFilter(pDestTemp, pPixels, pFilter, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
				pFilter		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltAlpha4444NotTrans
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::BltAlpha4444NotTrans(WORD *pDest, WORD pitch, BYTE alpha)
{
	s_Value1 = alpha >> 1;	

	int		count,	
			transCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{			
				j = count;
				do
				{		
					transCount = *pPixels++;					
					colorCount = *pPixels++;		

					
					memset(pDestTemp, 0, transCount<<1);
					pDestTemp += transCount;		

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;

					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}


//----------------------------------------------------------------------
// AlphaChannel Copy  4444
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void	
CSprite::memcpyAlpha4444(WORD* pDest, WORD* pSource, WORD pixels)
{
	register WORD		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels;

	// Alpha Channel Blending
	
	while (i--)
	{	
		
		sTemp = *pSource;
	
		sr = (sTemp >> ColorDraw::s_bSHIFT4_R);// & 0x0F;
		sg = (sTemp >> ColorDraw::s_bSHIFT4_G) & 0x0F;
		sb = (sTemp >> ColorDraw::s_bSHIFT4_B) & 0x0F;
		
		*pDest =	(s_Value1 << 12) |
					(sr << 8) |
					(sg << 4) |
					(sb);
	
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// BltAlpha4444SmallNotTrans
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void
CSprite::BltAlpha4444SmallNotTrans(WORD *pDest, WORD pitch, BYTE alpha, BYTE shift)
{
	s_Value1 = alpha >> 1;	
	s_Value2 = shift;
	
	s_Value3 = 1 << shift;


	int		count,	
			transCount,
			colorCount,
			transCountShift,
			colorCountShift;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		int stepY = 1 << shift;		
		pDest = (WORD*)((BYTE*)pDest + (i>>shift)*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{			
				j = count;
				do
				{		
					transCount = *pPixels++;					
					colorCount = *pPixels++;		

					
					transCountShift = transCount >> shift;
					colorCountShift = colorCount >> shift;

					
					memset(pDestTemp, 0, transCountShift<<1);
					pDestTemp += transCountShift;		

					
					memcpyAlpha4444Small(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCountShift;

					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);

			i-=stepY;

		} while (i>0);
	}
}

//----------------------------------------------------------------------
// Alpha Copy  4444 Small
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//



//----------------------------------------------------------------------
void	
CSprite::memcpyAlpha4444Small(WORD* pDest, WORD* pSource, WORD pixels)
{
	register WORD		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels >> s_Value2;

	
	
	while (i--)
	{	
		
		sTemp = *pSource;
	
		sr = (sTemp >> ColorDraw::s_bSHIFT4_R);// & 0x0F;
		sg = (sTemp >> ColorDraw::s_bSHIFT4_G) & 0x0F;
		sb = (sTemp >> ColorDraw::s_bSHIFT4_B) & 0x0F;
		
		*pDest =	(s_Value1 << 12) |
					(sr << 8) |
					(sg << 4) |
					(sb);
	
		pDest++;
		pSource += s_Value3;
	}
}


//----------------------------------------------------------------------
// Blt1555NotTrans
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSprite::Blt1555NotTrans(WORD *pDest, WORD pitch)
{
	

	int		count,	
			transCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{			
				j = count;
				do
				{		
					transCount = *pPixels++;					
					colorCount = *pPixels++;		

					
					memset(pDestTemp, 0, transCount<<1);
					pDestTemp += transCount;		

					
					memcpy1555(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;

					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}


//----------------------------------------------------------------------
// AlphaChannel Copy  1555
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void	
CSprite::memcpy1555(WORD* pDest, WORD* pSource, WORD pixels)
{
	register int		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels;

	// Alpha Channel Blending
	
	while (i--)
	{	
		
		sTemp = *pSource;
	
		sr = (sTemp >> ColorDraw::s_bSHIFT_R);// & 0x0F;
		sg = (sTemp >> ColorDraw::s_bSHIFT_G) & 0x0000001F;
		sb = (sTemp >> ColorDraw::s_bSHIFT_B) & 0x0000001F;
		
		*pDest =	0x8000 |
					(sr << 10) |
					(sg << 5) |
					(sb);
	
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// Blt1555SmallNotTrans
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void
CSprite::Blt1555SmallNotTrans(WORD *pDest, WORD pitch, BYTE shift)
{
	
	s_Value2 = shift;
	
	s_Value3 = 1 << shift;


	int		count,	
			transCount,
			colorCount,
			transCountShift,
			colorCountShift;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		int stepY = 1 << shift;		
		pDest = (WORD*)((BYTE*)pDest + (i>>shift)*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{			
				j = count;
				do
				{		
					transCount = *pPixels++;					
					colorCount = *pPixels++;		

					
					transCountShift = transCount >> shift;
					colorCountShift = colorCount >> shift;

					
					memset(pDestTemp, 0, transCountShift<<1);
					pDestTemp += transCountShift;		

					
					memcpy1555Small(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCountShift;

					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);

			i-=stepY;

		} while (i>0);
	}
}

//----------------------------------------------------------------------
// Alpha Copy  1555 Small
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//


//----------------------------------------------------------------------
void	
CSprite::memcpy1555Small(WORD* pDest, WORD* pSource, WORD pixels)
{
	register int		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels >> s_Value2;

	
	while (i--)
	{	
		
		sTemp = *pSource;
	
		sr = (sTemp >> ColorDraw::s_bSHIFT_R);// & 0x0F;
		sg = (sTemp >> ColorDraw::s_bSHIFT_G) & 0x0000001F;
		sb = (sTemp >> ColorDraw::s_bSHIFT_B) & 0x0000001F;
		
		*pDest =	0x8000 | //(s_Value1 << 15) |
					(sr << 10) |
					(sg << 5) |
					(sb);
	
		pDest++;
		pSource += s_Value3;
	}
}

DWORD
CSprite::GetFileSize()
{
	DWORD fileSize = 0;

	
	fileSize += 2;
	fileSize += 2;
	
	
	if (m_Pixels==NULL || m_Width==0 || m_Height==0)
		return 0;
	
	
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
		
		
		fileSize += 2;
		fileSize += index<<1;
	}

	return fileSize;
}