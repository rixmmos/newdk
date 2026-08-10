//----------------------------------------------------------------------
//
// Filename    : GMServerInfoHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GMServerInfo.h"
#include "Properties.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "UserInfoManager.h"
#endif


//----------------------------------------------------------------------
//
// GMServerInfoHander::execute()
//


//
//----------------------------------------------------------------------
void GMServerInfoHandler::execute(GMServerInfo* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG

#ifdef __LOGIN_SERVER__

        ZONEUSERDATA rData;
    WorldID_t WorldID = pPacket->getWorldID();
    ServerGroupID_t ServerGroupID = pPacket->getServerID();
    BYTE MaxCount = pPacket->getZoneUserCount();

    // WorldID and ServerGroupID come off the wire. getUserInfo() range-checks
    // WorldID and throws NoSuchElementException for an unknown pair, which the
    // datagram receive loop already treats as "drop this datagram". The Assert
    // used to sit after the first dereference, where it could not help.
    UserInfo* pUserInfo = g_pUserInfoManager->getUserInfo(ServerGroupID, WorldID);
    Assert(pUserInfo != NULL);

    pUserInfo->setUserNum(0);
    for (int count = 0; count < MaxCount; count++) {
        pPacket->popZoneUserData(rData);
        pUserInfo->setUserNum(pUserInfo->getUserNum() + rData.UserNum);
    }

#endif

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
