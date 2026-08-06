#ifndef _MP3_READER_H_
#define _MP3_READER_H_

#ifdef PLATFORM_WINDOWS
#include <wtypes.h>
#else
#include "../../basic/Platform.h"
#endif
#include "mp3.h"
#include <stdio.h>

extern int Open(LPSTR PathName) ;						
extern int Close() ;									
extern int ReadHeader(UINT *headerStr) ;				
extern int ReadBytes(BYTE* buf, UINT bytes) ;			
extern int ReadWords(WORD* buf, UINT words) ;			
extern int ReadDwords(DWORD* buf, UINT dwords) ;		
extern UINT ReadBits(UINT bits) ;
extern int ReadSideInfo(UINT channel, side_info_t *sideInfo) ;	// 
extern int GetCurOffset() ;				
extern int GetFileSize() ;				
extern int SetOffset(int offset) ;
extern int GetMaxFrames(MP3 *mp3) ;
extern int GetMinFrames(MP3 *mp3) ;

#endif //_MP3_READER_H_
