//-----------------------------------------------------------------------------
// MGameTime.cpp
//-----------------------------------------------------------------------------
#include "Client_PCH.h"
#include "MGameTime.h"

//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------
MGameTime*	g_pGameTime = NULL;

extern DWORD		g_CurrentTime;

//-----------------------------------------------------------------------------
//
// constructor / destructor
//
//-----------------------------------------------------------------------------
MGameTime::MGameTime()
{
	m_StartYear		= 0;
	m_StartMonth	= 0;
	m_StartDay		= 0;

	m_StartHour		= 0;
	m_StartMinute	= 0;
	m_StartSecond	= 0;

	// timeGetTime
	m_StartTime		= 0;

	
	m_TimeRatio		= 1;

	
	m_Year			= 0;
	m_Month			= 0;
	m_Day			= 0;

	m_Hour			= 0;
	m_Minute		= 0;
	m_Second		= 0;
}

MGameTime::~MGameTime()
{
}

//-----------------------------------------------------------------------------
//
// member functions
//
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Set StartTime
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void	
MGameTime::SetStartTime(DWORD time, WORD year, BYTE month, BYTE day, BYTE hour, BYTE minute, BYTE second)
{
	// timeGetTime
	m_StartTime		= time;

	m_StartYear		= year;
	m_StartMonth	= month;
	m_StartDay		= day;

	m_StartHour		= hour;
	m_StartMinute	= minute;
	m_StartSecond	= second;	
}

//-----------------------------------------------------------------------------
// Set CurrentTime
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void	
MGameTime::SetCurrentTime(DWORD time)
{
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	const DWORD Day	= 24*60*60;
	const DWORD Hour	= 60*60;
	const DWORD Min	= 60;
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	const BYTE DaysPerMonth[13] = 
	{
		0, 31, 28, 31, 30, 31, 30, 
		31, 31, 30, 31, 30, 31
	};


	//------------------------------------------------------------
	
	//------------------------------------------------------------
	
	//------------------------------------------------------------
	DWORD gap = (time - m_StartTime) / 1000 * m_TimeRatio;

	int rem = gap;

	
	rem += m_StartSecond + m_StartMinute*Min + m_StartHour*Hour;

	//------------------------------------------------------------
	
	//------------------------------------------------------------
	int	d = rem/Day;			rem -= d*Day;	
	int	h = rem/Hour;			rem -= h*Hour;	
	int	m = rem/Min;			rem -= m*Min;
	int	s = rem;

	m_Hour		= h;
	m_Minute	= m;
	m_Second	= s;

	
	int days = DaysPerMonth[m_StartMonth];

	
	if (m_StartMonth==2)
	{	
		
		if ((m_StartYear&0x03)==0 && m_StartYear%100!=0 || m_StartYear%400==0)
		{
			days ++;
		}
	}
	
	
	m_Day = m_StartDay + d;

	
	if (m_StartDay+d > days)
	{
		
		m_Day -= days;

		
		m_Month = m_StartMonth + 1;

		if (m_Month==13)
		{
			m_Year ++;
			m_Month = 1;
		}
		else
		{
			
			m_Year = m_StartYear;
		}

		
		SetStartTime(g_CurrentTime, m_Year, m_Month, m_Day, m_Hour, m_Minute, m_Second);
	}
	else
	{
		m_Month = m_StartMonth;
		m_Year	= m_StartYear;
	}

}
