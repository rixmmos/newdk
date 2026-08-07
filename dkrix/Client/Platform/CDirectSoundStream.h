//----------------------------------------------------------------------
// CDirectSoundStream.h
//----------------------------------------------------------------------


//----------------------------------------------------------------------

#ifndef __CSDLSTREAM_H__
#define __CSDLSTREAM_H__


#define NUM_PLAY_NOTIFICATIONS  16

/* Platform-independent includes (SDL2 backend on all platforms) */
#include "../../basic/Platform.h"
#include "../../basic/AudioTypes.h"

/* Forward declarations for DirectSound types (opaque pointers) */
/* The actual implementation uses SDL_mixer for all platforms */
struct IDirectSound;
struct IDirectSoundBuffer;
struct IDirectSoundNotify;

/* Type definitions */
#ifndef LPDIRECTSOUNDBUFFER
typedef struct IDirectSoundBuffer* LPDIRECTSOUNDBUFFER;
#endif
typedef struct IDirectSoundNotify* LPDIRECTSOUNDNOTIFY;

struct SDL_POSITION_NOTIFY {
	DWORD dwFlags;
	DWORD dwOffset;
	DWORD dwCallback;
};

class CSDLStream {
	public :
		CSDLStream();
		~CSDLStream();


		void					Release();

		BOOL					IsLoad() const			{ return m_bLoad; }
		BOOL					IsPlay() const			{ return m_bPlay; }

		void					Load(LPSTR filename);

		void					Play(BOOL bLooped);
		void					Stop();

		void					SetVolumeLimit(LONG volume);
		LONG					GetVolumeLimit() const	{ return m_MaxVolume; }

		
		void					Update();

		// get
		LPDIRECTSOUNDBUFFER		GetBuffer() const	{ return m_pDSBuffer; }

	protected :
		BOOL					UpdateProgress();
		BOOL					HandleNotification( BOOL bLooped );
		BOOL					FillBuffer( BOOL bLooped );
		BOOL					ReadStream( BOOL bLooped, VOID* pbBuffer, DWORD dwBufferLength );
		BOOL					RestoreBuffers( BOOL bLooped );

		BOOL					Reset();
		void					WaveReadFile( HMMIO hmmioIn, UINT cbRead, BYTE* pbDest, MMCKINFO* pckIn, UINT* cbActualRead );

	protected :
		BOOL					m_bLoad;
		BOOL					m_bPlay;
		BOOL					m_bLoop;

		LPDIRECTSOUNDBUFFER		m_pDSBuffer;
		LPDIRECTSOUNDNOTIFY		m_pDSNotify;

		SDL_POSITION_NOTIFY		m_aPosNotify[ NUM_PLAY_NOTIFICATIONS + 1 ];

		HANDLE					m_hNotificationEvents[2];

		
		DWORD					m_dwBufferSize;
		DWORD					m_dwNotifySize;
		DWORD					m_dwNextWriteOffset;
		DWORD					m_dwProgress;
		DWORD					m_dwLastPos;
		BOOL					m_bFoundEnd;

		
		WAVEFORMATEX			m_wavefmt;        // Pointer to WAVEFORMATEX structure
		HMMIO					m_hmmioIn;     // MM I/O handle for the WAVE
		MMCKINFO				m_ckIn;        // Multimedia RIFF chunk
		MMCKINFO				m_ckInRiff;    // Use in opening a WAVE file

		LONG					m_MaxVolume;		
};

#endif

