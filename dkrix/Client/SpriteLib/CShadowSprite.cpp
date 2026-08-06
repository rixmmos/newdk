//----------------------------------------------------------------------
// CShadowSprite.cpp
//----------------------------------------------------------------------
#include "client_PCH.h"
#include "CSpriteSurface.h"
#include "CFilter.h"
#include "CShadowSprite.h"

#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
#endif


//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
WORD	CShadowSprite::s_Colorkey = 0;
BYTE	CShadowSprite::s_Value1 = 1;
BYTE	CShadowSprite::s_Value2 = 31;
WORD	CShadowSprite::s_wValue1 = 1;


//----------------------------------------------------------------------
// Init Static member
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//
// constructor/destructore
//
//----------------------------------------------------------------------

CShadowSprite::CShadowSprite()
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

CShadowSprite::~CShadowSprite()
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
CShadowSprite::Release()
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
		
		delete [] m_Pixels;

		m_Pixels	= NULL;
		m_Width		= 0;
		m_Height	= 0;	
	}
}

//----------------------------------------------------------------------
// operator = 
//----------------------------------------------------------------------
void		
CShadowSprite::operator = (const CShadowSprite& Sprite)
{
	
	Release();

	
	if (Sprite.m_Pixels==NULL || Sprite.m_Width==0 || Sprite.m_Height==0)
		return;

	
	m_Width = Sprite.m_Width;
	m_Height = Sprite.m_Height;	

	
	WORD index;	

	
	m_Pixels = new WORD* [m_Height];

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		index	= 1 + m_Pixels[i][0]*2;	

		
		m_Pixels[i] = new WORD [index];
		memcpy(m_Pixels[i], Sprite.m_Pixels[i], index<<1);
	}

}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CShadowSprite::SaveToFile(ofstream& file)
{
	
	file.write((const char*)&m_Width , 2);
	file.write((const char*)&m_Height, 2);

	
	if (m_Pixels==NULL || m_Width==0 || m_Height==0)
		return false;
	
	
	WORD index;	

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		index	= 1 + m_Pixels[i][0]*2;	

		
		file.write((const char*)&index, 2);
		file.write((const char*)m_Pixels[i], index<<1);
	}


	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool	
CShadowSprite::LoadFromFile(ifstream& file)
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
	//m_Pixels = (WORD**)malloc(sizeof(WORD*)*m_Height);

	WORD len;

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		file.read((char*)&len, 2);
		
		m_Pixels[i] = NULL;
		m_Pixels[i] = new WORD [len];		
	
		file.read((char*)m_Pixels[i], len<<1);
	}	
	
	m_bInit = true;

	return true;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//

//

//


//
//----------------------------------------------------------------------
void
CShadowSprite::SetPixel(WORD *pSource, WORD pitch, WORD width, WORD height)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	WORD*	data = new WORD[m_Width+2];

	int	index;				
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
			
			if (*pSourceTemp==s_Colorkey)
			{
				
				if (!bCheckTrans)
				{
					
					count++;
					
					data[index++] = color;
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

					bCheckTrans = FALSE;
				}

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
			data[index++] = color;
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
// SetPixel
//----------------------------------------------------------------------
// IndexSprite --> ShadowSprite
//----------------------------------------------------------------------
void		
CShadowSprite::SetPixel(CIndexSprite& ispr)
{
	
	Release();

	m_Width = ispr.GetWidth();
	m_Height = ispr.GetHeight();

	int		transPair,	
			indexCount,			
			colorCount,
			index;
	
	WORD	*pPixels;

	register int i;
	register int j;	

	
	m_Pixels = new WORD* [m_Height];

	for (int i=0; i<m_Height; i++)
	{			
		pPixels		= ispr.GetPixelLine( i );
		
		
		transPair	= *pPixels++;

		
		m_Pixels[i] = new WORD [1 + (transPair<<1)];
		index = 0;
		m_Pixels[i][index++] = transPair;
		
 		
		for (j=0; j<transPair; j++)
		{			
			//------------------------------------
			
			//------------------------------------
			m_Pixels[i][index++] = *pPixels;	
			pPixels++;			

			indexCount = *pPixels++;	
			pPixels += indexCount;		
			
			colorCount = *pPixels++;	
			pPixels		+= colorCount;	

			//------------------------------------
			
			//------------------------------------
			m_Pixels[i][index++] = indexCount + colorCount;
		}
	}

	m_bInit = true;
}

//----------------------------------------------------------------------
// SetPixel
//----------------------------------------------------------------------
// Sprite --> ShadowSprite
//----------------------------------------------------------------------
void		
CShadowSprite::SetPixel(CSprite& spr)
{
	
	Release();

	m_Width = spr.GetWidth();
	m_Height = spr.GetHeight();

	int		transPair,	
			colorCount,
			index;
	
	WORD	*pPixels;

	register int i;
	register int j;	

	
	m_Pixels = new WORD* [m_Height];

	for (int i=0; i<m_Height; i++)
	{			
		pPixels		= spr.GetPixelLine( i );
		
		
		transPair	= *pPixels++;

		
		m_Pixels[i] = new WORD [1 + (transPair<<1)];
		index = 0;
		m_Pixels[i][index++] = transPair;
		
 		
		for (j=0; j<transPair; j++)
		{			
			//------------------------------------
			
			//------------------------------------
			m_Pixels[i][index++] = *pPixels;	
			pPixels++;			

			colorCount = *pPixels++;	
			pPixels		+= colorCount;	

			//------------------------------------
			
			//------------------------------------
			m_Pixels[i][index++] = colorCount;
		}
	}

	m_bInit = true;
}

/*
void		
CShadowSprite::Uncompress()
{
	m_bCompress = FALSE;
}
*/

//----------------------------------------------------------------------
// Is ColorPixel ?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CShadowSprite::IsColorPixel(short x, short y)
{
	
	if (m_Pixels==NULL)
		return false; 

	
	if (x<0 || y<0 || x>=m_Width || y>=m_Height)
		return false;

	
	WORD	*pPixels = m_Pixels[y];

	
	int	count = *pPixels++;

	int	transCount, 
			colorCount,
			index = 0;

	for (int register i=0; i<count; i++)
	{
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

		//pPixels += colorCount;
		index += colorCount;
	}

	return false;
}

//----------------------------------------------------------------------
// Blt
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CShadowSprite::Blt(WORD *pDest, WORD pitch)
{
	int	count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	for (int i=0; i<m_Height; i++)
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

				
				memset((void*)pDestTemp, 0, colorCount<<1);
		
				pDestTemp	+= colorCount;
				//pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
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
	for (int i=pRect->top; i<pRect->bottom; i++)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += index - pRect->left;

						
						//memcpy(pDestTemp, pPixels, colorCount<<1);
						
						memset((void*)pDestTemp, 0, colorCount<<1);

						pDestTemp += colorCount;
						//pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = pRect->left - index;

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						memset((void*)pDestTemp, 0, (colorCount-dist)<<1);

						pDestTemp += colorCount-dist;
						//pPixels += colorCount;

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
					
					
					memset((void*)pDestTemp, 0, colorCount<<1);

					
					pDestTemp += colorCount;
					//pPixels += colorCount;			
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
CShadowSprite::BltClipRight(WORD* pDest, WORD pitch, RECT* pRect)
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

	for (int i=pRect->top; i<pRect->bottom; i++)
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
				if (index+colorCount > pRect->right)
				{
					
					if (index > pRect->right)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
						
						memset((void*)pDestTemp, 0, (pRect->right - index)<<1);

						break;
					}
				}

				
				pDestTemp += transCount;

				
				//memcpy(pDestTemp, pPixels, colorCount<<1);
				
				memset((void*)pDestTemp, 0, colorCount<<1);

				pDestTemp += colorCount;
				//pPixels += colorCount;			
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
CShadowSprite::BltClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
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

	register short i;
	register short j;

	//---------------------------------------------
	
	//---------------------------------------------
	for (int i=pRect->top; i<pRect->bottom; i++)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += index - pRect->left;

						
						
						if (index+colorCount > pRect->right)
						{							
							
							if (index > pRect->right)
							{
							}
							else
							{
								memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
							}

							j = count;
							
							break;
						}
					
						memset((void*)pDestTemp, 0, colorCount<<1);

						pDestTemp += colorCount;
						//pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{		
						dist = pRect->left - index;

						
						if (index+colorCount > pRect->right)
						{						
							memset((void*)pDestTemp, 0, pRect->right - pRect->left);

							j = count;						
							break;
						}

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						memset((void*)pDestTemp, 0, (colorCount-dist)<<1);

						pDestTemp += colorCount-dist;
						//pPixels += colorCount;		
						index += colorCount;

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
					if (index+colorCount > pRect->right)
					{
						
						if (index > pRect->right)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
							
							memset((void*)pDestTemp, 0, (pRect->right - index)<<1);

							break;
						}
					}

					
					pDestTemp += transCount;

					
					//memcpy(pDestTemp, pPixels, colorCount<<1);
					
					memset((void*)pDestTemp, 0, colorCount<<1);

					pDestTemp += colorCount;
					//pPixels += colorCount;			
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
CShadowSprite::BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	for (int i=pRect->top; i<pRect->bottom; i++)
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

				
				//memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
				
				memset((void*)pDestTemp, 0, colorCount<<1);
				
				pDestTemp	+= colorCount;
				//pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}


//----------------------------------------------------------------------
// BltDarkness
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CShadowSprite::BltDarkness(WORD *pDest, WORD pitch, BYTE DarkBits)
{
	s_Value1 = DarkBits;

	int	count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	for (int i=0; i<m_Height; i++)
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

				
				//memset((void*)pDestTemp, 0, colorCount<<1);
				memcpyShadowDarkness(pDestTemp, colorCount);
		
				pDestTemp	+= colorCount;
				//pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltDarkness ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltDarknessClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	s_Value1 = DarkBits;

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
	for (int i=pRect->top; i<pRect->bottom; i++)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += index - pRect->left;

						
						//memcpy(pDestTemp, pPixels, colorCount<<1);
						
						//memset((void*)pDestTemp, 0, colorCount<<1);
						memcpyShadowDarkness(pDestTemp, colorCount);

						pDestTemp += colorCount;
						//pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = pRect->left - index;

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						//memset((void*)pDestTemp, 0, (colorCount-dist)<<1);
						memcpyShadowDarkness(pDestTemp, colorCount-dist);

						pDestTemp += colorCount-dist;
						//pPixels += colorCount;

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
					
					
					//memset((void*)pDestTemp, 0, colorCount<<1);
					memcpyShadowDarkness(pDestTemp, colorCount);

					
					pDestTemp += colorCount;
					//pPixels += colorCount;			
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
CShadowSprite::BltDarknessClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	s_Value1 = DarkBits;

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

	for (int i=pRect->top; i<pRect->bottom; i++)
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
				if (index+colorCount > pRect->right)
				{
					
					if (index > pRect->right)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
						
						//memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
						memcpyShadowDarkness(pDestTemp, pRect->right - index);

						break;
					}
				}

				
				pDestTemp += transCount;

				
				//memcpy(pDestTemp, pPixels, colorCount<<1);
				
				//memset((void*)pDestTemp, 0, colorCount<<1);
				memcpyShadowDarkness(pDestTemp, colorCount);

				pDestTemp += colorCount;
				//pPixels += colorCount;			
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
CShadowSprite::BltDarknessClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	s_Value1 = DarkBits;

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
	for (int i=pRect->top; i<pRect->bottom; i++)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += index - pRect->left;

						
						
						if (index+colorCount > pRect->right)
						{							
							
							if (index > pRect->right)
							{
							}
							else
							{
								//memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
								memcpyShadowDarkness(pDestTemp, pRect->right - index);
							}

							j = count;
							
							break;
						}

						
						//memcpy(pDestTemp, pPixels, colorCount<<1);
						
						//memset((void*)pDestTemp, 0, colorCount<<1);
						memcpyShadowDarkness(pDestTemp, colorCount);

						pDestTemp += colorCount;
						//pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = pRect->left - index;

						
						if (index+colorCount > pRect->right)
						{
							memcpyShadowDarkness(pDestTemp, pRect->right - pRect->left);

							j = count;						
							break;
						}

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						//memset((void*)pDestTemp, 0, (colorCount-dist)<<1);
						memcpyShadowDarkness(pDestTemp, colorCount-dist);

						pDestTemp += colorCount-dist;
						//pPixels += colorCount;		
						index += colorCount;

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
					if (index+colorCount > pRect->right)
					{
						
						if (index > pRect->right)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;
						
							
							//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
							
							//memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
							memcpyShadowDarkness(pDestTemp, pRect->right-index);

							break;
						}
					}

					
					pDestTemp += transCount;

					
					//memcpy(pDestTemp, pPixels, colorCount<<1);
					
					//memset((void*)pDestTemp, 0, colorCount<<1);
					memcpyShadowDarkness(pDestTemp, colorCount);

					pDestTemp += colorCount;
					//pPixels += colorCount;			
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
CShadowSprite::BltDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	s_Value1 = DarkBits;

	int	count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	for (int i=pRect->top; i<pRect->bottom; i++)
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

				
				//memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
				
				//memset((void*)pDestTemp, 0, colorCount<<1);
				memcpyShadowDarkness(pDestTemp, colorCount);
				
				pDestTemp	+= colorCount;
				//pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// ShadowDarkness Copy
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CShadowSprite::memcpyShadowDarkness(WORD* pDest, WORD pixels)
{
	QWORD	*qpDest		= (QWORD*)pDest;

	register int j;

	BYTE qTimes = pixels >> 2;	// pixels / 4

	
	switch ( pixels & 0x03 )	// pixels % 4
	{
		//------------------
		
		//------------------
		case 0 :			
			
			for (j=0; j<qTimes; j++)
			{
				*qpDest = ((*qpDest >> s_Value1) & ColorDraw::s_qwMASK_SHIFT[s_Value1]);				

				*qpDest++;				
			}
		break;

		//------------------
		
		//------------------
		case 1 :
			
			*(WORD*)qpDest = ((*(WORD*)qpDest >> s_Value1) & ColorDraw::s_wMASK_SHIFT[s_Value1]);
			
				
			qpDest = (QWORD*)((WORD*)qpDest + 1);			

			
			for (j=0; j<qTimes; j++)
			{
				*qpDest = ((*qpDest >> s_Value1) & ColorDraw::s_qwMASK_SHIFT[s_Value1]);

				*qpDest++;				
			}
			
		break;

		//------------------
		
		//------------------
		case 2 :
			
			*(DWORD*)qpDest = ((*(DWORD*)qpDest >> s_Value1) & ColorDraw::s_dwMASK_SHIFT[s_Value1]);			
				
			qpDest = (QWORD*)((DWORD*)qpDest + 1);			

			
			for (j=0; j<qTimes; j++)
			{
				*qpDest = ((*qpDest >> s_Value1) & ColorDraw::s_qwMASK_SHIFT[s_Value1]);

				*qpDest++;				
			}
		break;

		//------------------
		
		//------------------
		case 3 :
			
			*(WORD*)qpDest = ((*(WORD*)qpDest >> s_Value1) & ColorDraw::s_wMASK_SHIFT[s_Value1]);	

			
			*(DWORD*)qpDest = ((*(DWORD*)qpDest >> s_Value1) & ColorDraw::s_dwMASK_SHIFT[s_Value1]);
				
			qpDest = (QWORD*)((DWORD*)qpDest + 1);			

			
			for (j=0; j<qTimes; j++)
			{
				*qpDest = ((*qpDest >> s_Value1) & ColorDraw::s_qwMASK_SHIFT[s_Value1]);

				*qpDest++;				
			}
		break;
	}
}

//----------------------------------------------------------------------
// Blt4444
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CShadowSprite::Blt4444(WORD *pDest, WORD pitch, WORD pixel)
{
	s_wValue1 = pixel;

	int	count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;

	for (register int i=0; i<m_Height; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		
 		
		for (register int j=0; j<count; j++)
		{				
			pDestTemp += *pPixels++;			
			colorCount = *pPixels++;		

			
			//memset((void*)pDestTemp, 0, colorCount<<1);
			memcpyShadow4444(pDestTemp, colorCount);
	
			pDestTemp	+= colorCount;
			//pPixels		+= colorCount;
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::Blt4444ClipLeft(WORD* pDest, WORD pitch, RECT* pRect, WORD pixel)
{
	s_wValue1 = pixel;

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
	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		for (j=0; j<count; j++)
		{
			transCount = *pPixels++;		
			colorCount = *pPixels++;		
					
			
			index += transCount;
			
		
			//---------------------------------------------
			
			//---------------------------------------------
			if (index+colorCount > pRect->left)
			{
				//---------------------------------------------
				
				//---------------------------------------------
				if (index > pRect->left)
				{	
					
					pDestTemp += index - pRect->left;

					
					//memcpy(pDestTemp, pPixels, colorCount<<1);
					
					//memset((void*)pDestTemp, 0, colorCount<<1);
					memcpyShadow4444(pDestTemp, colorCount);

					pDestTemp += colorCount;
					//pPixels += colorCount;

					
					break;
				}
				//---------------------------------------------
				
				
				//---------------------------------------------
				else
				{
					dist = pRect->left - index;

					
					//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
					
					//memset((void*)pDestTemp, 0, (colorCount-dist)<<1);
					memcpyShadow4444(pDestTemp, colorCount-dist);

					pDestTemp += colorCount-dist;
					//pPixels += colorCount;

					
					break;
				}
			}					

			
			//pPixels += colorCount;
			index += colorCount;
		}

		//---------------------------------------------
		
		//---------------------------------------------		
		for (j++; j<count; j++)
		{
			transCount = *pPixels++;		
			colorCount = *pPixels++;		
					
			
			pDestTemp += transCount;			
			
			
			//memset((void*)pDestTemp, 0, colorCount<<1);
			memcpyShadow4444(pDestTemp, colorCount);

			
			pDestTemp += colorCount;
			//pPixels += colorCount;			
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::Blt4444ClipRight(WORD* pDest, WORD pitch, RECT* pRect, WORD pixel)
{
	s_wValue1 = pixel;

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

	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
			
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		for (j=0; j<count; j++)
		{
			transCount = *pPixels++;		
			colorCount = *pPixels++;		
					
			
			index += transCount;
			
			
			

			

			//---------------------------------------------
			
			//---------------------------------------------			
			if (index+colorCount > pRect->right)
			{
				
				if (index > pRect->right)
				{
					break;
				}
				
				else
				{
					pDestTemp += transCount;
				
					
					//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
					
					//memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
					memcpyShadow4444(pDestTemp, pRect->right - index);

					break;
				}
			}

			
			pDestTemp += transCount;

			
			//memcpy(pDestTemp, pPixels, colorCount<<1);
			
			//memset((void*)pDestTemp, 0, colorCount<<1);
			memcpyShadow4444(pDestTemp, colorCount);

			pDestTemp += colorCount;
			//pPixels += colorCount;			
			index += colorCount;
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CShadowSprite::Blt4444ClipWidth(WORD* pDest, WORD pitch, RECT* pRect, WORD pixel)
{
	s_wValue1 = pixel;

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
	for (int i=pRect->top; i<pRect->bottom; i++)
	{
		pPixels = m_Pixels[i];
		pDestTemp = pDest;		

		
		count = *pPixels++;		

		
		index = 0;
		
		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		for (j=0; j<count; j++)
		{
			transCount = *pPixels++;		
			colorCount = *pPixels++;		
					
			
			index += transCount;
			
		
			//---------------------------------------------
			
			//---------------------------------------------
			if (index+colorCount > pRect->left)
			{
				//---------------------------------------------
				
				//---------------------------------------------
				if (index > pRect->left)
				{	
					
					pDestTemp += index - pRect->left;

					
					
					if (index+colorCount > pRect->right)
					{							
						
						if (index > pRect->right)
						{
						}
						else
						{
							//memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
							memcpyShadow4444(pDestTemp, pRect->right - index);
						}

						j = count;
						
						break;
					}

					
					//memcpy(pDestTemp, pPixels, colorCount<<1);
					
					//memset((void*)pDestTemp, 0, colorCount<<1);
					memcpyShadow4444(pDestTemp, colorCount);

					pDestTemp += colorCount;
					//pPixels += colorCount;
					index += colorCount;

					
					break;
				}
				//---------------------------------------------
				
				
				//---------------------------------------------
				else
				{
					dist = pRect->left - index;

					
					if (index+colorCount > pRect->right)
					{
						memcpyShadow4444(pDestTemp, pRect->right - pRect->left);

						j = count;						
						break;
					}

					
					//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
					
					//memset((void*)pDestTemp, 0, (colorCount-dist)<<1);
					memcpyShadow4444(pDestTemp, colorCount-dist);

					pDestTemp += colorCount-dist;
					//pPixels += colorCount;		
					index += colorCount;

					
					break;
				}
			}					

			
			//pPixels += colorCount;
			index += colorCount;
		}

		//---------------------------------------------
		
		
		//---------------------------------------------
		
		//---------------------------------------------
		for (j++; j<count; j++)
		{
			transCount = *pPixels++;		
			colorCount = *pPixels++;		
					
			
			index += transCount;
			
			
			

			

			//---------------------------------------------
			
			//---------------------------------------------			
			if (index+colorCount > pRect->right)
			{
				
				if (index > pRect->right)
				{
					break;
				}
				
				else
				{
					pDestTemp += transCount;
				
					
					//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
					
					//memset((void*)pDestTemp, 0, (pRect->right - index)<<1);
					memcpyShadow4444(pDestTemp, pRect->right-index);

					break;
				}
			}

			
			pDestTemp += transCount;

			
			//memcpy(pDestTemp, pPixels, colorCount<<1);
			
			//memset((void*)pDestTemp, 0, colorCount<<1);
			memcpyShadow4444(pDestTemp, colorCount);

			pDestTemp += colorCount;
			//pPixels += colorCount;			
			index += colorCount;
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt4444 Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CShadowSprite::Blt4444ClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel)
{
	s_wValue1 = pixel;

	int	count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;

	for (int i=pRect->top; i<pRect->bottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		for (j=0; j<count; j++)
		{				
			pDestTemp += *pPixels++;		
			colorCount = *pPixels++;		

			
			//memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
			
			//memset((void*)pDestTemp, 0, colorCount<<1);
			memcpyShadow4444(pDestTemp, colorCount);
			
			pDestTemp	+= colorCount;
			//pPixels		+= colorCount;
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// ShadowDarkness Copy
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CShadowSprite::memcpyShadow4444(WORD* pDest, WORD pixels)
{
	register int i = pixels;

	if (i>0)
	{
		do {	
			*pDest = s_wValue1;
			pDest++;
		} while (--i);
	}
}


//----------------------------------------------------------------------
// Blt Small
//----------------------------------------------------------------------

//

// shift = 0 , 100*100
// shift = 1 , 50*50
// shift = 2 , 25*25
// shift = 3 , 12*12
// shift = 4 , 6*6
// ...

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltSmall(WORD *pDest, WORD pitch, BYTE shift)
{
	int		count,		
			transCount,
			transCountShift,
			colorCount,
			colorCountShift,
			totalCount,
			totalShiftCount,
			pixelGap,
			pixelGapShift;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	if (m_Height > 0)
	{
		i = m_Height-1;
		int stepY = 1 << shift;		
		pDest = (WORD*)((BYTE*)pDest + (i>>shift)*pitch);

		do {	
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
					pDestTemp += transCountShift;		

					
					//--------------------------------------------------
					
					//--------------------------------------------------
					totalCount += colorCount;
					totalShiftCount += colorCountShift;
					
					//--------------------------------------------------
					
					//--------------------------------------------------
					memset((void*)pDestTemp, 0, colorCount);
			
					pDestTemp	+= colorCountShift;
					//pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
			
			i-=stepY;

		} while (i>0);
	}
}

//----------------------------------------------------------------------
// BltSmall ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltSmallClipLeft(WORD* pDest, WORD pitch, RECT* pRect, BYTE shift)
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
	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += (index - pRect->left) >> shift;

						
						colorCount >>= shift;

						
						//memcpy(pDestTemp, pPixels, colorCount<<1);
						
						memset((void*)pDestTemp, 0, colorCount<<1);

						pDestTemp += colorCount;
						//pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = pRect->left - index;

						
						colorCount = (colorCount-dist) >> shift;

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						memset((void*)pDestTemp, 0, colorCount<<1);

						pDestTemp += colorCount;
						//pPixels += colorCount;

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
							
					
					pDestTemp += transCount>>shift;			

					
					colorCount >>= shift;
					
					
					memset((void*)pDestTemp, 0, colorCount<<1);

					
					pDestTemp += colorCount;
					//pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltSmallClipRight(WORD* pDest, WORD pitch, RECT* pRect, BYTE shift)
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

	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
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
				transCount = *pPixels++ >> shift;		
				colorCount = *pPixels++ >> shift;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > pRect->right)
				{
					
					if (index > pRect->right)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
						
						memset((void*)pDestTemp, 0, (pRect->right - index)<<1);

						break;
					}
				}

				
				pDestTemp += transCount;

				index += colorCount;
				
				
				//memcpy(pDestTemp, pPixels, colorCount<<1);
				
				memset((void*)pDestTemp, 0, colorCount<<1);

				pDestTemp += colorCount;
				//pPixels += colorCount;							
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CShadowSprite::BltSmallClipWidth(WORD* pDest, WORD pitch, RECT* pRect, BYTE shift)
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

	register short i;
	register short j;

	//---------------------------------------------
	
	//---------------------------------------------
	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += (index - pRect->left)>>shift;

						
						
						if (index+colorCount > pRect->right)
						{							
							
							if (index > pRect->right)
							{
							}
							else
							{
								memset((void*)pDestTemp, 0, ((pRect->right - index)>>shift)<<1);
							}

							j = count;
							
							break;
						}

						
						colorCount >>= shift;
					
						memset((void*)pDestTemp, 0, colorCount<<1);

						pDestTemp += colorCount;
						//pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{		
						dist = pRect->left - index;

						
						if (index+colorCount > pRect->right)
						{						
							memset((void*)pDestTemp, 0, (pRect->right - pRect->left)>>shift);

							j = count;						
							break;
						}

						
						index += colorCount;
						colorCount = (colorCount-dist) >> shift;

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						memset((void*)pDestTemp, 0, colorCount<<1);

						pDestTemp += colorCount;
						//pPixels += colorCount;		
						

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
					if (index+colorCount > pRect->right)
					{
						
						if (index > pRect->right)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount>>shift;
						
							
							//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
							
							memset((void*)pDestTemp, 0, ((pRect->right - index)>>shift)<<1);

							break;
						}
					}

					
					pDestTemp += transCount>>shift;


					index += colorCount;
					
					colorCount >>= shift;

					
					//memcpy(pDestTemp, pPixels, colorCount<<1);
					
					memset((void*)pDestTemp, 0, colorCount<<1);

					pDestTemp += colorCount;
					//pPixels += colorCount;								
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CShadowSprite::BltSmallClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE shift)
{
	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;
	
	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		if (count > 0)
		{			
			j = count;
			do
			{
				pDestTemp += ((*pPixels++)>>shift);		
				colorCount = *pPixels++;				
				
				colorCount >>= shift;

				
				memset((void*)pDestTemp, 0, colorCount<<1);
		
				pDestTemp	+= colorCount;
				//pPixels		+= colorCount;
			} while (--j);
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Small4444
//----------------------------------------------------------------------

//

// shift = 0 , 100*100
// shift = 1 , 50*50
// shift = 2 , 25*25
// shift = 3 , 12*12
// shift = 4 , 6*6
// ...

//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltSmall4444(WORD *pDest, WORD pitch, WORD pixel, BYTE shift)
{
	
	s_wValue1 = pixel;

	int		count,		
			transCount,
			transCountShift,
			colorCount,
			colorCountShift,
			totalCount,
			totalShiftCount,
			pixelGap,
			pixelGapShift;


	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;

	 

	int stepY = 1 << shift;		
	
	for (int i=0; i<m_Height; i+=stepY)
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
				pDestTemp += transCountShift;		
			
				//--------------------------------------------------
				
				//--------------------------------------------------
				totalCount += colorCount;
				totalShiftCount += colorCountShift;
				
				//--------------------------------------------------
				
				//--------------------------------------------------
				memcpyShadow4444(pDestTemp, colorCountShift);

				pDestTemp	+= colorCountShift;

				//pPixels		+= colorCount;
			} while (--j);

		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall4444 ClipLeft
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltSmall4444ClipLeft(WORD* pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift)
{
	
	s_wValue1 = pixel;

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
	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += (index - pRect->left) >> shift;

						
						colorCount >>= shift;

						
						//memcpy(pDestTemp, pPixels, colorCount<<1);
						
						memcpyShadow4444(pDestTemp, colorCount);

						pDestTemp += colorCount;
						//pPixels += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{
						dist = pRect->left - index;

						
						colorCount = (colorCount-dist) >> shift;

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						memcpyShadow4444(pDestTemp, colorCount);

						pDestTemp += colorCount;
						//pPixels += colorCount;

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
							
					
					pDestTemp += transCount>>shift;			

					
					colorCount >>= shift;
					
					
					memcpyShadow4444(pDestTemp, colorCount);

					
					pDestTemp += colorCount;
					//pPixels += colorCount;			
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall4444 ClipRight
//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CShadowSprite::BltSmall4444ClipRight(WORD* pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift)
{
	
	s_wValue1 = pixel;

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

	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
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
				transCount = *pPixels++ >> shift;		
				colorCount = *pPixels++ >> shift;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > pRect->right)
				{
					
					if (index > pRect->right)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;
					
						
						//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
						
						memcpyShadow4444(pDestTemp, (pRect->right - index));

						break;
					}
				}

				
				pDestTemp += transCount;

				index += colorCount;
				
				
				//memcpy(pDestTemp, pPixels, colorCount<<1);
				
				memcpyShadow4444(pDestTemp, colorCount);

				pDestTemp += colorCount;
				//pPixels += colorCount;							
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall4444 ClipWidth
//----------------------------------------------------------------------



//----------------------------------------------------------------------
void
CShadowSprite::BltSmall4444ClipWidth(WORD* pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift)
{
	
	s_wValue1 = pixel;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			colorCount,
			index,
			dist;

	register short i;
	register short j;

	//---------------------------------------------
	
	//---------------------------------------------
	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
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
				if (index+colorCount > pRect->left)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > pRect->left)
					{	
						
						pDestTemp += (index - pRect->left)>>shift;

						
						
						if (index+colorCount > pRect->right)
						{							
							
							if (index > pRect->right)
							{
							}
							else
							{
								memcpyShadow4444(pDestTemp, (pRect->right - index)>>shift);
							}

							j = count;
							
							break;
						}

						
						colorCount >>= shift;
					
						memcpyShadow4444(pDestTemp, colorCount);

						pDestTemp += colorCount;
						//pPixels += colorCount;
						index += colorCount;

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{		
						dist = pRect->left - index;

						
						if (index+colorCount > pRect->right)
						{						
							memcpyShadow4444(pDestTemp, (pRect->right - pRect->left)>>shift);

							j = count;						
							break;
						}

						
						index += colorCount;
						colorCount = (colorCount-dist) >> shift;

						
						//memcpy(pDestTemp, pPixels+dist, (colorCount-dist)<<1);					
						
						memcpyShadow4444(pDestTemp, colorCount);

						pDestTemp += colorCount;
						//pPixels += colorCount;		
						

						
						break;
					}
				}					

				
				//pPixels += colorCount;
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
					if (index+colorCount > pRect->right)
					{
						
						if (index > pRect->right)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount>>shift;
						
							
							//memcpy(pDestTemp, pPixels, (pRect->right - index)<<1);
							
							memcpyShadow4444(pDestTemp, (pRect->right - index)>>shift);

							break;
						}
					}

					
					pDestTemp += transCount>>shift;


					index += colorCount;
					
					colorCount >>= shift;

					
					//memcpy(pDestTemp, pPixels, colorCount<<1);
					
					memcpyShadow4444(pDestTemp, colorCount);

					pDestTemp += colorCount;
					//pPixels += colorCount;								
				} while (--j);
			}
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// BltSmall4444 Clip Height
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CShadowSprite::BltSmall4444ClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD pixel, BYTE shift)
{
	
	s_wValue1 = pixel;

	int		count,			
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int i;
	register int j;
	
	int stepY = 1 << shift;		
	int endY = pRect->bottom << shift;
	for (int i=pRect->top; i<endY; i+=stepY)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		if (count > 0)
		{			
			j = count;
			do
			{
				pDestTemp += ((*pPixels++)>>shift);		
				colorCount = *pPixels++;				
				
				colorCount >>= shift;

				
				memcpyShadow4444(pDestTemp, colorCount);
		
				pDestTemp	+= colorCount;
				//pPixels		+= colorCount;
			} while (--j);
		}
		

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

