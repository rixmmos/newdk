////////////////////////////////////////////////////////////////////////////////
// Filename    : VisionInfo.h
// Written By  : Reiot
// Description :
////////////////////////////////////////////////////////////////////////////////

#ifndef __VISIONINFO_H__
#define __VISIONINFO_H__

#include "Exception.h"
#include "Types.h"

const Coord_t maxViewportWidth = 64;
const Coord_t maxViewportUpperHeight = 72;
const Coord_t maxViewportLowerHeight = 72;
const Coord_t maxVisionWidth = maxViewportWidth * 2 + 1;
const Coord_t maxVisionHeight = maxViewportUpperHeight + maxViewportLowerHeight + 1;

////////////////////////////////////////////////////////////////////////////////
// class VisionInfo;




////////////////////////////////////////////////////////////////////////////////

 

////////////////////////////////////////////////////////////////////////////////
// class VisionInfoManager;

////////////////////////////////////////////////////////////////////////////////

class VisionInfoManager {
public:
    VisionInfoManager() {}
    ~VisionInfoManager();

public:
    // init vision info
    void init();

    // get vision info
    /*	VisionInfo* getVisionInfo(Sight_t sight, Dir_t dir) //
        {
            // for speed. by sigi. 2002.11.8
            //__BEGIN_TRY
            //if (sight > maxSight) throw OutOfBoundException("sight overflow");
            //if (dir >= DIR_MAX) throw OutOfBoundException("dir overflow");

            return m_pVisionInfos[sight][dir];

            //__END_CATCH
        }*/

    static VisionState getVisionState(ZoneCoord_t SourceX, ZoneCoord_t SourceY, ZoneCoord_t TargetX,
                                      ZoneCoord_t TargetY) {
        int diffX = abs(TargetX - SourceX);
        int diffY = (int)TargetY - (int)SourceY;

        bool isInX = diffX <= maxViewportWidth;
        bool isInY = (diffY < 0) ? ((-diffY) <= maxViewportUpperHeight) : (diffY <= maxViewportLowerHeight);

        return (isInX && isInY) ? IN_SIGHT : OUT_OF_SIGHT;
    }

    // get debug string
    string toString() const;

private:
    
    //	VisionInfo *** m_pVisionInfos;
};

// global variable declaration
extern VisionInfoManager* g_pVisionInfoManager;

#endif
