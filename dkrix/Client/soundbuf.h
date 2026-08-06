#ifndef _SOUNDBUF_H_
#define _SOUNDBUF_H_

#ifdef PLATFORM_WINDOWS
#include <wtypes.h>
#include <dsound.h>
#else
#include "../basic/Platform.h"  // For basic types
#include "CDirectSoundStream.h"  // For DSBPOSITIONNOTIFY definition
#endif
#include "mp3.h"


struct MP3 ;

struct SOUNDBUF
{
	char		databuf[0x1200] ;		
	int			nWritten ;				
	int			lp ;					
	int			rp ;					
	int			channels ;				
	int			nFrames ;				
	int			nPushedFrames ;			
	int			bufSize ;				
	int			curBuffer ;				
	LPDIRECTSOUNDBUFFER		dsPlayBuf ;				
	LPDIRECTSOUNDBUFFER		dsWriteBuf ;			
	int			readyBufs ;				
	int			offWrite ;				
	BOOL		bPlaying ;				
} ;

typedef SOUNDBUF	*LPSOUNDBUF ;


LPSOUNDBUF CreateSoundBuf ( MP3 *mp3, int nFrames, DSBPOSITIONNOTIFY[], int n  ) ;
LPSOUNDBUF CreateSoundBuf ( MP3 *mp3, int nFrames) ;
int AppendData ( int ch, LPSOUNDBUF lpsb, int data ) ;
int Destroy ( LPSOUNDBUF lpsb ) ;
int WriteTo ( LPSOUNDBUF lpsb , BYTE* arr) ;
extern int OutputData (LPSOUNDBUF lpsb ) ;
#endif // _SOUNDBUF_H_