/*-----------------------------------------------------------------------------

	InputService.h

	Forward declaration and accessor for the global input service.
	This provides a cleaner include path for game code.

	2025.02.04 - Created to reduce DXLib header dependencies
	            2025.02.04 - Updated to use InputCodes instead of CDirectInput

-----------------------------------------------------------------------------*/

#ifndef __INPUTSERVICE_H__
#define __INPUTSERVICE_H__

// Include key code definitions (platform-independent SDL-based)
#include "basic/InputCodes.h"

// Forward declaration for input service class
class InputManager;

// Global input service instance (defined in DXLib/InputManager.cpp)
extern InputManager* g_pSDLInput;

#endif // __INPUTSERVICE_H__
