#include "client_PCH.h"
#include "CSpritePal.h"
#include "CSpriteSurface.h"


DWORD	CSpritePal::s_Value1 = 1;
DWORD	CSpritePal::s_Value2 = 31;
DWORD	CSpritePal::s_Value3 = 1;

void CSpritePal::SetPixel(BYTE *pSource, WORD pitch, WORD width, WORD height)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	BYTE*	data = new BYTE[m_Width*2+10];

	int		index;				
	int		count;				
	int		trans,				
			color;				

	BOOL	bCheckTrans;		

	BYTE	*pSourceTemp;


	
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


//		for (j=0; j<width; j++)
//		{

//			if (*pSourceTemp==s_Colorkey)
//			{

//				if (!bCheckTrans)
//				{

//					count++;					
//					data[lastColorIndex] = color;
//					color = 0;
//					bCheckTrans = TRUE;
//				}				
//				trans++;				
//			}
//			else
//			{

//				if (bCheckTrans)
//				{						

//					trans = 0;

//					bCheckTrans = FALSE;
//				}

//				color++;								
//			}
//			pSourceTemp++;
//		}
		
		for( j=0; j<width;)
		{
			int trans_count = 0;
			int image_count = 0;
			
			while( *(pSourceTemp+j) == s_Colorkey )
			{
				if( j >= width  || trans_count == 255)
					break;
				trans_count++;
				j++;				
			}
			count++;
			data[index++] = trans_count;
			
			while( *(pSourceTemp + j) != s_Colorkey )
			{
				if( j>= width || image_count == 255 )
					break;
				image_count ++;
				j++;
				
			}
			data[index++] = image_count;
			if( image_count > 0)
			{
				memcpy(data+index, pSourceTemp+(j-image_count), image_count);
				index+=image_count;
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

		pSource = pSource + pitch;
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

void CSpritePal::Blt(int x, int y, WORD* pDest, int pitch, MPalette &pal)
{
	int	count,			
		colorCount;
	
	WORD	*pDestTemp;
	BYTE	*pPixels;
	
	register int i;
	register int j;
	register int k;
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
					
					
					for(k = 0; k < colorCount; k++)
					{
						memcpy((void*)(pDestTemp+k), (void*)(&pal[*(pPixels+k)]), 2);
					}
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}
			
		} while (i++ < rectBottom-1);
	}
}

//----------------------------------------------------------------------
// BltEffect
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSpritePal::BltEffect(WORD *pDest, WORD pitch, MPalette &pal)
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

					
					CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);
					
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
CSpritePal::BltEffectClipLeft(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
{
	WORD	*pDestTemp;
	BYTE	*pPixels;

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

						
						CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);
						pDestTemp += colorCount;
						pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = rectLeft - index;

						
						CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels+dist, colorCount-dist, pal);					
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
					
					
					CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);

					
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
CSpritePal::BltEffectClipRight(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
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
					
						
						CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, rectRight - index, pal);
						break;
					}
				}

				
				pDestTemp += transCount;

				
				CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);
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
CSpritePal::BltEffectClipWidth(WORD* pDest, WORD pitch, RECT* pRect, MPalette &pal)
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

	//---------------------------------------------
	
	//---------------------------------------------
	int rectBottom = pRect->bottom;
	int rectLeft = pRect->left;
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
								CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, rectRight - index, pal);
							}

							j = 0;
							
							break;
						}

						
						CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);
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
							CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels+dist, rectRight - rectLeft, pal);

							j = 0;						
							break;
						}		

						
						CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels+dist, colorCount-dist, pal);					
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
						
							
							CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, rectRight - index, pal);
							break;
						}
					}

					
					pDestTemp += transCount;

					
					CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);
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
CSpritePal::BltEffectClipHeight(WORD *pDest, WORD pitch, RECT* pRect, MPalette &pal)
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

				
				CSpriteSurface::memcpyPalEffect(pDestTemp, pPixels, colorCount, pal);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
} 

//----------------------------------------------------------------------
// Blt1555NotTrans
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CSpritePal::Blt1555NotTrans(WORD *pDest, WORD pitch, MPalette &pal)
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
					
					
					memcpy1555(pDestTemp, pPixels, colorCount, pal);
					
					pDestTemp	+= colorCount;
					
					pPixels		+= colorCount;
				} while (--j);
			}
			
			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// Blt1555SmallNotTrans
//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void
CSpritePal::Blt1555SmallNotTrans(WORD *pDest, WORD pitch, BYTE shift, MPalette &pal)
{
	
	s_Value2 = shift;
	
	s_Value3 = 1 << shift;
	
	
	int		count,	
		transCount,
		colorCount,
		transCountShift,
		colorCountShift;
	
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
					
					
					memcpy1555Small(pDestTemp, pPixels, colorCount, pal);
					
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
// AlphaChannel Copy  1555
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//----------------------------------------------------------------------
void	
CSpritePal::memcpy1555(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	register int		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;
	
	register int i = pixels;
	
	// Alpha Channel Blending
	
	while (i--)
	{	
		
		sTemp = pal[*pSource];
		
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
// Alpha Copy  1555 Small
//----------------------------------------------------------------------

//----------------------------------------------------------------------


//

//


//----------------------------------------------------------------------
void	
CSpritePal::memcpy1555Small(WORD* pDest, BYTE* pSource, WORD pixels, MPalette &pal)
{
	register int		sTemp;
	register int		sr,sg,sb;
	//static WORD		temp, temp2;
	
	register int i = pixels >> s_Value2;
	
	
	while (i--)
	{	
		
		sTemp = pal[*pSource];
		
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

bool
CSpritePal::IsColorPixel( short x, short y )
{
	if( x >= m_Width || x< 0 || y>= m_Height || y< 0)
		return false;

	int	count, colorCount , offset=0;
	
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
			pPixels	+= colorCount;
		} while (--j);
	}	
	return false;
}

WORD
CSpritePal::GetPixel( short x, short y, MPalette &pal )
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
				pPixels += (x - offset);
				return pal[*pPixels];
			}
			pPixels += colorCount;
		} while(--j);
	}

	return 0;
}
