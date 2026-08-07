// Forwarding shim so shared/Packets/*.h's quoted #include "Packet.h"
// resolves for the client build without exposing the whole
// Client/Packet/ directory (which contains generically named headers,
// e.g. FileAPI.h, that shadow real Windows SDK headers once on the
// global include path — see dkrix/CMakeLists.txt's Phase 12 comment).
// This directory contains only these two forwarders, so it is safe to
// add target-wide.
#include "../Packet.h"
