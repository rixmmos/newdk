// Forwarding shim so shared/Packets/CGNPCAskAnswer.cpp's quoted
// #include "SocketEncryptOutputStream.h" resolves for the client build
// without exposing the whole Client/Packet/ directory (which contains
// generically named headers, e.g. FileAPI.h, that shadow real Windows
// SDK headers once on the global include path — see
// dkrix/CMakeLists.txt's Phase 12 comment and Packet.h in this
// directory for the fuller story).
//
// Added in Wave 2 batch A. Once forwarded, the real
// Client/Packet/SocketEncryptOutputStream.h resolves its own quoted
// includes from its own directory, same as always — this shim only
// bridges the first hop.
#include "../SocketEncryptOutputStream.h"
