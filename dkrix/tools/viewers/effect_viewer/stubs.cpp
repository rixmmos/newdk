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
MEffectTarget::~MEffectTarget() {}
