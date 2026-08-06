//////////////////////////////////////////////////////////////////////////////
// Filename    : PCCorpse.h
// Written by  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __PC_CORPSE_H__
#define __PC_CORPSE_H__

#include "Corpse.h"

//////////////////////////////////////////////////////////////////////////////
// class PCCorpse;

//////////////////////////////////////////////////////////////////////////////

class PCCorpse : public Corpse {
    // methods from Object
public:
    virtual string toString() const;

    // methods from Item
public:
    
    void transportItems(Creature* pCreature);

    // data members
private:
    
    
    HairStyle m_HairStyle;
    Color_t m_HairColor;
    Color_t m_SkinColor;
    Color_t m_UpperColors[2];
    Color_t m_LowerColors[2];
};

#endif
