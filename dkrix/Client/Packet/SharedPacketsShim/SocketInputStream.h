// Forwarding shim so shared/Packets/CGPickupMoney.cpp's quoted
// #include "SocketInputStream.h" resolves for the client build without
// exposing the whole Client/Packet/ directory (which contains
// generically named headers, e.g. FileAPI.h, that shadow real Windows
// SDK headers once on the global include path — see
// dkrix/CMakeLists.txt's Phase 12 comment and Packet.h in this
// directory for the fuller story).
//
// Added in Wave 4 batch 3. Client/Packet/Packet.h already includes this
// header from its own directory, so every migrated packet gets it
// transitively; CGPickupMoney.cpp is the first migrated file to include
// it directly at top level (its Cpackets copy spelled it
// "../SocketInputStream.h"), which needs a resolvable path of its own.
#include "../SocketInputStream.h"
