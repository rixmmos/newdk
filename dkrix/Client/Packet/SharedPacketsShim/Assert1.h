// Forwarding shim so shared/Packets/CGNPCAskAnswer.cpp's quoted
// #include "Assert1.h" resolves for the client build without exposing
// the whole Client/Packet/ directory (which contains generically named
// headers, e.g. FileAPI.h, that shadow real Windows SDK headers once
// on the global include path — see dkrix/CMakeLists.txt's Phase 12
// comment and Packet.h in this directory for the fuller story).
//
// Added in Wave 2 batch A: CGNPCAskAnswer.cpp uses the server's
// "Assert1.h" spelling (both Assert.h and Assert1.h exist, identically,
// in both trees; the server file happened to use Assert1.h). Wave 1
// batch 1 flagged this pair's SocketEncryptInputStream.h path /
// Assert1.h-vs-Assert.h divergence as messier and skipped it; this
// batch takes it on.
#include "../Assert1.h"
