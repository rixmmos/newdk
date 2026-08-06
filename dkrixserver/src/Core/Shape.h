//////////////////////////////////////////////////////////////////////////////
// Filename    : Shape.h
// Written by  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __SHAPE_H__
#define __SHAPE_H__

#include "Exception.h"
#include "Types.h"

BYTE getShoulderType(ItemType_t IType);
PantsType getPantsType(ItemType_t IType);
JacketType getJacketType(ItemType_t IType);
HelmetType getHelmetType(ItemType_t IType);
ShieldType getShieldType(ItemType_t IType);
MotorcycleType getMotorcycleType(ItemType_t IType);


VampireCoatType getVampireCoatType(ItemType_t IType);


OustersCoatType getOustersCoatType(ItemType_t IType);
OustersArmType getOustersArmType(ItemType_t IType);

#endif
