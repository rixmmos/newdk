//////////////////////////////////////////////////////////////////////////////
// Filename    : Types.h
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>

#include "Types/SystemTypes.h"
#include "Types/PlayerTypes.h"
#include "Types/ObjectTypes.h"
#include "Types/CreatureTypes.h"
#include "Types/ItemTypes.h"
#include "Types/ZoneTypes.h"
#include "Types/QuestTypes.h"
#include "Types/ShopTypes.h"
#include "Types/GuildTypes.h"
#include "Types/WarTypes.h"

//#include "Utility.h"

// Some Windows TU/PCH combinations reach Packet headers before the legacy
// aliases from SystemTypes.h become visible. Keep a guarded local fallback so
// the packet layer remains self-contained.
#ifndef DKRIX_LEGACY_UINT_TYPES_DEFINED
#define DKRIX_LEGACY_UINT_TYPES_DEFINED
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#endif


#ifndef __XMAS_EVENT_CODE__
	#define __XMAS_EVENT_CODE__
#endif


#endif
