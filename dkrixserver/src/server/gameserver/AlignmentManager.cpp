//////////////////////////////////////////////////////////////////////////////
// Filename    : AlignmentManager.cpp
// Written By  :
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "AlignmentManager.h"

#include "Assert.h"
#include "DB.h"
// #include <algo.h>

AlignmentManager* g_pAlignmentManager = NULL;

//////////////////////////////////////////////////////////////////////////////
// class AlignmentManager member methods
//////////////////////////////////////////////////////////////////////////////

AlignmentManager::AlignmentManager()

    {__BEGIN_TRY __END_CATCH}

AlignmentManager::~AlignmentManager()

    {__BEGIN_TRY __END_CATCH_NO_RETHROW}

Alignment AlignmentManager::getAlignmentType(Alignment_t Alignment)

{
    __BEGIN_TRY

    if (Alignment <= -10000) {
        return LESS_EVIL;
    }
    if (Alignment >= -10000 && Alignment < -7500) {
        return LESS_EVIL;
    } else if (Alignment >= -7500 && Alignment < -2500) {
        return EVIL;
    } else if (Alignment >= -2500 && Alignment < 2500) {
        return NEUTRAL;
    } else if (Alignment >= 2500 && Alignment < 7500) {
        return GOOD;
    } else if (Alignment >= 7500 && Alignment <= 10000) {
        return MORE_GOOD;
    } else {
        return MORE_GOOD;
    }

    __END_CATCH
}

int AlignmentManager::getMultiplier(Alignment_t AttackerAlignment, Alignment_t DefenderAlignment)

{
    __BEGIN_TRY

    Alignment AAlignmentType = getAlignmentType(AttackerAlignment);
    Alignment DAlignmentType = getAlignmentType(DefenderAlignment);

    
    if (AAlignmentType >= GOOD) {
        
        if (DAlignmentType >= GOOD) {
            
            if (AttackerAlignment > DefenderAlignment) {
                return -200;
            }
            
            else if (AttackerAlignment <= DefenderAlignment) {
                return -300;
            }
        }
        
        else if (DAlignmentType == NEUTRAL) {
            return -100;
        }
        
        else if (DAlignmentType <= EVIL) {
            return 200;
        }
    } else if (getAlignmentType(AttackerAlignment) == NEUTRAL) {
        
        if (DAlignmentType >= GOOD) {
            return -300;
        }
        
        else if (DAlignmentType == NEUTRAL) {
            
            if (AttackerAlignment > DefenderAlignment) {
                return -100;
            }
            
            else if (AttackerAlignment <= DefenderAlignment) {
                return -200;
            }
        }
        
        else if (DAlignmentType <= EVIL) {
            return 100;
        }
    } else if (getAlignmentType(AttackerAlignment) <= EVIL) {
        
        if (DAlignmentType >= GOOD) {
            return -300;
        }
        
        else if (DAlignmentType == NEUTRAL) {
            return -200;
        }
        
        else if (DAlignmentType <= EVIL) {
            
            if (AttackerAlignment > DefenderAlignment) {
                return 200;
            }
            
            else if (AttackerAlignment <= DefenderAlignment) {
                return 100;
            }
        }
    } else {
        return -300;
    }

    return -300;

    __END_CATCH
}

BYTE AlignmentManager::getDropItemNum(Alignment_t Alignment, bool isPK)

{
    __BEGIN_TRY

    int Count = 0;

    if (Alignment > -10000 && Alignment < -7500) {
        Count = 2;
    } else if (Alignment >= -7500 && Alignment < -2500) {
        Count = 1;
    } else if (Alignment == -10000) {
        Count = 3;
    }

     

    return (BYTE)Count;

    __END_CATCH
}

BYTE AlignmentManager::getDropBonusPercentage(Alignment_t Alignment)

{
    __BEGIN_TRY
    return 0;

    int Percent = (10000 - Alignment) / 400;

    Percent = max(0, Percent);
    Percent = min(50, Percent);

    return (BYTE)Percent;

    __END_CATCH
}

BYTE AlignmentManager::getMoneyDropPenalty(Alignment_t Alignment)

{
    __BEGIN_TRY

    BYTE Penalty = 0;

    if (Alignment == 10000) {
        Penalty = 0;
    } else if (Alignment >= 7500 && Alignment < 10000) {
        Penalty = 1;
    } else if (Alignment >= 2500 && Alignment < 7500) {
        Penalty = 2;
    } else if (Alignment >= -2500 && Alignment < 2500) {
        Penalty = 4;
    } else if (Alignment >= -7500 && Alignment < -2500) {
        Penalty = 8;
    } else if (Alignment >= -10000 && Alignment < -7500) {
        Penalty = 16;
    } else {
        Penalty = 32;
    }

    return Penalty;

    __END_CATCH
}

string AlignmentManager::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "AlignmentManager ("
        << ")";
    return msg.toString();

    __END_CATCH
}
