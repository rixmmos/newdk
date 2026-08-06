//----------------------------------------------------------------------
// MWeather.h
//----------------------------------------------------------------------

//----------------------------------------------------------------------

#ifndef	__MWEATHER_H__
#define	__MWEATHER_H__

#include "MTypeDef.h"
#include "DrawTypeDef.h"


//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MAP_EFFECT {
	public :		
		MAP_EFFECT() { m_Type = MAP_EFFECT_ARRIVE3; }
		~MAP_EFFECT() {}

		void	Set(BYTE type, TYPE_SPRITEID sid, short x, short y, char sx, char sy, BYTE count)
		{
			m_Type		= type;
			m_SpriteID	= sid;
			m_X			= x;
			m_Y			= y;
			m_StepX		= sx;
			m_StepY		= sy;
			m_Count		= count;
			m_maxCount	= count;
		}

		//------------------------------------------------------------
		
		//------------------------------------------------------------
		bool		Move()
		{	
			if (m_Count==0)
				return false;

			m_X += m_StepX;
			m_Y += m_StepY;

			m_Count--;

			return true;
		}

		//------------------------------------------------------------
		// Get Functions
		//------------------------------------------------------------
		BYTE			IsActive() const		{ return m_Count; }
		BYTE			GetType() const			{ return m_Type; }
		TYPE_SPRITEID	GetSpriteID() const		{ return m_SpriteID; }
		short			GetX() const			{ return m_X; }
		short			GetY() const			{ return m_Y; }		
		short			GetCount() const		{ return m_Count; }
		short			GetMaxCount() const		{ return m_maxCount; }

		//------------------------------------------------------------
		// Set
		//------------------------------------------------------------
		void			SetSX(char sx)			{ m_StepX = sx; }
		void			SetSY(char sy)			{ m_StepY = sy; }

		//------------------------------------------------------------
		// operator = 
		//------------------------------------------------------------
		void			operator = (const MAP_EFFECT& effect)
		{
			m_Type		= effect.m_Type;
			m_SpriteID	= effect.m_SpriteID;
			m_X			= effect.m_X;
			m_Y			= effect.m_Y;
			m_StepX		= effect.m_StepX;
			m_StepY		= effect.m_StepY;
			m_Count		= effect.m_Count;
			m_maxCount	= effect.m_maxCount;
		}

	public :
		enum WEATHER_EFFECT_TYPE
		{
			MAP_EFFECT_FALL,
			MAP_EFFECT_ARRIVE1,
			MAP_EFFECT_ARRIVE2,
			MAP_EFFECT_ARRIVE3,
			MAP_EFFECT_ARRIVE4,
			MAP_EFFECT_ARRIVE5,
			MAP_EFFECT_ARRIVE6
		};

	protected :
		BYTE			m_Type;		

		TYPE_SPRITEID	m_SpriteID;

		
		short			m_X, m_Y; 

		
		char			m_StepX, m_StepY;

		
		BYTE			m_Count, m_maxCount;
};


//----------------------------------------------------------------------

//----------------------------------------------------------------------
class MWeather {
	public :
		MWeather();
		~MWeather();

		//-------------------------------
		
		//-------------------------------
		bool		IsActive() const		{ return m_WeatherType!=WEATHER_NULL; }
		BYTE		GetWeatherType() const	{ return m_WeatherType; }		

		//-------------------------------
		
		//-------------------------------
		void		Release();			

		//-------------------------------
		
		//-------------------------------
		void		Stop()					{ m_WeatherType = WEATHER_STOP; }

		//-------------------------------
		
		//-------------------------------
		void		SetRain(BYTE number);			
		void		SetSnow(BYTE number);			
		void		SetSpot(BYTE number);			

		//-------------------------------
		
		//-------------------------------
		void		Action();

		//-------------------------------
		// Get
		//-------------------------------
		int					GetStartX() const		{ return m_StartX; }
		int					GetStartY() const		{ return m_StartY; }
		BYTE				GetSize() const			{ return m_nActiveMapEffect; }
		const MAP_EFFECT&	operator [] (BYTE n)	{ return m_pMapEffect[n]; }

	protected :
		void		Init(BYTE n);		
		
		void		GenerateRain(const BYTE& n);
		void		GenerateSnow(const BYTE& n);
		void		GenerateSpot(const BYTE& n);
	

	public :
		
		enum WEATHER_TYPE
		{
			WEATHER_NULL = 0,
			WEATHER_STOP,		
			WEATHER_RAIN,
			WEATHER_SNOW,
			WEATHER_SPOT,
		};


	
	protected :
		
		int				m_StartX;
		int				m_StartY;

		
		BYTE			m_WeatherType;
		BYTE			m_nMapEffect;
		BYTE			m_nActiveMapEffect;
		MAP_EFFECT*		m_pMapEffect;

		// temp(?)
		BYTE			m_MoreEffectCount;
};


extern	MWeather*	g_pWeather;

#endif

