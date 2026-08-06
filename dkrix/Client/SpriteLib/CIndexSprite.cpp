//----------------------------------------------------------------------
// CIndexSprite.cpp
//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "CSprite.h"
#include "CSpriteSurface.h"
#include "CIndexSprite.h"

#ifdef SPRITELIB_BACKEND_SDL
#include "SpriteLibBackend.h"
#endif

//-----------------------------------------------------------------------------
// Static Member
//-----------------------------------------------------------------------------
WORD	CIndexSprite::ColorSet[MAX_COLORSET][MAX_COLORGRADATION];
WORD	CIndexSprite::GradationValue[MAX_COLORGRADATION];
WORD	CIndexSprite::ColorSetDarkness[MAX_DARKBIT][MAX_COLORSET][MAX_COLORGRADATION];
BYTE	CIndexSprite::ColorToGradation[MAX_COLOR_TO_GRADATION];

//----------------------------------------------------------------------
// Static member
//----------------------------------------------------------------------
WORD	CIndexSprite::s_Colorkey = 0;
int		CIndexSprite::s_IndexValue[MAX_COLORSET_USE];

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
const int defaultGradationColor = 384;

//-----------------------------------------------------------------------------
// memcpy Effect
//-----------------------------------------------------------------------------
FUNCTION_MEMCPYEFFECT 	CIndexSprite::s_pMemcpyEffectFunction = memcpyEffectDarker;
FUNCTION_MEMCPYEFFECT	CIndexSprite::s_pMemcpyEffectFunctionTable[MAX_EFFECT] =
{
	memcpyEffectDarker,
	memcpyEffectGrayScale,
	memcpyEffectLighten,
	memcpyEffectDarken,
	memcpyEffectColorDodge,
	memcpyEffectScreen,
	memcpyEffectDodgeBurn,
	memcpyEffectDifferent,
	memcpyEffectGradation,
	memcpyEffectSimpleOutline,
	memcpyEffectWipeOut,
	memcpyEffectNet,
	memcpyEffectGrayScaleVarious
};

//-----------------------------------------------------------------------------
//
// Static Functions
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Set ColorSet
//-----------------------------------------------------------------------------
void
CIndexSprite::SetColorSet()
{
	int i,j,k, set;
	WORD color;


	 

	static BYTE rgbPoint[MAX_COLORSET_SEED][3] =
	{
		// RGB565 format: G should be 2x R/B for neutral colors (G has 6 bits, R/B have 5 bits)
		{ 0, 0, 31 },
		{ 0, 63, 0 },	// Pure green - G is 6-bit (0-63)
		{ 31, 0, 0 },
		{ 0, 63, 63 },
		{ 31, 0, 31 },
		{ 31, 63, 0 },

		{ 0, 0, 16 },
		{ 0, 32, 0 },	// Dark green - G is 6-bit (0-63)
		{ 16, 0, 0 },
		{ 0, 32, 32 },
		{ 16, 0, 16 },
		{ 16, 32, 0 },

		{ 16, 63, 0 },
		{ 16, 0, 31 },
		{ 31, 32, 0 },
		{ 0, 32, 31 },
		{ 31, 0, 16 },
		{ 0, 63, 16 },

		{ 16, 63, 16 },
		{ 16, 32, 31 },
		{ 31, 32, 16 },

		{ 16, 63, 31 },
		{ 31, 32, 31 },
		{ 31, 63, 16 },

		{ 16, 32, 16 }, 
		{ 24, 48, 24 }, 
		{ 8, 16, 8 },   

		{ 30, 48, 18 }, 
		{ 25, 30, 11 },	
		{ 21, 24, 11 },				// G is 6-bit
		{ 19, 30, 13 }, 

		{ 21, 36, 11 }, 

		{ 22, 32, 9 } 
	};

		

	//----------------------------------------------------------------------
	// ColorIndex Table 
	//----------------------------------------------------------------------
	set = 0;
	int r,g,b;

	for (int i=0; i<MAX_COLORSET_SEED; i++)
	{
		r = rgbPoint[i][0];
		g = rgbPoint[i][1];
		b = rgbPoint[i][2];
		
		// MAX_COLORGRADATION_HALF ~ 1
		for (j=MAX_COLORGRADATION_HALF; j>=1; j--)
		{
			
			if (j==MAX_COLORGRADATION_HALF)
			{
				GetIndexColor(ColorSet[set], j,
							31, 63, 31,  // RGB565 white: G is 6-bit (0-63)
							r,g,b);
			}
			else
			{
				WORD color = ColorSet[i*MAX_COLORSET_SEED_MODIFY][MAX_COLORGRADATION_HALF-j];
				int r0 = ColorDraw::Red( color );
				int g0 = ColorDraw::Green( color );
				int b0 = ColorDraw::Blue( color );

				GetIndexColor(ColorSet[set], j, 
							r0, g0, b0, 
							r,g,b);
			}

			GetIndexColor(ColorSet[set]+j, MAX_COLORGRADATION-j, 
						r,g,b,
						0, 0, 0);

			set++;
		}
		/*
		// White ~ rgb ~ Black
		// White ~ rgb
		//GetIndexColor(ColorSet[set], MAX_COLORGRADATION, 
		//				31, 31, 31, 
		//				r,g,b);
		GetIndexColor(ColorSet[set], MAX_COLORGRADATION_HALF,
						31, 63, 31,  // RGB565 white
						r,g,b);

		GetIndexColor(ColorSet[set]+MAX_COLORGRADATION_HALF, MAX_COLORGRADATION_HALF,
						r,g,b,
						0, 0, 0);

		set++;

		// White/2 ~ rgb ~~~ Black
		GetIndexColor(ColorSet[set], MAX_COLORGRADATION_1_DIV_3,
						(31+r)/2, (63+g)/2, (31+b)/2,  // RGB565 white/2: G is 6-bit
						r,g,b);

		GetIndexColor(ColorSet[set]+MAX_COLORGRADATION_1_DIV_3, MAX_COLORGRADATION_2_DIV_3, 
						r,g,b,
						0, 0, 0);		
		set++;		
		
		// rgb ~ Black		
		GetIndexColor(ColorSet[set], MAX_COLORGRADATION, 
						r,g,b,
						0, 0, 0);

		set++;
		*/
	}
	
	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	for (j=0; j<MAX_COLORGRADATION; j++)
	{
		color = ColorSet[0][j];
		GradationValue[j] = ColorDraw::Red(color) + ColorDraw::Green(color) + ColorDraw::Blue(color);
	}

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	for (int i=0; i<MAX_COLORSET; i++)
	{
		for (j=0; j<MAX_COLORGRADATION; j++)
		{
			color = ColorSet[i][j];
			//GradationValue[j] = ColorDraw::Red(color) + ColorDraw::Green(color) + ColorDraw::Blue(color);

			
			for (k=0; k<MAX_DARKBIT; k++)
			{	
				r = ((color >> ColorDraw::s_bSHIFT_R) >> k) << ColorDraw::s_bSHIFT_R;
				g = (((color >> ColorDraw::s_bSHIFT_G) & 0x1F) >> k) << ColorDraw::s_bSHIFT_G;
				b = (color & 0x1F) >> k;
				ColorSetDarkness[k][i][j] = r | g | b;
			}
		}
	}

	//----------------------------------------------------------------------
	// Color to Gradation
	//----------------------------------------------------------------------
	for (BYTE cg=0; cg<MAX_COLOR_TO_GRADATION; cg++)
	{
		ColorToGradation[cg] = GetColorToGradation(cg);
	}

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	/*
	int colorSet = MAX_COLORSET-1;

	int css[] = { 375, 44 };

	for (cg=0; cg<MAX_COLOR_TO_GRADATION; cg++)
	{
		ColorSet[colorSet][cg] = ColorSet[css[cg%2]][cg];
	}
	*/
}

//----------------------------------------------------------------------
// Save IndexTable To File
//----------------------------------------------------------------------
BOOL
CIndexSprite::SaveIndexTableToFile(std::ofstream& file)
{
	int i,j,k;

	int cg = MAX_COLORGRADATION;
	int cs = MAX_COLORSET;
	int db = MAX_DARKBIT;

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	file.write((const char*)&cg, 4);
	file.write((const char*)&cs, 4);
	file.write((const char*)&db, 4);

	//----------------------------------------------------------------------
	// WORD	CIndexSprite::ColorSet[MAX_COLORSET][MAX_COLORGRADATION];
	//----------------------------------------------------------------------
	for (int i=0; i<MAX_COLORSET; i++)
	{
		for (j=0; j<MAX_COLORGRADATION; j++)
		{
			file.write((const char*)&ColorSet[i][j], 2);
		}
	}

	//----------------------------------------------------------------------
	//	WORD	CIndexSprite::GradationValue[MAX_COLORGRADATION];
	//----------------------------------------------------------------------
	for (int i=0; i<MAX_COLORSET; i++)
	{
		file.write((const char*)&GradationValue[i], 2);
	}

	//----------------------------------------------------------------------
	// WORD	CIndexSprite::ColorSetDarkness[MAX_DARKBIT][MAX_COLORSET][MAX_COLORGRADATION];
	//----------------------------------------------------------------------
	for (k=0; k<MAX_DARKBIT; k++)
	{
		for (int i=0; i<MAX_COLORSET; i++)
		{
			for (j=0; j<MAX_COLORGRADATION; j++)
			{
				file.write((const char*)&ColorSetDarkness[k][i][j], 2);
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------
// Load IndexTable From File
//----------------------------------------------------------------------
BOOL
CIndexSprite::LoadIndexTableFromFile(std::ifstream& file)
{
	int i,j,k;

	int cg, cs, db;

	//----------------------------------------------------------------------
	
	//----------------------------------------------------------------------
	file.read((char*)&cg, 4);
	file.read((char*)&cs, 4);
	file.read((char*)&db, 4);

	if (cg != MAX_COLORGRADATION ||
		cs != MAX_COLORSET ||
		db != MAX_DARKBIT)
	{
		//return FALSE;
	}

	//----------------------------------------------------------------------
	// WORD	CIndexSprite::ColorSet[MAX_COLORSET][MAX_COLORGRADATION];
	//----------------------------------------------------------------------
	for (int i=0; i<MAX_COLORSET; i++)
	{
		for (j=0; j<MAX_COLORGRADATION; j++)
		{
			file.read((char*)&ColorSet[i][j], 2);
		}
	}

	//----------------------------------------------------------------------
	//	WORD	CIndexSprite::GradationValue[MAX_COLORGRADATION];
	//----------------------------------------------------------------------
	for (int i=0; i<MAX_COLORSET; i++)
	{
		file.read((char*)&GradationValue[i], 2);
	}

	//----------------------------------------------------------------------
	// WORD	CIndexSprite::ColorSetDarkness[MAX_DARKBIT][MAX_COLORSET][MAX_COLORGRADATION];
	//----------------------------------------------------------------------
	for (k=0; k<MAX_DARKBIT; k++)
	{
		for (int i=0; i<MAX_COLORSET; i++)
		{
			for (j=0; j<MAX_COLORGRADATION; j++)
			{
				file.read((char*)&ColorSetDarkness[k][i][j], 2);
			}
		}
	}

	return TRUE;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void	
CIndexSprite::SetUsingColorSet(int set1, int set2)
{
	s_IndexValue[0] = set1;
	s_IndexValue[1] = set2;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
CIndexSprite::SetUsingColorSetOnly(BYTE setNumber, int colorSet)
{
	s_IndexValue[setNumber] = colorSet;
}


//----------------------------------------------------------------------


//----------------------------------------------------------------------
void
CIndexSprite::GetIndexColor(WORD* pColor, int step, 
							int r0, int g0, int b0, 
							int r1, int g1, int b1)
{
	float r = (float)r0;
	float g = (float)g0;
	float b = (float)b0;

	float step_1 = (float)step - 1.0f;
	float sr = (r1-r0) / (float)step_1;
	float sg = (g1-g0) / (float)step_1;
	float sb = (b1-b0) / (float)step_1;

	BYTE red, green, blue;

	for (int i=0; i<step; i++)
	{
		red		= (BYTE)r;
		green	= (BYTE)g;
		blue	= (BYTE)b;
		
		*pColor++ = ColorDraw::Color(red, green, blue);		
		
		r += sr;
		g += sg;
		b += sb;
	}
}


//----------------------------------------------------------------------
//
// constructor/destructore
//
//----------------------------------------------------------------------

CIndexSprite::CIndexSprite()
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

CIndexSprite::~CIndexSprite()
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
CIndexSprite::Release()
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
// Assign Operator
//----------------------------------------------------------------------
void
CIndexSprite::operator = (const CIndexSprite& Sprite)
{
	
	Release();

	
	
	if (Sprite.m_Pixels==NULL || Sprite.m_Width==0 || Sprite.m_Height==0)
		return;

	
	m_Width = Sprite.m_Width;
	m_Height = Sprite.m_Height;
	
	
	int index;	
	register int i;
	register int j;

	int colorCount, transPair, indexCount;

	
	m_Pixels = new WORD* [m_Height];

	//--------------------------------
	// 5:6:5
	//--------------------------------
	for (int i=0; i<m_Height; i++)
	{
		
		transPair = Sprite.m_Pixels[i][0];
				
		index	= 1;

		
		for (j=0; j<transPair; j++)
		{			
			// transCount = m_Pixels[i][index];
			index++;	
			indexCount = Sprite.m_Pixels[i][index++];	

			index += indexCount;		

			
			colorCount = Sprite.m_Pixels[i][index++];

			
			index	+= colorCount;
		}

		
		m_Pixels[i] = new WORD [index];
		memcpy(m_Pixels[i], Sprite.m_Pixels[i], index<<1);
	}

	
	m_bInit = true;
}


//-----------------------------------------------------------------------------
// Get IndexColor
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BYTE
CIndexSprite::GetIndexColor(WORD color)
{
	//-------------------------------------------------------
	
	//-------------------------------------------------------
	WORD spriteGradation = ColorDraw::Red(color) 
						+ ColorDraw::Green(color) 
						+ ColorDraw::Blue(color);
			
	return GetColorToGradation( spriteGradation );
}

//-----------------------------------------------------------------------------
// GetColorToGradation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
BYTE
CIndexSprite::GetColorToGradation(BYTE spriteGradation)
{
	
	//WORD spriteGradation = (color >> 11) + ((color >> 6) & 0x1F) + (color & 0x1F);
	
	//WORD spriteGradation = (color >> 10) + ((color >> 5) & 0x1F) + (color & 0x1F);

	//-------------------------------------------------------
	
	
	//-------------------------------------------------------
	int g;
	for (g=0; g<MAX_COLORGRADATION; g++)
	{
		if (spriteGradation > GradationValue[g])
		{
			break;
		}
	}

	
	if (g==0 || g==MAX_COLORGRADATION-1)
	{
		return g;
	}

	
	WORD value1 = GradationValue[g-1] - spriteGradation;
	WORD value2 = spriteGradation - GradationValue[g-1];

	
	if (value1 < value2)
	{
		return g-1;
	}
	else if (value1 > value2)
	{
		return g;
	}

	
	return g-1;
}

//-----------------------------------------------------------------------------
// Set Pixel
//-----------------------------------------------------------------------------



//


//


//-----------------------------------------------------------------------------
void		
CIndexSprite::SetPixel(WORD* pSource, WORD sourcePitch, 
							 WORD* pIndex1, WORD indexPitch1,	
							 WORD* pIndex2, WORD indexPitch2,	
							 WORD width, WORD height)
{
	//--------------------------------------------------
	
	//--------------------------------------------------
	if (width==0 || height==0)
	{
		Release();
		return;
	}

	WORD	*pSourceTemp, *pIndexTemp1, *pIndexTemp2;
	WORD	transIndex1=0, transIndex2=0;	

	//--------------------------------------------------
	
	//--------------------------------------------------
	
	
	//--------------------------------------------------
	WORD	*pSourceTemp2;
	pSourceTemp = pSource;

	for (register int i=0; i<height; i++)
	{
		pSourceTemp2 = pSourceTemp;

		for (register int j=0; j<width; j++)
		{
			
			if (*pSourceTemp2==s_Colorkey)
			{
				
				if (pIndex1!=NULL)
				{
					transIndex1 = *(WORD*)((BYTE*)pIndex1 + i*indexPitch1 + (j<<1));
				}

				if (pIndex2!=NULL)
				{
					transIndex2 = *(WORD*)((BYTE*)pIndex2 + i*indexPitch2 + (j<<1));
				}

				i = height;
				break;
			}

			pSourceTemp2++;
		}

		pSourceTemp = (WORD*)((BYTE*)pSourceTemp + sourcePitch);
	}
	

	//--------------------------------------------------
	
	//--------------------------------------------------
	WORD** ppColor, **ppIndex;

	ppColor = new WORD* [height];
	ppIndex = new WORD* [height];

	//--------------------------------------------------
	
	//--------------------------------------------------
	for (int i=0; i<height; i++)
	{
		//--------------------------------------------------
		
		//--------------------------------------------------
		ppColor[i] = new WORD [width];
		ppIndex[i] = new WORD [width];

		//--------------------------------------------------
		
		//--------------------------------------------------
		pSourceTemp = pSource;

		if (pIndex1!=NULL)
		{
			pIndexTemp1 = pIndex1;
		}

		if (pIndex2!=NULL)
		{
			pIndexTemp2 = pIndex2;
		}

		//--------------------------------------------------
		
		//--------------------------------------------------
		for (register int j=0; j<width; j++)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			if (*pSourceTemp==s_Colorkey)
			{
				ppIndex[i][j] = INDEX_TRANS;
			}
			//--------------------------------------------------
			
			
			//--------------------------------------------------
			else
			{
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (pIndex1!=NULL && *pIndexTemp1!=transIndex1)
				{
					ppIndex[i][j] = GetIndexColor( *pSourceTemp );	// (0<<8) | gradation;
				}
				//--------------------------------------------------
				
				//--------------------------------------------------
				else if (pIndex2!=NULL && *pIndexTemp2!=transIndex2)
				{
					ppIndex[i][j] = (1<<8) | GetIndexColor( *pSourceTemp );
				}
				//--------------------------------------------------
				
				//--------------------------------------------------
				else
				{
					ppIndex[i][j] = INDEX_NULL;

					
					ppColor[i][j] = *pSourceTemp;
				}				
			}

			pSourceTemp++;

			//--------------------------------------------------
			
			//--------------------------------------------------
			if (pIndex1!=NULL)
			{
				pIndexTemp1++;
			}

			if (pIndex2!=NULL)
			{
				pIndexTemp2++;
			}
		}

		//--------------------------------------------------
		
		//--------------------------------------------------
		pSource = (WORD*)((BYTE*)pSource + sourcePitch);

		if (pIndex1!=NULL)
		{
			pIndex1 = (WORD*)((BYTE*)pIndex1 + indexPitch1);
		}

		if (pIndex2!=NULL)
		{
			pIndex2 = (WORD*)((BYTE*)pIndex2 + indexPitch2);
		}
	}


	//--------------------------------------------------
	
	
	//--------------------------------------------------
	GenerateFromIndex(ppColor, width, height, ppIndex);


	//--------------------------------------------------
	
	//--------------------------------------------------
	for (int i=0; i<height; i++)
	{
		delete [] ppColor[i];
		delete [] ppIndex[i];
	}
	
	delete [] ppColor;
	delete [] ppIndex;
}


//-----------------------------------------------------------------------------
// Generate Index 2
//-----------------------------------------------------------------------------
// 



//
//-----------------------------------------------------------------------------


//
//

//




//                              .     
//                              .     
//                              .     



//                              .     
//                              .     
//                              .     

//    )
//






//

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
void			
CIndexSprite::GenerateFromIndex(WORD** ppColor,
								WORD width, WORD height,
								WORD** ppIndex)
{
	
	Release();

	m_Width = width;
	m_Height = height;

	
	WORD*	data = new WORD[m_Width*8+10];

	int	index;				
			
	int	transPairCount,					
			transColorCount,				
			indexColorCount,				
			normalColorCount;				

	int	indexColorCountPosition,		
			normalColorCountPosition;		

	enum	COLORTYPE { COLORTYPE_TRANS, COLORTYPE_INDEX, COLORTYPE_NORMAL }
			lastColorType;			
								

	
	m_Pixels = new WORD* [height];


	//-----------------------------------
	// test code
	//-----------------------------------
	//int* m_pLineIndex = new int [height];


	for (register int  i=0; i<height; i++)
	{
		index = 0;
		transPairCount = 0;
		transColorCount = 0; 		
		lastColorType = COLORTYPE_TRANS;

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		for (register int  j=0; j<width; j++)
		{
			//--------------------------------------------------
			
			//--------------------------------------------------
			if ((ppIndex[i][j] & 0xFF)==INDEX_TRANS)
			{
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (lastColorType == COLORTYPE_INDEX)
				{
					
					data[indexColorCountPosition] = indexColorCount;

					
					
					transPairCount++;
					
					data[index++] = 0;					

					transColorCount = 0;
					lastColorType = COLORTYPE_TRANS;
				}
				//--------------------------------------------------
				
				//--------------------------------------------------
				else if (lastColorType == COLORTYPE_NORMAL)
				{
					
					transPairCount++;
					
					data[normalColorCountPosition] = normalColorCount;
					normalColorCount = 0;

					transColorCount = 0;
					lastColorType = COLORTYPE_TRANS;
				}

				
				transColorCount++;				
			}		
			//--------------------------------------------------				
			
			//--------------------------------------------------
			else if ((ppIndex[i][j] & 0xFF)!=INDEX_NULL)
			{
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (lastColorType == COLORTYPE_TRANS)
				{						
					data[index++] = transColorCount;		
					transColorCount = 0;
					
					
					
					indexColorCountPosition = index++;
					indexColorCount	= 0;
					
					lastColorType = COLORTYPE_INDEX;
				}
				//--------------------------------------------------
				
				//--------------------------------------------------
				else if (lastColorType == COLORTYPE_NORMAL)
				{
					
					
					transPairCount++;
					
					data[normalColorCountPosition] = normalColorCount;
					normalColorCount = 0;

					
					data[index++] = 0;

					
					indexColorCountPosition = index++;
					indexColorCount	= 0;					

					lastColorType = COLORTYPE_INDEX;						
				}	
				
				
				indexColorCount++;

				
				data[index++]	= ppIndex[i][j];
			}
			//--------------------------------------------------				
			
			//--------------------------------------------------
			else //if ((ppIndex[i][j] & 0xFF)==INDEX_NULL)
			{	
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (lastColorType == COLORTYPE_TRANS)
				{
					data[index++] = transColorCount;
					transColorCount = 0;
						
					
					data[index++] = 0;	
						
					
					normalColorCountPosition = index++;
					normalColorCount = 0;

					lastColorType = COLORTYPE_NORMAL;	
				}
				else
				//--------------------------------------------------
				
				//--------------------------------------------------
				if (lastColorType == COLORTYPE_INDEX)
				{
					
					data[indexColorCountPosition] = indexColorCount;
						
					
					normalColorCountPosition = index++;
					normalColorCount = 0;

					lastColorType = COLORTYPE_NORMAL;						
				}

				data[index++] = ppColor[i][j];	
				normalColorCount++;
			}
		}
		
		//--------------------------------------------------
		
		//--------------------------------------------------
		if (lastColorType == COLORTYPE_TRANS)
		{
			
		}	
		//--------------------------------------------------
		
		//--------------------------------------------------
		else if (lastColorType == COLORTYPE_INDEX)
		{		
			
			data[indexColorCountPosition] = indexColorCount;

			
			data[index++] = 0;

			
			transPairCount++;			
		}
		//--------------------------------------------------
		
		//--------------------------------------------------
		else 
		{
			
			data[normalColorCountPosition] = normalColorCount;	

			transPairCount++;		
		}

		
		m_Pixels[i] = new WORD [index+1];

		
		
		m_Pixels[i][0] = transPairCount;
		memcpy(m_Pixels[i]+1, data, index<<1);

		//m_pLineIndex[i] = index+1;
	}

	delete [] data;

	m_bInit = true;

	/*
	ofstream indexFile("index.txt");
	ofstream colorFile("color.txt");
	ofstream spriteFile("sprite.txt");

	for (int i=0; i<m_Height; i++)
	{
		for (int j=0; j<m_Width; j++)
		{
			indexFile << (int)ppIndex[i][j] << " ";
			colorFile << (int)ppColor[i][j] << " ";
		}
		indexFile << endl;
		colorFile << endl;

		for (j=0; j<m_pLineIndex[i]; j++)
		{
			spriteFile << (int)m_Pixels[i][j] << " ";
		}

		spriteFile << endl;
	}
	delete [] m_pLineIndex;
	*/
}



//-----------------------------------------------------------------------------
// Generate Index (Old version)
//-----------------------------------------------------------------------------
// 



//
//-----------------------------------------------------------------------------


//
//

//




//                              .     
//                              .     
//                              .     




//                              .     
//                              .     
//                              .     

//    )
//






//

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
 


//-----------------------------------------------------------------------------
// Get IndexInfo
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void			
CIndexSprite::GetIndexInfo(WORD**& ppIndex)
{
	int	transPair,	
			indexCount,			
			colorCount;

	int	colorSet,
			colorGradation;

	WORD	*pDest,
			*pPixels;

	register int i;
	register int j;
	register int k;
	register int l;

	for (int i=0; i<m_Height; i++)
	{			
		pPixels	= m_Pixels[i];
		pDest	= ppIndex[i];

		
		transPair	= *pPixels++;
		
 		
		if (transPair > 0)
		{
			j = transPair;
			do {		
				pDest += *pPixels++;	
				indexCount = *pPixels++;		

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {			
						colorSet		= *pPixels >> 8;
						colorSet		&= 0xFF;				// set
						colorGradation	= *pPixels & 0xFF;		// gradation
						pPixels++;

						// (colorSet<<8) | colorGradation
						*pDest		= (colorSet << 8) | colorGradation;
						pDest++;
					} while (--k);
				}

				
				colorCount = *pPixels++;
				
				
				for (l=0; l<colorCount; l++)
				{					
					*pDest++ = (INDEXSET_NULL<<8) | INDEX_NULL;
				}
				
				pPixels		+= colorCount;
			} while (--j);
		}
	}
}

//----------------------------------------------------------------------
// Is ColorPixel ?
//----------------------------------------------------------------------

//----------------------------------------------------------------------
bool		
CIndexSprite::IsColorPixel(short x, short y)
{
	
	if (m_bInit)
	{	
		
		if (x<0 || y<0 || x>=m_Width || y>=m_Height)
			return false;

		
		WORD	*pPixels = m_Pixels[y];

		
		int	count = *pPixels++;

		int	index = 0;

		
		
		

		register int i;

		if (count > 0)
		{
			i = count;
			do {
				index += *pPixels++;

				
				if (x < index)
				{
					return false;
				}

				
				index += *pPixels;

				
				pPixels += *pPixels;

				pPixels++;


				
				index += *pPixels;

				
				pPixels += *pPixels;

				pPixels++;


				
				if (x < index)
				{
					return true;
				}
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
CIndexSprite::GetPixel(int x, int y) const
{
	
	if (m_bInit)
	{	
		
		if (x<0 || y<0 || x>=m_Width || y>=m_Height)
			return 0;

		
		WORD	*pPixels = m_Pixels[y];

		
		int	count = *pPixels++;

		int	index = 0;

		
		
		

		register int i;

		if (count > 0)
		{
			i = count;
			do {
				index += *pPixels++;

				
				if (x < index)
				{
					return 0;
				}

				
				index += *pPixels;

				
				pPixels += *pPixels;

				pPixels++;


				
				index += *pPixels;

				
				pPixels += *pPixels;

				pPixels++;


				
				if (x < index)
				{
					// [ TEST CODE ]
					
					
					return pPixels[x-index];
				}
			} while (--i);
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Blt Index
//-----------------------------------------------------------------------------
// s_Value1 - Main Color
// s_Value2 - Sub Color
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CIndexSprite::Blt(WORD *pDest, WORD pitch)
{
	int	transPair,	
			indexCount,			
			colorCount;

	int	colorSet,
			colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	register int k;
	int rectBottom = m_Height;	

	
	//for (int i=0; i<rectBottom; i++)	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{			
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do
				{
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					
					if (indexCount > 0)
					{
						k = indexCount;
						do
						{
							colorSet		= (*pPixels >> 8) & 0xFF;						
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;
							
							
							*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
					
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
// s_Value1 - Main Color
// s_Value2 - Sub Color
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------


//----------------------------------------------------------------------
void
CIndexSprite::BltClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;
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
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do
							{
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do
							{
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					memcpy(pDestTemp, pPixels+dist, colorCount2<<1);					
					pDestTemp += colorCount2;
					pPixels += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do
						{				
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;		
				
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
CIndexSprite::BltClipRight(WORD* pDest, WORD pitch, RECT* pRect)
{	
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	register int	k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}						
						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					k = indexCount;
					do {				
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
						pDestTemp ++;
					} while (--k);
				}		
				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					memcpy(pDestTemp, pPixels, (rectRight - index)<<1);
					break;			
				}			
				
				//--------------------
				
				//--------------------
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
// s_Value1 - Main Color
// s_Value2 - Sub Color
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------



//----------------------------------------------------------------------
void
CIndexSprite::BltClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {		
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {				
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						memcpy((void*)pDestTemp, (void*)pPixels, colorCount<<1);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						index += colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					memcpy(pDestTemp, pPixels+dist, colorCount2<<1);					
					pDestTemp += colorCount2;
					pPixels += colorCount;
					index += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)		
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								k = indexCount;
								do {
									colorSet		= (*pPixels >> 8) & 0xFF;	// set
									colorGradation	= (*pPixels & 0xFF);			// gradation
									pPixels++;

									
									*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
									pDestTemp ++;
								} while (--k);
							}						
							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}		
					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						memcpy(pDestTemp, pPixels, (rectRight - index)<<1);
						break;			
					}			
					
					//--------------------
					
					//--------------------
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
CIndexSprite::BltClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
	register int  k;

	int colorSet, colorGradation;
	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {		
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
						pDestTemp ++;
					} while (--k);
				}

				
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
// Blt Darkness
//----------------------------------------------------------------------
void		
CIndexSprite::BltDarkness(WORD *pDest, WORD pitch, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	int	transPair,	
			indexCount,			
			colorCount;

	int	colorSet,
			colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	register int k;
	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}

					
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
// Blt Darkness Clip Left
//----------------------------------------------------------------------
void		
CIndexSprite::BltDarknessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {		
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {						
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------	
			if (--j > 0)
			{
				do {		
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {			
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;		
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Darkness Clip Right
//----------------------------------------------------------------------
void		
CIndexSprite::BltDarknessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	register int	k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}						
						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
						pDestTemp ++;
					} while (--k);
				}		
				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight - index);
					break;			
				}			
				
				//--------------------
				
				//--------------------
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
// Blt Darkness Clip Width
//----------------------------------------------------------------------
void		
CIndexSprite::BltDarknessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						index += colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyDarkness(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;
					index += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								k = indexCount;
								do {
									colorSet		= (*pPixels >> 8) & 0xFF;	// set
									colorGradation	= (*pPixels & 0xFF);			// gradation
									pPixels++;

									
									*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
									pDestTemp ++;
								} while (--k);
							}						
							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}
					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						CSpriteSurface::memcpyDarkness(pDestTemp, pPixels, rectRight - index);
						break;			
					}			
					
					//--------------------
					
					//--------------------
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
// Blt Darkness Clip Height
//----------------------------------------------------------------------
void		
CIndexSprite::BltDarknessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
	register int  k;

	int colorSet, colorGradation;
	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSetDarkness[DarkBits][s_IndexValue[colorSet]][colorGradation];
						pDestTemp ++;
					} while (--k);
				}

				
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
// Blt Darkness
//----------------------------------------------------------------------
void		
CIndexSprite::BltAlpha(WORD *pDest, WORD pitch, BYTE alpha)
{
	CSpriteSurface::s_Value1 = alpha;

	int	transPair,	
			indexCount,			
			colorCount;

	int	colorSet,
			colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	register int k;
	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do {				
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
							pDestTemp ++;
						} while (--k);
					}

					
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
// Blt Darkness Clip Left
//----------------------------------------------------------------------
void		
CIndexSprite::BltAlphaClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	CSpriteSurface::s_Value1 = alpha;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);		

			//---------------------------------------------
			
			//---------------------------------------------		
			if (--j > 0)
			{
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;		
				} while (--j);
			}
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Darkness Clip Right
//----------------------------------------------------------------------
void		
CIndexSprite::BltAlphaClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	CSpriteSurface::s_Value1 = alpha;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	register int	k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
								pDestTemp ++;
							} while (--k);
						}						
						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
						pDestTemp ++;
					} while (--k);
				}		
				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, rectRight - index);
					break;			
				}			
				
				//--------------------
				
				//--------------------
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
// Blt Darkness Clip Width
//----------------------------------------------------------------------
void		
CIndexSprite::BltAlphaClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	CSpriteSurface::s_Value1 = alpha;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						index += colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyAlpha(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;
					index += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		
			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								k = indexCount;
								do {
									colorSet		= (*pPixels >> 8) & 0xFF;	// set
									colorGradation	= (*pPixels & 0xFF);			// gradation
									pPixels++;

									
									*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
									pDestTemp ++;
								} while (--k);
							}						
							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
							pDestTemp ++;
						} while (--k);
					}		
					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						CSpriteSurface::memcpyAlpha(pDestTemp, pPixels, rectRight - index);
						break;			
					}			
					
					//--------------------
					
					//--------------------
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
// Blt Darkness Clip Height
//----------------------------------------------------------------------
void		
CIndexSprite::BltAlphaClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE alpha)
{
	CSpriteSurface::s_Value1 = alpha;

	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
	register int  k;

	int colorSet, colorGradation;
	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {						
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= CSpriteSurface::memcpyAlpha1Pixel(*pDestTemp, ColorSet[s_IndexValue[colorSet]][colorGradation]);
						pDestTemp ++;
					} while (--k);
				}

				
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
// BltWave
//----------------------------------------------------------------------

//----------------------------------------------------------------------
 



//----------------------------------------------------------------------
// Blt Darkness
//----------------------------------------------------------------------
void		
CIndexSprite::BltColor(WORD *pDest, WORD pitch, BYTE rgb)
{
	CSpriteSurface::s_Value1 = rgb;

	int	transPair,	
			indexCount,			
			colorCount;

	int	colorSet,
			colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	register int k;
	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do 
						{
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
							pDestTemp ++;
						} while (--k);
					}

					
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
// Blt Darkness Clip Left
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	CSpriteSurface::s_Value1 = rgb;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {		
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {						
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------	
			if (--j > 0)
			{
				do {		
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {			
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;		
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Darkness Clip Right
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	register int	k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
								pDestTemp ++;
							} while (--k);
						}						
						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
						pDestTemp ++;
					} while (--k);
				}		
				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels, rectRight - index);
					break;			
				}			
				
				//--------------------
				
				//--------------------
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
// Blt Darkness Clip Width
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						index += colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyColor(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;
					index += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								k = indexCount;
								do {
									colorSet		= (*pPixels >> 8) & 0xFF;	// set
									colorGradation	= (*pPixels & 0xFF);			// gradation
									pPixels++;

									
									*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
									pDestTemp ++;
								} while (--k);
							}						
							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
							pDestTemp ++;
						} while (--k);
					}
					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						CSpriteSurface::memcpyColor(pDestTemp, pPixels, rectRight - index);
						break;			
					}			
					
					//--------------------
					
					//--------------------
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
// Blt Darkness Clip Height
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE rgb)
{
	
	CSpriteSurface::s_Value1 = rgb;

	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
	register int  k;

	int colorSet, colorGradation;
	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[defaultGradationColor][colorGradation]
												& ColorDraw::s_wMASK_RGB[rgb];
						pDestTemp ++;
					} while (--k);
				}

				
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
// Blt ColorSet
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorSet(WORD *pDest, WORD pitch, WORD colorSet)
{
	CSpriteSurface::s_Value1 = colorSet;

	int	transPair,	
			indexCount,			
			colorCount;

	int	//colorSet,
			colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	register int k;
	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do 
						{
							//colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
							pDestTemp ++;
						} while (--k);
					}

					
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
// Blt ColorSet Clip Left
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorSetClipLeft(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	CSpriteSurface::s_Value1 = colorSet;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int //colorSet, 
		colorGradation;

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
			do {		
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {						
								//colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								//colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------	
			if (--j > 0)
			{
				do {		
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {			
							//colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;		
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt ColorSet Clip Right
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorSetClipRight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	register int	k;

	int //colorSet, 
		colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								//colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
								pDestTemp ++;
							} while (--k);
						}						
						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						//colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
						pDestTemp ++;
					} while (--k);
				}		
				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, rectRight - index);
					break;			
				}			
				
				//--------------------
				
				//--------------------
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
// Blt ColorSet Clip Width
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorSetClipWidth(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int //colorSet, 
		colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								//colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								//colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						index += colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;
					index += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								k = indexCount;
								do {
									//colorSet		= (*pPixels >> 8) & 0xFF;	// set
									colorGradation	= (*pPixels & 0xFF);			// gradation
									pPixels++;

									
									*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
									pDestTemp ++;
								} while (--k);
							}						
							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							//colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
							pDestTemp ++;
						} while (--k);
					}
					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, rectRight - index);
						break;			
					}			
					
					//--------------------
					
					//--------------------
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
// Blt ColorSet Clip Height
//----------------------------------------------------------------------
void		
CIndexSprite::BltColorSetClipHeight(WORD *pDest, WORD pitch, RECT* pRect, WORD colorSet)
{
	
	CSpriteSurface::s_Value1 = colorSet;

	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
	register int  k;

	int //colorSet, 
		colorGradation;

	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						//colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[colorSet][colorGradation];
												//& ColorDraw::s_wMASK_RGB[colorSet];
						pDestTemp ++;
					} while (--k);
				}

				
				colorCount = *pPixels++;
				
				CSpriteSurface::memcpyEffectGradation(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}


//-----------------------------------------------------------------------------
// BltEffect Index
//-----------------------------------------------------------------------------
// s_Value1 - Main Color
// s_Value2 - Sub Color
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void
CIndexSprite::BltEffect(WORD *pDest, WORD pitch)
{
	int	transPair,	
			indexCount,			
			colorCount;

	//int	colorSet,
	//		colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	//register int k;
	int rectBottom = m_Height;	

	
	//for (int i=0; i<rectBottom; i++)	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{			
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do
				{
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					if (indexCount > 0)
					{
						
						memcpyEffect( pDestTemp, pPixels, indexCount );

						pDestTemp += indexCount;
						pPixels += indexCount;
					}

					
					colorCount = *pPixels++;
					
					if (colorCount > 0)
					{
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
					}
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// BltEffect ClipLeft
//----------------------------------------------------------------------
// s_Value1 - Main Color
// s_Value2 - Sub Color
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------


//----------------------------------------------------------------------
void
CIndexSprite::BltEffectClipLeft(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int		count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
//	register int k;

	//int colorSet, colorGradation;
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
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						if (indexCount > 0)
						{
							memcpyEffect( pDestTemp, pPixels, indexCount );

							pDestTemp += indexCount;
							pPixels += indexCount;
						}


						
						colorCount = *pPixels++;
						
						if (colorCount > 0)
						{
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
							
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;					
						}

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						if (indexCount > 0)
						{
							memcpyEffect( pDestTemp, pPixels, indexCount );

							pDestTemp += indexCount;
							pPixels += indexCount;
						}


						
						colorCount = *pPixels++;
						
						if (colorCount > 0)
						{
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
							
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;
						}

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					if (colorCount2 > 0)
					{
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels+dist, colorCount2);
						pDestTemp += colorCount2;
						pPixels += colorCount;
					}

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						memcpyEffect( pDestTemp, pPixels, indexCount );

						pDestTemp += indexCount;
						pPixels += indexCount;
					}

					
					colorCount = *pPixels++;
					
					if (colorCount > 0)
					{
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;		
					}
				
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
CIndexSprite::BltEffectClipRight(WORD* pDest, WORD pitch, RECT* pRect)
{	
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	//register int	k;

	//int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							memcpyEffect( pDestTemp, pPixels, indexCount );

							pDestTemp += indexCount;
							pPixels += indexCount;
						}

						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					memcpyEffect( pDestTemp, pPixels, indexCount );

					pDestTemp += indexCount;
					pPixels += indexCount;
				}

				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					int tempCount = rectRight - index;
					if (tempCount > 0)
					{
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, tempCount);
					}
					break;			
				}			
				
				//--------------------
				
				//--------------------
				if (colorCount > 0)
				{
					CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
				}

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
// s_Value1 - Main Color
// s_Value2 - Sub Color
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------



//----------------------------------------------------------------------
void
CIndexSprite::BltEffectClipWidth(WORD* pDest, WORD pitch, RECT* pRect)
{
	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
//	register int k;

	//int colorSet, colorGradation;

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
			do {		
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						if (indexCount > 0)
						{
							memcpyEffect( pDestTemp, pPixels, indexCount );

							pDestTemp += indexCount;
							pPixels += indexCount;
						}


						
						colorCount = *pPixels++;
						
						if (colorCount > 0)
						{
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
							
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;					
						}

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						if (indexCount > 0)
						{
							memcpyEffect( pDestTemp, pPixels, indexCount );

							pDestTemp += indexCount;
							pPixels += indexCount;
						}

						
						colorCount = *pPixels++;
						
						if (colorCount > 0)
						{
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
							
							pDestTemp	+= colorCount;
							pPixels		+= colorCount;
							index += colorCount;
						}

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					if (colorCount2 > 0)
					{
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels+dist, colorCount2);
						pDestTemp += colorCount2;
					}
					pPixels += colorCount;
					index += colorCount;					

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)		
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								memcpyEffect( pDestTemp, pPixels, indexCount );

								pDestTemp += indexCount;
								pPixels += indexCount;
							}

							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						memcpyEffect( pDestTemp, pPixels, indexCount );

						pDestTemp += indexCount;
						pPixels += indexCount;
					}

					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						int tempCount = rectRight - index;
						if (tempCount > 0)
						{
							CSpriteSurface::memcpyEffect(pDestTemp, pPixels, tempCount);
						}
						break;			
					}			
					
					//--------------------
					
					//--------------------
					if (colorCount > 0)
					{
						CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
					}

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
CIndexSprite::BltEffectClipHeight(WORD *pDest, WORD pitch, RECT* pRect)
{
	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
//	register int  k;

	//int colorSet, colorGradation;
	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {		
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					memcpyEffect( pDestTemp, pPixels, indexCount );

					pDestTemp += indexCount;
					pPixels += indexCount;
				}

				
				colorCount = *pPixels++;
				
				if (colorCount > 0)
				{
					CSpriteSurface::memcpyEffect(pDestTemp, pPixels, colorCount);
				}
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}


//----------------------------------------------------------------------
// memcpy EffectDarker
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectDarker(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectGrayScale
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectGrayScale(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectLighten
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectLighten(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectDarken
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectDarken(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectColorDodge
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectColorDodge(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectScreen
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectScreen(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectDodgeBurn
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectDodgeBurn(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectDifferent
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectDifferent(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectGradation
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectGradation(WORD* pDest, WORD* pSource, WORD pixels)
{
}

//----------------------------------------------------------------------
// memcpy EffectSimpleOutline
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectSimpleOutline(WORD* pDest, WORD* pSource, WORD pixels)
{
	int colorSet, colorGradation;

	
	colorSet		= (*pSource >> 8) & 0xFF;						
	colorGradation	= *pSource & 0xFF;			// gradation
	*pDest			= ColorSet[s_IndexValue[colorSet]][colorGradation];

	int pixels_1 = pixels-1;
	
	pDest += pixels_1;
	pSource += pixels_1;
	
	
	colorSet		= (*pSource >> 8) & 0xFF;						
	colorGradation	= *pSource & 0xFF;			// gradation
	//*pDest			= ColorSet[s_IndexValue[colorSet]][colorGradation];
}

//----------------------------------------------------------------------
// memcpy EffectWipeOut
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectWipeOut(WORD* pDest, WORD* pSource, WORD pixels)
{
	int colorSet, colorGradation;	

	int skipPixels = (pixels * CSpriteSurface::s_Value1) >> 6;	// / 64
	int drawPixels = (pixels - skipPixels)>>1;
	int drawPixels2 = pixels - drawPixels - skipPixels;
	
	
	
	
	
	//------------------------------------------------------------	
	
	//------------------------------------------------------------		
	if (drawPixels > 0)
	{
		do
		{
			colorSet		= (*pSource >> 8) & 0xFF;						
			colorGradation	= *pSource & 0xFF;			// gradation
			pSource++;

			
			*pDest		= ColorSet[s_IndexValue[colorSet]][colorGradation];
			pDest ++;
		} while (--drawPixels);
	}

	//------------------------------------------------------------	
	
	//------------------------------------------------------------	
	pDest += skipPixels;
	pSource += skipPixels;

	//------------------------------------------------------------	
	
	//------------------------------------------------------------	
	if (drawPixels2 > 0)
	{
		do
		{
			colorSet		= (*pSource >> 8) & 0xFF;						
			colorGradation	= *pSource & 0xFF;			// gradation
			pSource++;

			
			*pDest		= ColorSet[s_IndexValue[colorSet]][colorGradation];
			pDest ++;
		} while (--drawPixels2);
	}

	//pDest += drawPixels2;
	//pSource += drawPixels2;	
}

//----------------------------------------------------------------------
// memcpy EffectNet
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectNet(WORD* pDest, WORD* pSource, WORD pixels)
{
	
	register int i = pixels;
	int skipPixels = 1 + CSpriteSurface::s_Value1;

	int colorSet, colorGradation;	
	
	
	do
	{
		colorSet		= (*pSource >> 8) & 0xFF;						
		colorGradation	= *pSource & 0xFF;			// gradation
		
		
		*pDest		= ColorSet[s_IndexValue[colorSet]][colorGradation];

		pSource += skipPixels;
		pDest += skipPixels;

		i -= skipPixels;

	} while (i > 0);

}


//----------------------------------------------------------------------
// memcpy 
//----------------------------------------------------------------------
void		
CIndexSprite::memcpyEffectGrayScaleVarious(WORD* pDest, WORD* pSource, WORD pixels)
{
	register int i = pixels;
	int colorSet, colorGradation;	
	//int grayValue = s_Value1;

	//------------------------------------------------------------	
	// Gray Scale : color value = (R+G+B)/3
	//------------------------------------------------------------
	int average;
	int r, g, b;
	int color;
	
	while (i--)
	{
		colorSet		= (*pSource >> 8) & 0xFF;						
		colorGradation	= *pSource & 0xFF;			// gradation
		
		color = ColorSet[s_IndexValue[colorSet]][colorGradation];

		r = ColorDraw::Red(color);
		g = ColorDraw::Green(color);
		b = ColorDraw::Blue(color);

		average = ( r + g + b ) / 3;		// 0 ~ 31

		r = average + ((r-average)*CSpriteSurface::s_Value1 >> 5);
		g = average + ((g-average)*CSpriteSurface::s_Value1 >> 5);
		b = average + ((b-average)*CSpriteSurface::s_Value1 >> 5);
	
		*pDest = (r << ColorDraw::s_bSHIFT_R) 
				| (g << ColorDraw::s_bSHIFT_G)
				| b;				
		
		pDest++;
		pSource++;
	}
}



//----------------------------------------------------------------------
// Blt Brightness
//----------------------------------------------------------------------
void		
CIndexSprite::BltBrightness(WORD *pDest, WORD pitch, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	int	transPair,	
			indexCount,			
			colorCount;

	int	colorSet,
			colorGradation;

	WORD	*pDestTemp,
			*pPixels;

	register int i;
	register int j;
	register int k;
	int rectBottom = m_Height;
	
	if (rectBottom > 0)
	{
		i = rectBottom-1;
		pDest = (WORD*)((BYTE*)pDest + i*pitch);

		do
		{
			pPixels		= m_Pixels[i];
			pDestTemp	= pDest;

			
			transPair	= *pPixels++;
			
 			
			if (transPair > 0)
			{
				j = transPair;
				do {
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
						
					}

					
					colorCount = *pPixels++;
					
					CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);					
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;
				} while (--j);
			}

			pDest = (WORD*)((BYTE*)pDest - pitch);
		} while (i--);
	}
}

//----------------------------------------------------------------------
// Blt Brightness Clip Left
//----------------------------------------------------------------------
void		
CIndexSprite::BltBrightnessClipLeft(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {		
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {						
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyBrightness(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			//---------------------------------------------	
			if (--j > 0)
			{
				do {		
					pDestTemp += *pPixels++;	
					indexCount = *pPixels++;	

					
					if (indexCount > 0)
					{
						k = indexCount;
						do {			
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}

					
					colorCount = *pPixels++;
					
					CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);
					
					pDestTemp	+= colorCount;
					pPixels		+= colorCount;		
				} while (--j);
			}
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Brightness Clip Right
//----------------------------------------------------------------------
void		
CIndexSprite::BltBrightnessClipRight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			index;

	register int	i;
	register int	j;
	register int	k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;
				
				
				

				

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+indexCount > rectRight)
				{
					
					if (index > rectRight)
					{
						break;
					}
					
					else
					{
						pDestTemp += transCount;

						
						indexCount = rectRight - index;

						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}						
						
						break;
					}
				}

				
				pDestTemp += transCount;

				//-------------------------
				
				//-------------------------
				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
						pDestTemp ++;
					} while (--k);
				}		
				
				index += indexCount;
				

				
				colorCount = *pPixels++;

				//---------------------------------------------
				
				//---------------------------------------------			
				if (index+colorCount > rectRight)
				{
					
					CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, rectRight - index);
					break;			
				}			
				
				//--------------------
				
				//--------------------
				CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);

				pDestTemp += colorCount;
				pPixels += colorCount;			
				index += colorCount;
			} while (--j);
		}
		
		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}

//----------------------------------------------------------------------
// Blt Brightness Clip Width
//----------------------------------------------------------------------
void		
CIndexSprite::BltBrightnessClipWidth(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	WORD	*pPixels,
			*pDestTemp;

	//--------------------------------------------
	
	//--------------------------------------------
	int	count,
			transCount, 
			indexCount,
			colorCount,
			colorCount2,
			index,
			dist;

	register int i;
	register int j;
	register int k;

	int colorSet, colorGradation;

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
			do {
				transCount = *pPixels++;		
				indexCount = *pPixels++;		
						
				
				index += transCount;			
			
				//---------------------------------------------
				
				//---------------------------------------------
				if (index+indexCount > rectLeft)
				{
					//---------------------------------------------
					
					//---------------------------------------------
					if (index > rectLeft)
					{	
						
						pDestTemp += index - rectLeft;

						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;					

						
						break;
					}
					//---------------------------------------------
					
					
					//---------------------------------------------
					else
					{					
						dist = rectLeft - index;		
						
						pPixels += dist;
						indexCount -= dist;
						
						
						
						if (indexCount > 0)
						{
							k = indexCount;
							do {
								colorSet		= (*pPixels >> 8) & 0xFF;	// set
								colorGradation	= (*pPixels & 0xFF);			// gradation
								pPixels++;

								
								*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
								pDestTemp ++;
							} while (--k);
						}

						
						colorCount = *pPixels++;
						
						CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);
						
						pDestTemp	+= colorCount;
						pPixels		+= colorCount;
						index += colorCount;

						
						break;
					}
				}		

				
				
				index += indexCount;

				
				pPixels += indexCount;

				
				colorCount = *pPixels++;			
				
				//---------------------------------------------
				
				
				//---------------------------------------------
				if (index+colorCount > rectLeft)
				{
					dist = rectLeft - index;				
					colorCount2 = colorCount - dist;

					
					CSpriteSurface::memcpyBrightness(pDestTemp, pPixels+dist, colorCount2);
					pDestTemp += colorCount2;
					pPixels += colorCount;
					index += colorCount;

					
					break;			
				}		

				
				pPixels += colorCount;
				index += colorCount;
			} while (--j);
		

			//---------------------------------------------
			
			
			//---------------------------------------------
			
			//---------------------------------------------
			if (--j > 0)
			{
				do {
					transCount = *pPixels++;		
					indexCount = *pPixels++;		
							
					
					index += transCount;
					
					
					

					

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+indexCount > rectRight)
					{
						
						if (index > rectRight)
						{
							break;
						}
						
						else
						{
							pDestTemp += transCount;

							
							indexCount = rectRight - index;

							
							if (indexCount > 0)
							{
								k = indexCount;
								do {
									colorSet		= (*pPixels >> 8) & 0xFF;	// set
									colorGradation	= (*pPixels & 0xFF);			// gradation
									pPixels++;

									
									*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
									pDestTemp ++;
								} while (--k);
							}						
							
							break;
						}
					}

					
					pDestTemp += transCount;

					//-------------------------
					
					//-------------------------
					
					if (indexCount > 0)
					{
						k = indexCount;
						do {
							colorSet		= (*pPixels >> 8) & 0xFF;	// set
							colorGradation	= (*pPixels & 0xFF);			// gradation
							pPixels++;

							
							*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
							pDestTemp ++;
						} while (--k);
					}
					
					index += indexCount;
					

					
					colorCount = *pPixels++;

					//---------------------------------------------
					
					//---------------------------------------------			
					if (index+colorCount > rectRight)
					{
						
						CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, rectRight - index);
						break;			
					}			
					
					//--------------------
					
					//--------------------
					CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);

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
// Blt Brightness Clip Height
//----------------------------------------------------------------------
void		
CIndexSprite::BltBrightnessClipHeight(WORD *pDest, WORD pitch, RECT* pRect, BYTE DarkBits)
{
	CSpriteSurface::s_Value1 = DarkBits;

	int	count,
			indexCount,
			colorCount;

	WORD	*pDestTemp,
			*pPixels;


	register int  i;
	register int  j;
	register int  k;

	int colorSet, colorGradation;
	int rectBottom = pRect->bottom;
	for (int i=pRect->top; i<rectBottom; i++)
	{			
		pPixels		= m_Pixels[i];
		pDestTemp	= pDest;

		
		count	= *pPixels++;		

		
		if (count > 0)
		{
			j = count;
			do {
				pDestTemp += *pPixels++;		
				indexCount = *pPixels++;	

				
				if (indexCount > 0)
				{
					k = indexCount;
					do {
						colorSet		= (*pPixels >> 8) & 0xFF;	// set
						colorGradation	= (*pPixels & 0xFF);			// gradation
						pPixels++;

						
						*pDestTemp		= ColorSet[s_IndexValue[colorSet]][colorGradation];
						pDestTemp ++;
					} while (--k);
				}

				
				colorCount = *pPixels++;
				
				CSpriteSurface::memcpyBrightness(pDestTemp, pPixels, colorCount);
				
				pDestTemp	+= colorCount;
				pPixels		+= colorCount;
			} while (--j);
		}

		pDest = (WORD*)((BYTE*)pDest + pitch);
	}
}
