//----------------------------------------------------------------------
// CAlphaSprite.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CSpriteSurface.h"
#include "CAlphaSprite.h"

#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
#endif


//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
WORD	CAlphaSprite::s_Colorkey = 0;
int		CAlphaSprite::s_Value1	= 0;
int		CAlphaSprite::s_Value2	= 0;
//BYTE	CAlphaSprite::s_Value2 = 31;



//----------------------------------------------------------------------
//
// constructor/destructore
//
//----------------------------------------------------------------------

CAlphaSprite::CAlphaSprite()
{
	m_Width		= 0;
	m_Height	= 0;
	m_Pixels	= NULL;
	m_bInit		= false;

#ifdef SPRITELIB_BACKEND_SDL
	m_backend_sprite = SPRITECTL_INVALID_SPRITE;
	m_backend_dirty = false;
#endif
}

CAlphaSprite::~CAlphaSprite()
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
CAlphaSprite::Release()
{
#ifdef SPRITELIB_BACKEND_SDL
	if (m_backend_sprite != SPRITECTL_INVALID_SPRITE) {
		spritectl_destroy_sprite(m_backend_sprite);
		m_backend_sprite = SPRITECTL_INVALID_SPRITE;
		m_backend_dirty = false;
	}
#endif
	if (m_Pixels!=NULL)
	{
		m_bInit		= false;

		for (int i=0; i<m_Height; i++)
			delete [] m_Pixels[i];
			//free(m_Pixels[i]);
		delete [] m_Pixels;
		//free(m_Pixels);

		//s_Delete++;

		m_Pixels	= NULL;
		m_Width		= 0;
		m_Height	= 0;	
	}
}

//----------------------------------------------------------------------
// Assign Operator
//----------------------------------------------------------------------
void
CAlphaSprite::operator = (const CAlphaSprite& Sprite)
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

			index += (colorCount<<1);	
		}

		
		m_Pixels[i] = new WORD [index];
		memcpy(m_Pixels[i], Sprite.m_Pixels[i], index<<1);
	}

	
	m_bInit = true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//

//

//



//




//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CAlphaSprite::SetPixel(WORD *pSource, WORD sourcePitch, 
						WORD *pFilter, WORD filterPitch, 
						WORD width, WORD height)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	WORD*	data = new WORD[m_Width*4+10];

	int	index,				
			lastColorIndex;		
	int	count;				
	int	trans,				
			color;				

	BOOL	bCheckTrans;		

	WORD	*pSourceTemp, *pFilterTemp;

	
	m_Pixels = new WORD* [height];

	for (register int i=0; i<height; i++)
	{
		index = 0;
		count = 0;
		trans = 0;
		color = 0;
		bCheckTrans = TRUE;

		pSourceTemp = pSource;
		pFilterTemp = pFilter;

		
		for (register int j=0; j<width; j++)
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

				
				BYTE alpha;
				WORD alpha2;				
				alpha = (BYTE)(*pFilterTemp & 0x001F);	
				alpha2 = (alpha << 8) | (32-alpha);	// (Alpha:32-Alpha)

				
				data[index++] = alpha2;					
				data[index++] = *pSourceTemp;			

				color++;								
			}

			pSourceTemp++;
			pFilterTemp++;
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

		pSource = (WORD*)((BYTE*)pSource + sourcePitch);
		pFilter = (WORD*)((BYTE*)pFilter + filterPitch);
	}

	delete [] data;

	m_bInit = true;
}

/*
void		
CAlphaSprite::Uncompress()
{
	m_bCompress = FALSE;
}
*/

//----------------------------------------------------------------------
// Is ColorPixel ?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CAlphaSprite::IsColorPixel(short x, short y)
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
			do
			{
				transCount = *pPixels++;
				colorCount = *pPixels++;

				index += transCount;

				
				if (x < index+colorCount)
				{
					
					if (x < index)
					{
						int n = index - x;

						
						pPixels += n<<1;

						
						if ((*pPixels >> 8)!= 0)
						{
							return true;
						}

						return false;
					}

					
					return true;
				}
				
				pPixels += (colorCount<<1);
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
CAlphaSprite::GetPixel(int x, int y, int bColor) const
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
			do
			{
				transCount = *pPixels++;
				colorCount = *pPixels++;

				index += transCount;

				
				if (x < index+colorCount)
				{
					
					if (x < index)
					{
						return 0;
					}

					
					
					return pPixels[((x-index)<<1)+bColor];
				}
				
				pPixels += (colorCount<<1);
				index += colorCount;
			} while (--i);
		}
	}

	return 0;
}

//----------------------------------------------------------------------
// AlphaChannel Copy
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void	
CAlphaSprite::memcpyAlpha(WORD* pDest, WORD* pSource, WORD pixels)
{
	register WORD		sTemp,dTemp;
	register int		sr,sg,sb,dr,dg,db;
	//static WORD		temp, temp2;

	register int i = pixels;

	BYTE alpha;

	// Alpha Channel Blending
	
	while (i--)
	{	
		
		alpha = *pSource >> 8;
		pSource++;

		
		sTemp = *pSource;
		dTemp = *pDest;

		sr = ColorDraw::Red(sTemp);
		sg = ColorDraw::Green(sTemp);
		sb = ColorDraw::Blue(sTemp);

		dr = ColorDraw::Red(dTemp);
		dg = ColorDraw::Green(dTemp);
		db = ColorDraw::Blue(dTemp);		
		
		*pDest = ((((sb - db)*alpha >> 5) + db) |
					((((sg - dg)*alpha >> 5) + dg) << ColorDraw::s_bSHIFT_G) |
					(((sr - dr)*alpha >> 5) + dr) << ColorDraw::s_bSHIFT_R);
	
		 

		pDest++;
		pSource++;
	}

}


//----------------------------------------------------------------------
// AlphaChannel Copy  4444
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void	
CAlphaSprite::memcpyAlpha4444(WORD* pDest, WORD* pSource, WORD pixels)
{
	register WORD		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels;

	BYTE alpha;

	// Alpha Channel Blending
	
	while (i--)
	{	
		
		alpha = *pSource >> 9;	//	alpha = (*pSource >> 8) >> 1;
		pSource++;

		
		sTemp = *pSource;
	
		sr = (sTemp >> ColorDraw::s_bSHIFT4_R);// & 0x0F;
		sg = (sTemp >> ColorDraw::s_bSHIFT4_G) & 0x0F;
		sb = (sTemp >> ColorDraw::s_bSHIFT4_B) & 0x0F;
		
		*pDest =	(alpha << 12) |
					(sr << 8) |
					(sg << 4) |
					(sb);
	
		pDest++;
		pSource++;
	}
}

//----------------------------------------------------------------------
// BltClip
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CAlphaSprite::BltClip(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------	
	
	//--------------------------------------------
	pDest = (WORD*)((BYTE*)pDest + pitch*pRect->top + pRect->left);
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
						
						memcpyAlpha(pDestTemp, pPixels, pRect->right - index);
						break;
					}						

					
					memcpyAlpha(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
				}				
				
				else
				{
					
					if (index > pRect->left)
					{	
						pDestTemp += index - pRect->left;
						memcpyAlpha(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						bPut = TRUE;
					}
					else
					if (index+colorCount > pRect->left)
					{
						dist = pRect->left - index;

						
						memcpyAlpha(pDestTemp, pPixels+dist, colorCount-dist);
						pDestTemp += colorCount-dist;

						
						bPut = TRUE;
					}
				}				

				
				pPixels += (colorCount<<1);		

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
CAlphaSprite::Blt(WORD *pDest, WORD pitch)
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
				do
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					memcpyAlpha(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;

					
					pPixels		+= (colorCount<<1);
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
CAlphaSprite::BltClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;

	//---------------------------------------------
	
	//---------------------------------------------
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

						
						memcpyAlpha(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
					
					
					memcpyAlpha(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
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
CAlphaSprite::BltClipRight(WORD* pDest, WORD pitch, RECT* pRect)
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
					
						
						memcpyAlpha(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpyAlpha(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += (colorCount<<1);
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
CAlphaSprite::BltClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;

	//---------------------------------------------
	
	//---------------------------------------------
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
								memcpyAlpha(pDestTemp, pPixels, (rectRight - index));
							}

							j = 0;
							
							break;
						}
						memcpyAlpha(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);
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
							memcpyAlpha(pDestTemp, pPixels+(dist<<1), (rectRight - rectLeft));

							j = 0;						
							break;
						}		
						memcpyAlpha(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
						
							
							memcpyAlpha(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpyAlpha(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
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
CAlphaSprite::BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int	count,			
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

				
				memcpyAlpha(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= (colorCount<<1);
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444(WORD *pDest, WORD pitch)
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

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;

					
					pPixels		+= (colorCount<<1);
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// Blt4444 ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444ClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;

	//---------------------------------------------
	
	//---------------------------------------------
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

						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
					
					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444ClipRight(WORD* pDest, WORD pitch, RECT* pRect)
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
					
						
						memcpyAlpha4444(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += (colorCount<<1);
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444ClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;

	//---------------------------------------------
	
	//---------------------------------------------
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

						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
						
							
							memcpyAlpha4444(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
					index += colorCount;
				} while (--j);
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444ClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
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

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= (colorCount<<1);
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}


//----------------------------------------------------------------------
// Blt4444NotTrans
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444NotTrans(WORD *pDest, WORD pitch)
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

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;

					
					pPixels		+= (colorCount<<1);
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// Blt4444NotTrans ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444NotTransClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;

	//---------------------------------------------
	
	//---------------------------------------------
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
						
						
						transCount = index - rectLeft;
						memset(pDestTemp, 0, transCount<<1);
						pDestTemp += transCount;		
						
						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
							
					
					memset(pDestTemp, 0, transCount<<1);
					pDestTemp += transCount;		
					
					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444NotTrans ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444NotTransClipRight(WORD* pDest, WORD pitch, RECT* pRect)
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
						
						memset(pDestTemp, 0, transCount<<1);
						pDestTemp += transCount;		
					
						
						memcpyAlpha4444(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				memset(pDestTemp, 0, transCount<<1);
				pDestTemp += transCount;		

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += (colorCount<<1);
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444NotTrans ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444NotTransClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;

	//---------------------------------------------
	
	//---------------------------------------------
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
						transCount = index - rectLeft;
						
						memset(pDestTemp, 0, transCount<<1);
						pDestTemp += transCount;		

						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
							
							memset(pDestTemp, 0, transCount<<1);
							pDestTemp += transCount;		
						
							
							memcpyAlpha4444(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					memset(pDestTemp, 0, transCount<<1);
					pDestTemp += transCount;		

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
					index += colorCount;
				} while (--j);
			}
		}		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444NotTrans Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444NotTransClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,
			transCount,
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
				transCount = *pPixels++;		
				colorCount = *pPixels++;		

				
				memset(pDestTemp, 0, transCount<<1);
				pDestTemp += transCount;		

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= (colorCount<<1);
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
CAlphaSprite::BltAlpha(WORD *pDest, WORD pitch, BYTE alpha)
{
	s_Value1 = alpha;

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
				do
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					memcpyAlphaValue(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;

					
					pPixels		+= (colorCount<<1);
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
CAlphaSprite::BltAlphaClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	s_Value1 = alpha;

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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;

	//---------------------------------------------
	
	//---------------------------------------------
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

						
						memcpyAlphaValue(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlphaValue(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
					
					
					memcpyAlphaValue(pDestTemp, pPixels, colorCount);

					
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
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
CAlphaSprite::BltAlphaClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	s_Value1 = alpha;

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
					
						
						memcpyAlphaValue(pDestTemp, pPixels, rectRight - index);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpyAlphaValue(pDestTemp, pPixels, colorCount);
				pDestTemp += colorCount;
				pPixels += (colorCount<<1);
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
CAlphaSprite::BltAlphaClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	s_Value1 = alpha;

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

	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
	int rectRight = pRect->right;

	//---------------------------------------------
	
	//---------------------------------------------
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
								memcpyAlphaValue(pDestTemp, pPixels, (rectRight - index));
							}

							j = 0;
							
							break;
						}

						memcpyAlphaValue(pDestTemp, pPixels, colorCount);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);
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
							memcpyAlphaValue(pDestTemp, pPixels+(dist<<1), (rectRight - rectLeft));

							j = 0;						
							break;
						}		

						memcpyAlphaValue(pDestTemp, pPixels+(dist<<1), colorCount-dist);
						pDestTemp += colorCount-dist;
						pPixels += (colorCount<<1);
						index += colorCount;

						
						break;
					}
				}					

				
				pPixels += (colorCount<<1);
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
						
							
							memcpyAlphaValue(pDestTemp, pPixels, rectRight - index);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpyAlphaValue(pDestTemp, pPixels, colorCount);
					pDestTemp += colorCount;
					pPixels += (colorCount<<1);
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
CAlphaSprite::BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	s_Value1 = alpha;

	int	count,			
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

				
				memcpyAlphaValue(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= (colorCount<<1);
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}


//----------------------------------------------------------------------
// AlphaChannel Copy
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void	
CAlphaSprite::memcpyAlphaValue(WORD* pDest, WORD* pSource, WORD pixels)
{
	register WORD		sTemp,dTemp;
	register int		sr,sg,sb,dr,dg,db;
	//static WORD		temp, temp2;

	register int i = pixels;

	//BYTE alpha;

	// Alpha Channel Blending
	
	while (i--)
	{	
		
		//alpha = *pSource >> 8;
		pSource++;

		
		sTemp = *pSource;
		dTemp = *pDest;

		sr = ColorDraw::Red(sTemp);
		sg = ColorDraw::Green(sTemp);
		sb = ColorDraw::Blue(sTemp);

		dr = ColorDraw::Red(dTemp);
		dg = ColorDraw::Green(dTemp);
		db = ColorDraw::Blue(dTemp);		
		
		*pDest = ((s_Value1 * (sb - db) >> 5) + db |
					((s_Value1 * (sg - dg) >> 5) + dg) << ColorDraw::s_bSHIFT_G |
					((s_Value1 * (sr - dr) >> 5) + dr) << ColorDraw::s_bSHIFT_R);
	
		 

		pDest++;
		pSource++;
	}
}


//----------------------------------------------------------------------
// Blt4444SmallNotTrans
//----------------------------------------------------------------------


//----------------------------------------------------------------------






//----------------------------------------------------------------------
void
CAlphaSprite::Blt4444SmallNotTrans(WORD *pDest, WORD pitch, BYTE shift)
{
	s_Value1 = shift;
	
	s_Value2 = (2 << shift) - 1; //((1 << s_Value1) << 1) - 1;	


	int		count,	
			transCount,
			colorCount,
			transCountShift,
			colorCountShift,
			totalCount,
			totalShiftCount,
			pixelGap,
			pixelGapShift;

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

			
			totalCount = 0;
			totalShiftCount = 0;
			if (count > 0)
			{			
				j = count;
				do
				{		
					transCount = *pPixels++;					
					colorCount = *pPixels++;		
					
					//--------------------------------------------------
					
					//--------------------------------------------------
					transCountShift = transCount >> shift;
					colorCountShift = colorCount >> shift;

					//--------------------------------------------------
					
					//--------------------------------------------------
					
					//--------------------------------------------------
					totalCount += transCount;
					totalShiftCount += transCountShift;

					
					pixelGap = totalCount - (totalShiftCount << shift);

					
					pixelGapShift = pixelGap >> shift;
					transCountShift += pixelGapShift;
					totalShiftCount += pixelGapShift;

					
					//--------------------------------------------------
					
					//--------------------------------------------------
					memset(pDestTemp, 0, transCountShift<<1);
					pDestTemp += transCountShift;		


					//--------------------------------------------------
					
					//--------------------------------------------------
					
					
					
					
					
					
					
					//
					
					//--------------------------------------------------
					totalCount += colorCount;
					totalShiftCount += colorCountShift;

					//--------------------------------------------------
					
					//--------------------------------------------------					
					memcpyAlpha4444Small(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCountShift;

					
					pPixels		+= (colorCount<<1);
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);

			i-=stepY;

		} while (i>0);
	}
}


//----------------------------------------------------------------------
// AlphaChannel Copy  4444 Small
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void	
CAlphaSprite::memcpyAlpha4444Small(WORD* pDest, WORD* pSource, WORD pixels)
{
	register WORD		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels >> s_Value1;

	BYTE alpha;
	
	// Alpha Channel Blending
	
	while (i--)
	{	
		
		alpha = *pSource >> 9;	
		pSource++;

		
		sTemp = *pSource;
	
		sr = (sTemp >> ColorDraw::s_bSHIFT4_R);// & 0x0F;
		sg = (sTemp >> ColorDraw::s_bSHIFT4_G) & 0x0F;
		sb = (sTemp >> ColorDraw::s_bSHIFT4_B) & 0x0F;
		
		*pDest =	(alpha << 12) |
					(sr << 8) |
					(sg << 4) |
					(sb);
	
		pDest++;
		pSource += s_Value2;
	}
}