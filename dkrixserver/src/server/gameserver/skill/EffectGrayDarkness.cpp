//////////////////////////////////////////////////////////////////////////////
// Filename    : EffectGrayDarkness.cpp
// Written by  : elca
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "EffectGrayDarkness.h"

#include "Creature.h"
#include "DB.h"
#include "Tile.h"
#include "Zone.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

EffectGrayDarkness::EffectGrayDarkness(Zone* pZone, ZoneCoord_t ZoneX, ZoneCoord_t ZoneY)

{
    __BEGIN_TRY

    m_pZone = pZone;
    m_X = ZoneX;
    m_Y = ZoneY;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// EffectGrayDarkness::affectCreature()


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// unaffect()
//////////////////////////////////////////////////////////////////////////////
void EffectGrayDarkness::unaffect()

{
    __BEGIN_TRY

    // cout << "EffectGrayDarkness " << "unaffect BEGIN" << endl;

    Tile& tile = m_pZone->getTile(m_X, m_Y);

    // unaffect creatures on tile
    //	const forward_list<Object*>& oList = tile.getObjectList();
    //	for (forward_list<Object*>::const_iterator itr = oList.begin(); itr != oList.end(); itr++)
    //	{
    //		if (*itr != this)
    //		{
    //			EffectGrayDarkness::unaffectObject((Object*)(*itr), false);
    //		}
    //	}

    tile.deleteEffect(m_ObjectID);

    // cout << "EffectGrayDarkness " << "unaffect END" << endl;

    __END_CATCH
}

/*void EffectGrayDarkness::unaffect(Zone* pZone , ZoneCoord_t x , ZoneCoord_t y , Object* pObject)

{
    __BEGIN_TRY
    __END_CATCH
}*/

string EffectGrayDarkness::toString() const throw() {
    __BEGIN_TRY

    StringStream msg;
    msg << "EffectGrayDarkness("
        << "DayTime:" << m_Deadline.tv_sec << ")";
    return msg.toString();

    __END_CATCH
}
