//----------------------------------------------------------------------
//
// Filename    : PCCorpse.cpp
// Written by  : Reiot
//
//----------------------------------------------------------------------

#ifndef __PC_CORPSE_H__
#define __PC_CORPSE_H__

// include files
#include "Corpse.h"

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void PCCorpse::transportItems(Creature* pCreature)

{}

//----------------------------------------------------------------------
// get debug string
//----------------------------------------------------------------------
string PCCorpse::toString() const

{
    StringStream msg;

    msg << "PCCorpse()";

    return msg.toString();
}
