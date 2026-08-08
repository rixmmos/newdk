// Forwarding shim so shared/Packets/CGPortCheck.h's quoted
// #include "DatagramPacket.h" resolves for the client build without
// exposing the whole Client/Packet/ directory (which contains
// generically named headers, e.g. FileAPI.h, that shadow real Windows
// SDK headers once on the global include path — see
// dkrix/CMakeLists.txt's Phase 12 comment and Packet.h in this
// directory for the fuller story).
//
// Added in Wave 2 batch A: CGPortCheck derives from DatagramPacket
// (Wave 1 batch 1 flagged this pair as messier for exactly this reason
// and skipped it; this batch takes it on). Once forwarded, the real
// Client/Packet/DatagramPacket.h resolves its own "Packet.h" /
// "Datagram.h" / "SocketAPI.h" quoted includes from its own directory,
// same as always — this shim only bridges the first hop.
#include "../DatagramPacket.h"
