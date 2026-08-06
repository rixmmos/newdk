//-----------------------------------------------------------------------------
// MGameTime.h
//-----------------------------------------------------------------------------
// 

//

//

//
//-----------------------------------------------------------------------------

#ifndef __MGAMETIME_H__
#define	__MGAMETIME_H__

class MGameTime {
	public :
		MGameTime();
		~MGameTime();

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		void	SetStartTime(DWORD time, WORD year, BYTE month, BYTE day, BYTE hour, BYTE minute, BYTE second);

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		void	SetTimeRatio(int ratio)		{ m_TimeRatio = ratio; }
		int		GetTimeRatio()				{ return m_TimeRatio; }

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		void	SetCurrentTime(DWORD time);

		//-------------------------------------------------------------
		// Get
		//-------------------------------------------------------------
		WORD	GetYear() const			{ return m_Year; }
		BYTE	GetMonth() const		{ return m_Month; }
		BYTE	GetDay() const			{ return m_Day; }
		BYTE	GetHour() const			{ return m_Hour; }
		BYTE	GetMinute() const		{ return m_Minute; }
		BYTE	GetSecond() const		{ return m_Second; }


	protected :
		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		// YYYY-MM-DD
		WORD		m_StartYear;
		BYTE		m_StartMonth;
		BYTE		m_StartDay;

		// HH:MM:SS
		BYTE		m_StartHour;
		BYTE		m_StartMinute;
		BYTE		m_StartSecond;

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		DWORD		m_StartTime;

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		int			m_TimeRatio;

		//-------------------------------------------------------------
		
		//-------------------------------------------------------------
		// YYYY-MM-DD
		WORD		m_Year;
		BYTE		m_Month;
		BYTE		m_Day;

		// HH:MM:SS
		BYTE		m_Hour;
		BYTE		m_Minute;
		BYTE		m_Second;
};


extern MGameTime*	g_pGameTime;

#endif

