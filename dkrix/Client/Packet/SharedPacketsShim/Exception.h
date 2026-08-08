// Forwarding shim so shared/Packets/*.h's quoted #include "Exception.h"
// resolves for the client build without exposing the whole
// Client/Packet/ directory (which contains generically named headers,
// e.g. FileAPI.h, that shadow real Windows SDK headers once on the
// global include path — see dkrix/CMakeLists.txt's Phase 12 comment and
// Packet.h in this directory for the fuller story).
//
// Added in Wave 1 batch 2: the pilot and Wave 1 batch 1 pairs never
// included Exception.h/Types.h explicitly (Packet.h already pulls both
// in transitively), so no forwarder was needed until this batch's pairs
// — which DO include them explicitly, matching the server's own
// pre-migration style — needed one. This directory holds only these
// forwarders, so it is safe to add target-wide.
#include "../Exception.h"
