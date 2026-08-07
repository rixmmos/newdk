/*-----------------------------------------------------------------------------

	DXLib.h

	Direct X Library Master Header file.

	2000.1.3. KJTINC

-----------------------------------------------------------------------------*/

#ifndef __DX_LIBRARY_H__
#define __DX_LIBRARY_H__

#ifdef PLATFORM_WINDOWS
#include "CSDLGraphics.h"
#endif
#include "CDirectDrawSurface.h"
#include "Platform/AudioManager.h"
#include "CDirectSoundStream.h"
#include "CDirectMusic.h"  // Now CSDLMusic
#include "Platform/InputManager.h"

#endif