// Forwarding shim so shared/Packets/CLSelectPC.h's quoted
// #include "Player.h" resolves for the client build without exposing
// the whole Client/Packet/ directory (which contains generically named
// headers, e.g. FileAPI.h, that shadow real Windows SDK headers once
// on the global include path — see dkrix/CMakeLists.txt's Phase 12
// comment and Packet.h in this directory for the fuller story).
//
// Added in Wave 4 batch 3: CLSelectPC is the first migrated pair whose
// header includes Player.h. Both trees' copies already did — the client
// Cpackets copy spelled it "../Player.h" — so this forwarder preserves
// the pre-migration resolution exactly. Client/Packet/Player.h is the
// only Player.h anywhere in dkrix/, so there is nothing for this name
// to shadow.
#include "../Player.h"
