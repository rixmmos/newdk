//--------------------------------------------------------------------------------
//
// Filename   : UpdateDef.h
// Written By : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __UPDATE_DEF_H__
#define __UPDATE_DEF_H__

#include "../Packet/Types.h"
#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include "../../basic/Platform.h"
#endif

//--------------------------------------------------------------------------------
//

//
//--------------------------------------------------------------------------------

typedef DWORD FileSize_t;
const uint szFileSize = sizeof(FileSize_t);

typedef BYTE FilenameLen_t;
const uint szFilenameLen = sizeof(FilenameLen_t);
const uint maxFilename = 256;

// version
typedef WORD Version_t;
const uint szVersion = sizeof(Version_t);


typedef WORD ParameterLen_t;
const uint szParameterLen = sizeof(ParameterLen_t);


const uint maxParams = 6;

// max parameter length

const uint maxParameterLen = 256 + 15;

#endif
