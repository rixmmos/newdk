//----------------------------------------------------------------------
// CDirectSound.h
//----------------------------------------------------------------------

#ifndef __CDirectSound_H__
#define __CDirectSound_H__

#pragma warning(disable:4786)

/* Platform-independent includes (SDL2 backend on all platforms) */
#include "../../basic/Platform.h"
#include "../../basic/AudioTypes.h"
#include <list>
#include <cstring>

/* Forward declarations for DirectSound types (opaque pointers) */
/* The actual implementation uses SDL_mixer for all platforms */
typedef struct IDirectSound* LPDIRECTSOUND;
#ifndef LPDIRECTSOUNDBUFFER
typedef struct IDirectSoundBuffer* LPDIRECTSOUNDBUFFER;
#endif
typedef std::list<LPDIRECTSOUNDBUFFER>	LPDIRECTSOUNDBUFFER_LIST;

class CSDLAudio
{

	public:		
		CSDLAudio();
		~CSDLAudio();

		//---------------------------------------------------------
		// Init / Release
		//---------------------------------------------------------
		bool					Init(HWND);									
		void					Release();									
		void					ReleaseDuplicateBuffer();

	bool					IsInit() const;

		//---------------------------------------------------------
		// Load / Release / Duplicate
		//---------------------------------------------------------
		LPDIRECTSOUNDBUFFER		LoadWav(LPSTR filename);					
		LPDIRECTSOUNDBUFFER		CreateBuffer(LPVOID sdat, DWORD size, DWORD caps, LPWAVEFORMATEX wfx);
		void					Release(LPDIRECTSOUNDBUFFER);				
		LPDIRECTSOUNDBUFFER		DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, bool bAutoRelease=true);	

		//---------------------------------------------------------
		// Play / Stop
		//---------------------------------------------------------
		bool					IsPlay(LPDIRECTSOUNDBUFFER) const;								
		bool					NewPlay(LPDIRECTSOUNDBUFFER, bool bLoop=false);	
		bool					Play(LPDIRECTSOUNDBUFFER, bool bLoop=false, bool bDuplicate=true);		
		bool					Stop(LPDIRECTSOUNDBUFFER);					
		void					ReleaseTerminatedDuplicateBuffer();

		//---------------------------------------------------------
		// Mute
		//---------------------------------------------------------
		bool					IsMute() const;
		void					SetMute();
		void					UnSetMute();

		//---------------------------------------------------------
		// Frequency
		//---------------------------------------------------------
		bool					AddFrequency(LPDIRECTSOUNDBUFFER, int);		
		bool					SubFrequency(LPDIRECTSOUNDBUFFER, int);		
	
		//---------------------------------------------------------
		// Volume
		//---------------------------------------------------------
		bool					SetMaxVolume(LPDIRECTSOUNDBUFFER buffer);
		bool					AddVolume(LPDIRECTSOUNDBUFFER, int);		
		bool					SubVolume(LPDIRECTSOUNDBUFFER, int);		
		bool					SubVolumeFromMax(LPDIRECTSOUNDBUFFER, int);	
		void					SetVolumeLimit(LONG volume);
		LONG					GetVolumeLimit() const;

		//---------------------------------------------------------
		// Pan
		//---------------------------------------------------------
		bool					RightPan(LPDIRECTSOUNDBUFFER, int);			
		bool					LeftPan(LPDIRECTSOUNDBUFFER, int);			
		bool					CenterToRightPan(LPDIRECTSOUNDBUFFER, int);			
		bool					CenterToLeftPan(LPDIRECTSOUNDBUFFER, int);			
		bool					CenterPan(LPDIRECTSOUNDBUFFER);				
		bool					ChangePan(LPDIRECTSOUNDBUFFER buffer, int pan);	// -10000 ~ 10000

		LPDIRECTSOUND			GetDS() const;

		

	protected:
		bool					DirectSoundFailed(const char *str);		


	protected :
		LPDIRECTSOUND			m_pDS;										
		bool					m_bInit;

		bool					m_bMute;			

		LONG					m_MaxVolume;		

		LPDIRECTSOUNDBUFFER_LIST	m_listDuplicatedBuffer;


	friend class CSDLMusic;
	friend class CSDLStream;
};

extern	CSDLAudio		g_SDLAudio;

#endif