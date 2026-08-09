// Forwarding shim so shared/Packets/CGPickupMoney.cpp's quoted
// #include "SocketOutputStream.h" resolves for the client build without
// exposing the whole Client/Packet/ directory (which contains
// generically named headers, e.g. FileAPI.h, that shadow real Windows
// SDK headers once on the global include path — see
// dkrix/CMakeLists.txt's Phase 12 comment and Packet.h in this
// directory for the fuller story).
//
// Added in Wave 4 batch 3, alongside SocketInputStream.h in this
// directory and for the same reason.
#include "../SocketOutputStream.h"
