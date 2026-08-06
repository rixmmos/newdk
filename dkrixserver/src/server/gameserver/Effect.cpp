//////////////////////////////////////////////////////////////////////////////
// Filename    : Effect.cpp
// Written by  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "Effect.h"

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
Effect::Effect()

{
    __BEGIN_TRY

    m_pZone = NULL;
    m_X = 0;
    m_Y = 0;
    m_pTarget = NULL;
    m_bBroadcastingEffect = true;

    setNextTime(99999999);
    setDeadline(99999999);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////
Effect::Effect(Zone* pZone, ZoneCoord_t x, ZoneCoord_t y, Object* pTarget, Turn_t delay)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = x;
    m_Y = y;
    m_pTarget = pTarget;

    setNextTime(99999999);
    setDeadline(delay);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
Effect::~Effect()

{}

void Effect::setNextTime(Turn_t delay)

{
    __BEGIN_TRY

    
    getCurrentTime(m_NextTime);

    
    m_NextTime.tv_sec += delay / 10;
    m_NextTime.tv_usec += (delay % 10) * 100000;

    __END_CATCH
}

void Effect::setDeadline(Turn_t delay) {
    __BEGIN_TRY

    
    getCurrentTime(m_Deadline);

    
    
    m_Deadline.tv_sec += delay / 10;
    m_Deadline.tv_usec += (delay % 10) * 100000;

    __END_CATCH
}


Duration_t Effect::getRemainDuration() {
    __BEGIN_TRY

    Timeval currentTime;
    getCurrentTime(currentTime);
    return (m_Deadline.tv_sec - currentTime.tv_sec) * 10 + (m_Deadline.tv_usec - currentTime.tv_usec) / 100000;

    __END_CATCH
}
