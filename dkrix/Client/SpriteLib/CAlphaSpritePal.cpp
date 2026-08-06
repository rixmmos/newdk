
#include "Client_PCH.h"
#include "CAlphaSpritePal.h"

#include "CSpriteSurface.h"

int		CAlphaSpritePal::s_Value1	= 0;
int		CAlphaSpritePal::s_Value2	= 0;

// Constructor and destructor for vtable
CAlphaSpritePal::CAlphaSpritePal()
{
}

CAlphaSpritePal::~CAlphaSpritePal()
{
}


void CAlphaSpritePal::SetPixel(BYTE *pSource, WORD pitch, BYTE *pSourceAlpha, WORD alphaPitch, WORD width, WORD height)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	BYTE*	data = new BYTE[m_Width*4+10];

	int		index;

	int		count;				
	int		trans,				
			color;				

	BOOL	bCheckTrans;		

	BYTE	*pSourceTemp;
	BYTE	*pAlphaTemp;


	
	m_pPixels = new BYTE* [height];
	BYTE **Pixels = new BYTE* [height];
	std::vector<int> PixelSize;

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
		pAlphaTemp = pSourceAlpha;
		
		for( j=0; j<width;)
		{
			int trans_count = 0;
			int image_count = 0;
			
			while( *(pSourceTemp+j) == s_Colorkey || *(pAlphaTemp+j) == 0 )
			{
				if( j >= width  || trans_count == 255)
					break;
				trans_count++;
				j++;				
			}
			count++;
			data[index++] = trans_count;
			
			while( *(pSourceTemp + j) != s_Colorkey && *(pAlphaTemp+j) != 0)
			{
				if( j>= width || image_count == 255 )
					break;
				image_count ++;
				j++;
				
			}
			data[index++] = image_count;
			
			if( image_count > 0)
			{
				for( int t = 0; t < image_count; t++ )
				{
					BYTE alpha;
					
					alpha = *( pAlphaTemp + ( j - image_count ) + t);
					data[index++] = alpha;
					data[index++] = *( pSourceTemp + ( j - image_count ) + t );
				}
			}			
		}
 
		

//		if (bCheckTrans)
//		{

//		}	

//		else
//		{			
//			count++;
//			data[lastColorIndex] = color;
//		}
		
		
		Pixels[i] = new BYTE [index+1];
		m_Size += index+1;

		
		
		Pixels[i][0] = count;
		memcpy(Pixels[i]+1, data, index);
		PixelSize.push_back(index+1);

		pSource += pitch;
		pSourceAlpha += alphaPitch;
	}

	m_pData = new BYTE[m_Size+sizeof(BYTE *)*height];
	m_pPixels = (BYTE **)(m_pData+m_Size);
	BYTE *TempData = m_pData;

	for(i = 0; i < height; i++)
	{
		memcpy(TempData, Pixels[i], PixelSize[i]);
		m_pPixels[i] = TempData;
		TempData += PixelSize[i];
		delete [] Pixels[i];
	}

	delete [] data;
	delete [] Pixels;

	m_bInit = true;
}

void CAlphaSpritePal::Blt(int x, int y, WORD* pDest, int pitch, MPalette &pal)
{
	int	count,			
		colorCount;
	
	WORD	*pDestTemp;
	BYTE	*pPixels;
	
	register int i;
	register int j;
	int rectBottom = m_Height;
	
	pDest = (WORD*)((BYTE*)pDest + y*pitch +x*2);
	
	if (rectBottom > 0)
	{
		i = 0;
		
		do
		{
			pPixels		= m_pPixels[i];
			pDestTemp = (WORD*)((BYTE*)pDest + i*pitch);
			
			
			count	= *pPixels++;		
			
			if (count > 0)
			{	
				j = count;
				do {
					pDestTemp += *pPixels++;			
					colorCount = *pPixels++;		
					
					
					memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
					
					pDestTemp	+= colorCount;
					
					pPixels		+= (colorCount<<1);
				} while (--j);
			}
			
		} while (i++ < rectBottom-1);
	}
}

//----------------------------------------------------------------------
// AlphaChannel Copy
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void	
CAlphaSpritePal::memcpyAlpha(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	register WORD		sTemp,dTemp;
	register int		sr,sg,sb,dr,dg,db;

	register int i = pixels;
	
	BYTE alpha;
	
	// Alpha Channel Blending
	
	while (i--)
	{	
		
		alpha = *pSource;

		pSource++;
		
		
		sTemp = pal[*pSource];
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
CAlphaSpritePal::memcpyAlpha4444(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	register WORD		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels;

	BYTE alpha;

	// Alpha Channel Blending
	
	while (i--)
	{	
		
		alpha = *pSource >> 1;	//	alpha = (*pSource >> 8) >> 1;
		pSource++;

		
		sTemp = pal[*pSource];
	
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
CAlphaSpritePal::BltClip(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
						
						memcpyAlpha(pDestTemp, pPixels, pRect->right - index, pal);
						break;
					}						

					
					memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
					pDestTemp += colorCount;
				}				
				
				else
				{
					
					if (index > pRect->left)
					{	
						pDestTemp += index - pRect->left;
						memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
						pDestTemp += colorCount;
						bPut = TRUE;
					}
					else
					if (index+colorCount > pRect->left)
					{
						dist = pRect->left - index;

						
						memcpyAlpha(pDestTemp, pPixels+dist, colorCount-dist, pal);
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
CAlphaSpritePal::Blt(WORD *pDest, WORD pitch, MPalette &pal)
{
	int	count,			
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;

	register int i;
	register int j;
	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_pPixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{			
				j = count;
				do
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
					
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
CAlphaSpritePal::BltClipLeft(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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

						
						memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
					
					
					memcpyAlpha(pDestTemp, pPixels, colorCount, pal);

					
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
CAlphaSpritePal::BltClipRight(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
					
						
						memcpyAlpha(pDestTemp, pPixels, rectRight - index, pal);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::BltClipWidth(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
								memcpyAlpha(pDestTemp, pPixels, (rectRight - index), pal);
							}

							j = 0;
							
							break;
						}
						memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
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
							memcpyAlpha(pDestTemp, pPixels+(dist<<1), (rectRight - rectLeft), pal);

							j = 0;						
							break;
						}		
						memcpyAlpha(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
						
							
							memcpyAlpha(pDestTemp, pPixels, rectRight - index, pal);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	int	count,			
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;

	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_pPixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		
		if (count > 0)
		{			
			j = count;
			do
			{
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				memcpyAlpha(pDestTemp, pPixels, colorCount, pal);
				
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
CAlphaSpritePal::Blt4444(WORD *pDest, WORD pitch, MPalette &pal)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_pPixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			
			if (count > 0)
			{			
				j = count;
				do
				{				
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
					
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
CAlphaSpritePal::Blt4444ClipLeft(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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

						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
					
					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);

					
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
CAlphaSpritePal::Blt4444ClipRight(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
					
						
						memcpyAlpha4444(pDestTemp, pPixels, rectRight - index, pal);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::Blt4444ClipWidth(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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

						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
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

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
						
							
							memcpyAlpha4444(pDestTemp, pPixels, rectRight - index, pal);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::Blt4444ClipHeight(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;

	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_pPixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		
		if (count > 0)
		{			
			j = count;
			do
			{	
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
				
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
CAlphaSpritePal::Blt4444NotTrans(WORD *pDest, WORD pitch, MPalette &pal)
{
	int		count,	
			transCount,
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;

	register int i;
	register int j;

	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_pPixels[i];
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

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
					
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
CAlphaSpritePal::Blt4444NotTransClipLeft(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
						
						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
					
					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);

					
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
CAlphaSpritePal::Blt4444NotTransClipRight(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
					
						
						memcpyAlpha4444(pDestTemp, pPixels, rectRight - index, pal);
						break;
					}
				}

				
				memset(pDestTemp, 0, transCount<<1);
				pDestTemp += transCount;		

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::Blt4444NotTransClipWidth(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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

						
						memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
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

						
						memcpyAlpha4444(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
						
							
							memcpyAlpha4444(pDestTemp, pPixels, rectRight - index, pal);
							break;
						}
					}

					
					memset(pDestTemp, 0, transCount<<1);
					pDestTemp += transCount;		

					
					memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::Blt4444NotTransClipHeight(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	int		count,
			transCount,
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;

	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_pPixels[i];
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

				
				memcpyAlpha4444(pDestTemp, pPixels, colorCount, pal);
				
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
CAlphaSpritePal::BltAlpha(WORD *pDest, WORD pitch, BYTE alpha, MPalette &pal)
{
	s_Value1 = alpha;

	int	count,			
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;

	register int i;
	register int j;
	int rectBottom = m_Height;

	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_pPixels[i];
			pDestTemp	= pDest;

			
			count	= *pPixels++;		

			
			if (count > 0)
			{			
				j = count;
				do
				{
					pDestTemp += *pPixels++;		
					colorCount = *pPixels++;		

					
					memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);
					
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
CAlphaSpritePal::BltAlphaClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal)
{
	s_Value1 = alpha;

	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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

						
						memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);
						pDestTemp += colorCount;
						pPixels += (colorCount<<1);

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						memcpyAlphaValue(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
					
					
					memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);

					
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
CAlphaSpritePal::BltAlphaClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal)
{
	s_Value1 = alpha;

	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
					
						
						memcpyAlphaValue(pDestTemp, pPixels, rectRight - index, pal);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::BltAlphaClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal)
{
	s_Value1 = alpha;

	BYTE	*pPixels;
	WORD	*pDestTemp;

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
		pPixels = m_pPixels[i];
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
								memcpyAlphaValue(pDestTemp, pPixels, (rectRight - index), pal);
							}

							j = 0;
							
							break;
						}

						memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);
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
							memcpyAlphaValue(pDestTemp, pPixels+(dist<<1), (rectRight - rectLeft), pal);

							j = 0;						
							break;
						}		

						memcpyAlphaValue(pDestTemp, pPixels+(dist<<1), colorCount-dist, pal);
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
						
							
							memcpyAlphaValue(pDestTemp, pPixels, rectRight - index, pal);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);
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
CAlphaSpritePal::BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha, MPalette &pal)
{
	s_Value1 = alpha;

	int	count,			
			colorCount;

	WORD	*pDestTemp;
	BYTE	*pPixels;


	register int i;
	register int j;

	int rectBottom = pRect->bottom;

	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_pPixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		
		if (count > 0)
		{			
			j = count;
			do
			{
				pDestTemp += *pPixels++;		
				colorCount = *pPixels++;		

				
				memcpyAlphaValue(pDestTemp, pPixels, colorCount, pal);
				
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
CAlphaSpritePal::memcpyAlphaValue(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
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

		
		sTemp = pal[*pSource];
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
CAlphaSpritePal::Blt4444SmallNotTrans(WORD *pDest, WORD pitch, BYTE shift, MPalette &pal)
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

	WORD	*pDestTemp;
	BYTE	*pPixels;

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
			pPixels		= m_pPixels[i];
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
					memcpyAlpha4444Small(pDestTemp, pPixels, colorCount, pal);
					
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
CAlphaSpritePal::memcpyAlpha4444Small(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	register WORD		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;

	register int i = pixels >> s_Value1;

	BYTE alpha;
	
	// Alpha Channel Blending
	
	while (i--)
	{	
		
		alpha = *pSource >> 1;	
		pSource++;

		
		sTemp = pal[*pSource];
	
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

bool
CAlphaSpritePal::IsColorPixel(short x, short y)
{
	if( y >= m_Height || y < 0 || x < 0 || x >= m_Width )
		return false;

	int	colorCount, count;
	int offset=0;

	const BYTE	*pPixels = m_pPixels[y];
	
	register int j;
	count	= *pPixels++;		
	if (count > 0)
	{	
		j = count;
		do {
			offset += *pPixels++;
			if( offset > x ) return false;
			colorCount = *pPixels++;		
			offset += colorCount;
			if( offset > x ) return true;
			pPixels		+= (colorCount<<1);
		} while (--j);
	}
	return false;
}

WORD
CAlphaSpritePal::GetPixel( short x, short y, MPalette &pal )
{
	if( y >= m_Height || y < 0 || x < 0 || x >= m_Width )
		return 0;
	
	const BYTE *pPixels = m_pPixels[y];
	int offset=0, colorCount,count;
	register int j;

	count = *pPixels++;

	if(count > 0 )
	{
		j = count;

		do {
			offset += *pPixels++;			
			if( offset > x ) return 0;			
			colorCount = *pPixels++;
			offset += colorCount;
			if( offset > x )		
			{
				offset -= colorCount;
				pPixels ++;		
				pPixels += ((x - offset)<<1);
				return pal[*pPixels];
			}
			pPixels += (colorCount<<1);
		} while(--j);
	}

	return 0;
}
