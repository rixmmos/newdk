////////////////////////////////////////////////////////////////////////////////
// Filename    : Vision.cpp
// Written By  : Reiot
// Description :
////////////////////////////////////////////////////////////////////////////////

#include "VisionInfo.h"

#include "Assert.h"
#include "LogClient.h"

////////////////////////////////////////////////////////////////////////////////
// global variable definition
////////////////////////////////////////////////////////////////////////////////
VisionInfoManager* g_pVisionInfoManager = NULL;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
 
//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
VisionInfoManager::~VisionInfoManager()

{
    __BEGIN_TRY

    /*	if (m_pVisionInfos)
        {
            for (Sight_t sight = 0 ; sight <= maxSight ; sight ++)
            {
                if (m_pVisionInfos[sight])
                {
                    for (Dir_t dir = 0 ; dir < DIR_MAX ; dir ++)
                    {
                        SAFE_DELETE(m_pVisionInfos[sight][dir]);
                    }

                    SAFE_DELETE_ARRAY(m_pVisionInfos[sight]);
                }
            }

            SAFE_DELETE_ARRAY(m_pVisionInfos);
        }*/

    __END_CATCH_NO_RETHROW
}


//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void VisionInfoManager::init()

    {

        __BEGIN_TRY

            /*	m_pVisionInfos = new VisionInfo ** [ maxSight ];
                Assert(m_pVisionInfos != NULL);

                for (Sight_t sight = 0 ; sight <= maxSight ; sight ++)
                {
                    m_pVisionInfos[sight] = new VisionInfo* [8];
                    Assert(m_pVisionInfos[sight] != NULL);

                    for (Dir_t dir = 0 ; dir < DIR_MAX ; dir ++)
                    {
                        m_pVisionInfos[sight][dir] = new VisionInfo(sight,dir);
                        Assert(m_pVisionInfos[sight][dir] != NULL);
                    }
                }	*/

            __END_CATCH}


//--------------------------------------------------------------------------------
// get vision info
//--------------------------------------------------------------------------------
/*
VisionInfo* VisionInfoManager::getVisionInfo (Sight_t sight , Dir_t dir)
{
    __BEGIN_TRY

    if (sight > maxSight) throw OutOfBoundException("sight overflow");

    if (dir >= DIR_MAX) throw OutOfBoundException("dir overflow");

    return m_pVisionInfos[sight][dir];

    __END_CATCH
}
*/


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string VisionInfoManager::toString() const

{
    __BEGIN_TRY

    /*	StringStream msg;

        msg << "VisionInfoManager(";

        for (Sight_t sight = 0 ; sight <= maxSight ; sight ++)
        {
            for (Dir_t dir = LEFT ; dir <= LEFTUP ; dir ++)
            {
                msg << m_pVisionInfos[sight][dir]->toString();
            }
        }

        msg << ")";

        return msg.toString();*/

    __END_CATCH
}
