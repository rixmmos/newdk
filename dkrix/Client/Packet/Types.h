//////////////////////////////////////////////////////////////////////////////
// Filename    : Types.h
// Written By  : Reiot
// Description : 
//////////////////////////////////////////////////////////////////////////////

#ifndef __TYPES_H__
#define __TYPES_H__

#include <cstdint>

#include "types/SystemTypes.h"
#include "types/PlayerTypes.h"
#include "types/ObjectTypes.h"
#include "types/CreatureTypes.h"
#include "types/ItemTypes.h"
#include "types/ZoneTypes.h"
#include "types/QuestTypes.h"
#include "types/ShopTypes.h"
#include "types/GuildTypes.h"
#include "types/WarTypes.h"

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
