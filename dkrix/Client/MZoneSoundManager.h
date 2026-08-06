//-----------------------------------------------------------------------------
// MZoneSoundManager.h
//-----------------------------------------------------------------------------




//


//-----------------------------------------------------------------------------

#ifndef	__MZONESOUNDMANAGER_H__
#define __MZONESOUNDMANAGER_H__

#include "MZoneSound.h"
#include "CTypeMap.h"
#ifdef PLATFORM_WINDOWS
#include <DSound.h>
#endif
#include <map>

#include <fstream>
using namespace std;

//-----------------------------------------------------------------------------
// ZONESOUND_NODE
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
class ZONESOUND_NODE
{
	public :
		ZONESOUND_NODE(TYPE_SOUNDID id=SOUNDID_NULL);
		~ZONESOUND_NODE();

		//---------------------------------------------------------------
		// Get
		//---------------------------------------------------------------
		TYPE_SOUNDID			GetSoundID() const		{ return m_SoundID; }
		LPDIRECTSOUNDBUFFER		GetBuffer() const		{ return m_pBuffer; }
		int						GetX() const			{ return m_X; }
		int						GetY() const			{ return m_Y; }
		bool					IsLoop() const			{ return m_bLoop; }

		//---------------------------------------------------------------
		// Play
		//---------------------------------------------------------------
		void					Play(int x=0, int y=0, bool bLoop=false);
		void					Stop();
		
		//---------------------------------------------------------------
		// Loop
		//---------------------------------------------------------------
		void					StopLoop();			
		bool					IsContinueLoop() const	{ return m_bContinueLoop; }
		void					SetContinueLoop()		{ m_bContinueLoop = true; }
		void					UnSetContinueLoop()		{ m_bContinueLoop = false; }
		
		//---------------------------------------------------------------
		// File I/O
		//---------------------------------------------------------------
		void					SaveToFile(std::ofstream& file);
		void					LoadFromFile(std::ifstream& file);

	protected :
		TYPE_SOUNDID				m_SoundID;				// Sound ID
		LPDIRECTSOUNDBUFFER			m_pBuffer;				// buffer
		int							m_X, m_Y;				
		bool						m_bLoop;				
		bool						m_bContinueLoop;		
		int							m_playerX, m_playerY;	
};


//-----------------------------------------------------------------------------
// MZoneSoundTable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class MZoneSoundTable : public CTypeMap<ZONESOUND_INFO>
{
	public :
		MZoneSoundTable();
		~MZoneSoundTable();
};

//-----------------------------------------------------------------------------
// MZoneSoundManager
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class MZoneSoundManager : public CTypeMap<ZONESOUND_NODE>
{
	public :
		MZoneSoundManager();
		~MZoneSoundManager();

		//---------------------------------------------------------------------
		// Update Sound
		//---------------------------------------------------------------------		
		void			UpdateSound();

	protected :
		TYPE_SECTORPOSITION		m_LastX, m_LastY;
		DWORD					m_LastUpdateTime;
};

//-----------------------------------------------------------------------------
// global
//-----------------------------------------------------------------------------
extern MZoneSoundTable*		g_pZoneSoundTable;

extern MZoneSoundManager*	g_pZoneSoundManager;

#endif