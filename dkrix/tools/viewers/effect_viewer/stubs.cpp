/**
 * @file stubs.cpp
 * @brief Stub implementations for effect_viewer
 *
 * Provides minimal implementations of dependencies required by MEffect
 * without pulling in the entire game engine.
 */

#include "Client/MTopView.h"
#include "Client/DrawTypeDef.h"
#include "Client/MEffectTarget.h"

// Global variables
DWORD g_CurrentFrame = 0;
MTopView* g_pTopView = nullptr;

// Stub implementation of GetEffectLight
// In a real scenario, this would look up the light value from effect frame data
// For effect_viewer, we return a default value
int MTopView::GetEffectLight(BLT_TYPE bltType, TYPE_FRAMEID frameID, int direction, int frame) const {
    // Default light value for effects
    // TODO: Look up actual light value from EffectResourceContainer
    return 0;
}

// MEffect.cpp emits references to MEffectTarget's typeinfo because the class
// is polymorphic. Its key function -- the first non-inline virtual, i.e. the
// destructor -- is what emits the vtable and typeinfo, so defining it here is
// enough to link. Compiling Client/MEffectTarget.cpp instead was tried and
// reverted: it drags in MActionResult, MPlayer, MString, CZoneTable, g_pPlayer
// and g_pZoneTable, which is the entire game engine this file exists to avoid.
//
// The real destructor deletes m_pResult, an MActionResult* -- precisely the
// dependency being avoided -- so this body is deliberately empty. The viewer
// leaks one result per target; that is the trade this file exists to make.
MEffectTarget::~MEffectTarget() {}

// Defining the key function above emits the vtable here, and the vtable has a
// slot for every virtual -- including operator=, which is virtual on this
// class (MEffectTarget.h:70) and non-inline. So the destructor alone was not
// enough: client CI run #40 failed with exactly one undefined reference,
// MEffectTarget::operator=(MEffectTarget const&), from
// _ZTV13MEffectTarget in this object file.
//
// Reproduced faithfully rather than left as a no-op. The real body
// (Client/MEffectTarget.cpp:94-104) is eight plain member assignments with
// none of the dependencies above, so there is no reason to diverge -- and a
// silently non-assigning operator= would be a far worse trap than a link
// error.
void MEffectTarget::operator=(const MEffectTarget& target) {
    m_MaxPhase = target.m_MaxPhase;
    m_CurrentPhase = target.m_CurrentPhase;
    m_DelayFrame = target.m_DelayFrame;
    m_X = target.m_X;
    m_Y = target.m_Y;
    m_Z = target.m_Z;
    m_ID = target.m_ID;
    m_ServerID = target.m_ServerID;
}
