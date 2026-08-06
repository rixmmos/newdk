//----------------------------------------------------------------------
// MWeather.cpp
//----------------------------------------------------------------------



//----------------------------------------------------------------------
#include "Client_PCH.h"
#include "MWeather.h"
#include "MPlayer.h"
#include "MViewDef.h"
#include "DebugInfo.h"
extern BOOL g_MyFull;
extern RECT g_GameRect;
//#define	new			DEBUG_NEW
//#define	delete		DEBUG_DELETE

//----------------------------------------------------------------------
// Global
//----------------------------------------------------------------------
MWeather*	g_pWeather;

//----------------------------------------------------------------------
//
//  constructor/destructor
//
//----------------------------------------------------------------------
MWeather::MWeather()
{
	m_WeatherType		= WEATHER_NULL;
	m_nMapEffect		= 0;
	m_nActiveMapEffect	= 0;
	m_pMapEffect		= NULL;
	m_MoreEffectCount	= 0;
}

MWeather::~MWeather()
{
	Stop();
	Release();
}

//----------------------------------------------------------------------
//
//  member functions
//
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Init
//----------------------------------------------------------------------
void
MWeather::Init(BYTE n)
{
	
	if (n==m_nMapEffect) 
		return;

	
	Release();

	
	m_nMapEffect = n;
	m_pMapEffect = new MAP_EFFECT [ m_nMapEffect ];
}

//----------------------------------------------------------------------
// Release
//----------------------------------------------------------------------
void
MWeather::Release()
{
	
	if (m_pMapEffect != NULL)
	{
		delete [] m_pMapEffect;
		m_nMapEffect = 0;
		m_pMapEffect = NULL;
		//m_nActiveMapEffect = 0;
		m_nActiveMapEffect = 0;
		m_WeatherType	= WEATHER_NULL;
	}
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MWeather::SetRain(BYTE number)
{
	if (number==0) return;

	//------------------------------------------------
	
	
	//------------------------------------------------
	if (m_WeatherType == WEATHER_RAIN)
	{
		//----------------------------------------
		
		//----------------------------------------
		if (number == m_nMapEffect)
			return;

		//----------------------------------------
		
		//----------------------------------------
		
		MAP_EFFECT*		pMapEffect = new MAP_EFFECT [ number ];

		//----------------------------------------
		
		//----------------------------------------
		int i;
		if (number > m_nMapEffect)
		{
			
			for (i=0; i<m_nActiveMapEffect; i++)
			{
				pMapEffect[i] = m_pMapEffect[i];
			}	
			
			
			m_MoreEffectCount = 2;
		}
		//----------------------------------------
		
		//----------------------------------------
		else
		{
			m_nActiveMapEffect = min( m_nActiveMapEffect, number );

			for (i=0; i<m_nActiveMapEffect; i++)
			{
				pMapEffect[i] = m_pMapEffect[i];
			}
		}

		
		Release();

		
		m_WeatherType	= WEATHER_RAIN;	
		m_pMapEffect = pMapEffect;
		m_nMapEffect = number;
		
		return;
	}

	//------------------------------------------------
	//
	
	//
	//------------------------------------------------

	
	Init( number );

	
	m_WeatherType	= WEATHER_RAIN;
	m_StartX		= g_pPlayer->GetX()*TILE_X;
	m_StartY		= g_pPlayer->GetY()*TILE_Y;

	
	m_nActiveMapEffect = 1;

	for (int i=0; i<m_nActiveMapEffect; i++)
	{
		GenerateRain( i );
	}

	
	m_MoreEffectCount = 2;	
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MWeather::SetSnow(BYTE number)
{
	if (number==0) return;

	//------------------------------------------------
	
	
	//------------------------------------------------
	if (m_WeatherType == WEATHER_SNOW)
	{
		//----------------------------------------
		
		//----------------------------------------
		if (number == m_nMapEffect)
			return;

		//----------------------------------------
		
		//----------------------------------------
		
		MAP_EFFECT*		pMapEffect = new MAP_EFFECT [ number ];

		//----------------------------------------
		
		//----------------------------------------
		int i;
		if (number > m_nMapEffect)
		{
			
			for (i=0; i<m_nActiveMapEffect; i++)
			{
				pMapEffect[i] = m_pMapEffect[i];
			}	
			
			
			m_MoreEffectCount = 2;
		}
		//----------------------------------------
		
		//----------------------------------------
		else
		{
			m_nActiveMapEffect = min( m_nActiveMapEffect, number );

			for (i=0; i<m_nActiveMapEffect; i++)
			{
				pMapEffect[i] = m_pMapEffect[i];
			}
		}

		
		Release();

		
		m_WeatherType	= WEATHER_SNOW;	
		m_pMapEffect = pMapEffect;
		m_nMapEffect = number;
		
		return;
	}

	//------------------------------------------------
	//
	
	//
	//------------------------------------------------

	
	Init( number );

	
	m_WeatherType	= WEATHER_SNOW;
	m_StartX		= g_pPlayer->GetX()*TILE_X;
	m_StartY		= g_pPlayer->GetY()*TILE_Y;

	
	m_nActiveMapEffect = 1;

	for (int i=0; i<number; i++)
	{
		GenerateSnow( i );
	}
	
	m_MoreEffectCount = 2;
}

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MWeather::SetSpot(BYTE number)
{
	if (number==0) return;
	
	//------------------------------------------------
	
	
	//------------------------------------------------
	if (m_WeatherType == WEATHER_SPOT)
	{
		//----------------------------------------
		
		//----------------------------------------
		if (number == m_nMapEffect)
			return;
		
		//----------------------------------------
		
		//----------------------------------------
		
		MAP_EFFECT*		pMapEffect = new MAP_EFFECT [ number ];
		
		//----------------------------------------
		
		//----------------------------------------
		int i;
		if (number > m_nMapEffect)
		{
			
			for (i=0; i<m_nActiveMapEffect; i++)
			{
				pMapEffect[i] = m_pMapEffect[i];
			}	
			
			
			m_MoreEffectCount = 2;
		}
		//----------------------------------------
		
		//----------------------------------------
		else
		{
			m_nActiveMapEffect = min( m_nActiveMapEffect, number );
			
			for (i=0; i<m_nActiveMapEffect; i++)
			{
				pMapEffect[i] = m_pMapEffect[i];
			}
		}
		
		
		Release();
		
		
		m_WeatherType	= WEATHER_SPOT;	
		m_pMapEffect = pMapEffect;
		m_nMapEffect = number;
		
		return;
	}
	
	//------------------------------------------------
	//
	
	//
	//------------------------------------------------
	
	
	Init( number );
	
	
	m_WeatherType	= WEATHER_SPOT;
	m_StartX		= g_pPlayer->GetX()*TILE_X;
	m_StartY		= g_pPlayer->GetY()*TILE_Y;
	
	
	m_nActiveMapEffect = 1;
	
	for (int i=0; i<number; i++)
	{
		GenerateSpot( i );
	}
	
	m_MoreEffectCount = 2;
}


//----------------------------------------------------------------------
// Generate Rain 
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MWeather::GenerateRain(const BYTE& n)
{
	BYTE type;

	switch (m_pMapEffect[n].GetType())
	{			
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		case MAP_EFFECT::MAP_EFFECT_ARRIVE4 : 
		
			type = rand() & 0x01;	// %2

			switch (type)
			{				
				case 0 :
					m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_FALL,
								0, 
								rand()%g_GameRect.right, rand()%g_GameRect.bottom, 
								-1*((rand()%4)+1), rand()%30+20, 
								rand()%25);
				break;

				case 1 :
					m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_FALL,
								rand()%2+1, 
								rand()%g_GameRect.right, rand()%g_GameRect.bottom, 
								-1*((rand()%6)+3), rand()%28+17, 
								rand()%20);
				break;
			}
		break;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		case MAP_EFFECT::MAP_EFFECT_FALL :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE1,
						3,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						1);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE1 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE2,
						4,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						1);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE2 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE3,
						5,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						1);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE3 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE4,
						6,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						1);	
		break;
	}

}


//----------------------------------------------------------------------
// Generate Snow 
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MWeather::GenerateSnow(const BYTE& n)
{
	BYTE type;

	switch (m_pMapEffect[n].GetType())
	{			
		//--------------------------------------------------------
		
		//--------------------------------------------------------
		case MAP_EFFECT::MAP_EFFECT_ARRIVE6 : 
		
			type = rand() & 0x01; //%2;

			switch (type)
			{				
				case 0 :
					m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_FALL,
								rand()%5+7, 
								rand()%g_GameRect.right, rand()%g_GameRect.bottom, 
								-1*((rand()%3)+1), rand()%7+4, 
								rand()%120);
				break;

				case 1 :
					m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_FALL,
								rand()%5+7,  
								rand()%g_GameRect.right, rand()%g_GameRect.bottom, 
								-1*((rand()%3)+3), rand()%7+4, 
								rand()%120);
				break;
			}
		break;

		//--------------------------------------------------------
		
		//--------------------------------------------------------
		case MAP_EFFECT::MAP_EFFECT_FALL :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE1,
						12,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						(rand() & 0x01)+2);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE1 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE2,
						13,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						(rand() & 0x01)+2);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE2 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE3,
						14,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						(rand() & 0x01)+2);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE3 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE4,
						15,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						(rand() & 0x01)+2);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE4 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE5,
						16,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						(rand() & 0x01)+2);	
		break;

		case MAP_EFFECT::MAP_EFFECT_ARRIVE5 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE6,
						17,						
						m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
						0, 0, 
						(rand() & 0x01)+2);	
		break;
	}

}

//----------------------------------------------------------------------
// Generate Spot
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void
MWeather::GenerateSpot(const BYTE& n)
{
// 	BYTE type;
//	
//	switch (m_pMapEffect[n].GetType())
//	{			
//		//--------------------------------------------------------

//		//--------------------------------------------------------
//	case MAP_EFFECT::MAP_EFFECT_ARRIVE6 : 
//		
//		type = rand() & 0x01; //%2;
//		
//		switch (type)
//		{				
//		case 0 :
			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_FALL,
				18+rand()%6,
				rand()%g_GameRect.right, rand()%g_GameRect.bottom, 
				0, -((rand()%4)+1),
				30+rand()%30);
//			break;
//			
//		case 1 :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_FALL,
//				18,  
//				rand()%800, rand()%600, 
//				((rand()%7)+4), 0, 
//				60);
//			break;
//		}
//		break;
		
//		//--------------------------------------------------------

//		//--------------------------------------------------------
//		case MAP_EFFECT::MAP_EFFECT_FALL :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE1,
//				18,						
//				m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
//				0, 0, 
//				(rand() & 0x01)+2);	
//			break;
//			
//		case MAP_EFFECT::MAP_EFFECT_ARRIVE1 :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE2,
//				18,						
//				m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
//				0, 0, 
//				(rand() & 0x01)+2);	
//			break;
//			
//		case MAP_EFFECT::MAP_EFFECT_ARRIVE2 :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE3,
//				18,						
//				m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
//				0, 0, 
//				(rand() & 0x01)+2);	
//			break;
//			
//		case MAP_EFFECT::MAP_EFFECT_ARRIVE3 :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE4,
//				18,						
//				m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
//				0, 0, 
//				(rand() & 0x01)+2);	
//			break;
//			
//		case MAP_EFFECT::MAP_EFFECT_ARRIVE4 :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE5,
//				18,						
//				m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
//				0, 0, 
//				(rand() & 0x01)+2);	
//			break;
//			
//		case MAP_EFFECT::MAP_EFFECT_ARRIVE5 :
//			m_pMapEffect[n].Set(MAP_EFFECT::MAP_EFFECT_ARRIVE6,
//				18,						
//				m_pMapEffect[n].GetX(), m_pMapEffect[n].GetY(), 
//				0, 0, 
//				(rand() & 0x01)+2);	
//			break;

//	}
	
}


//----------------------------------------------------------------------
// Action
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void		
MWeather::Action()
{
	if (m_pMapEffect==NULL)
		return;

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (m_WeatherType == WEATHER_NULL)
	{
		return;
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	if (m_WeatherType == WEATHER_STOP)
	{
		int k=0;
		for (int i=0; i<m_nMapEffect; i++)
		{
			if (m_pMapEffect[i].Move())
			{
			}
			else
			{
				//m_pMapEffect[i].Set(0, -100, -100, 0,0,0);
				k++;
			}
		}

		if (k==m_nMapEffect)
		{		
			m_WeatherType = WEATHER_NULL;
		}

		return;
	}

	//---------------------------------------------------------
	
	
	//---------------------------------------------------------
	if (m_nActiveMapEffect < m_nMapEffect)
	{
		if (m_MoreEffectCount==0)
		{
			// 
			BYTE start = m_nActiveMapEffect;

			m_nActiveMapEffect += 5;

			if (m_nActiveMapEffect > m_nMapEffect)
				m_nActiveMapEffect = m_nMapEffect;

			BYTE i;
			
			
			switch (m_WeatherType)
			{
				case WEATHER_RAIN :
					for (i=start; i<m_nActiveMapEffect; i++)
					{
						GenerateRain( i );
					}
				break;

				case WEATHER_SNOW :
					for (i=start; i<m_nActiveMapEffect; i++)
					{
						GenerateSnow( i );
					}
				break;

				case WEATHER_SPOT :
					for (i=start; i<m_nActiveMapEffect; i++)
					{
						GenerateSpot( i );
					}
					break;
			}

			m_MoreEffectCount = 10;
		}
		else
		{
			m_MoreEffectCount--;
		}
	}

	//---------------------------------------------------------
	
	//---------------------------------------------------------
	for (int i=0; i<m_nActiveMapEffect; i++)
	{
		
		if (m_pMapEffect[i].Move())
		{
			switch (m_WeatherType)
			{
				case WEATHER_RAIN :
				break;

				
				case WEATHER_SNOW :
//				case WEATHER_SPOT :
					if (m_pMapEffect[i].GetCount() & 0x08)
					{
						m_pMapEffect[i].SetSX( -1*(rand()%3-1) * ((rand()%3)+1) );
					}
				break;
			}
		}
		
		if(!m_pMapEffect[i].IsActive())
		{
			
			switch (m_WeatherType)
			{
				case WEATHER_RAIN :
				{
					GenerateRain( i );
				}
				break;
				
				case WEATHER_SNOW :
				{
					GenerateSnow( i );
				}
				break;

				case WEATHER_SPOT :
				{
					GenerateSpot( i );
				}
				break;
			}
		}
	}
}
	
