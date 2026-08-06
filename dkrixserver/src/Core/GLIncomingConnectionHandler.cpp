//----------------------------------------------------------------------
//
// Filename    : GLIncomingConnectionHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "GLIncomingConnection.h"
#include "Properties.h"

#ifdef __LOGIN_SERVER__

#include "GameServerManager.h"
#include "LGIncomingConnectionError.h"
#include "LGIncomingConnectionOK.h"
#include "ReconnectLoginInfo.h"
#include "ReconnectLoginInfoManager.h"

#endif

//----------------------------------------------------------------------
//
// GLIncomingConnectionHander::execute()
//


//
//----------------------------------------------------------------------
void GLIncomingConnectionHandler::execute(GLIncomingConnection* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG

#ifdef __LOGIN_SERVER__

        //--------------------------------------------------------------------------------
        //
        
        //
        // *NOTE*
        //
        
        
        
        
        
        
        
        //
        
        //
        // *TODO*
        //
        
        
        
        //
        
        //
        //--------------------------------------------------------------------------------

        DWORD authKey = rand() << (time(0) % 10) + rand() >> (time(0) % 10);

    
    ReconnectLoginInfo* pReconnectLoginInfo = new ReconnectLoginInfo();
    pReconnectLoginInfo->setClientIP(pPacket->getClientIP());
    pReconnectLoginInfo->setPlayerID(pPacket->getPlayerID());
    pReconnectLoginInfo->setKey(authKey);

    //--------------------------------------------------------------------------------
    //
    
    //
    // *TODO*
    //
    
    //
    //--------------------------------------------------------------------------------
    Timeval currentTime;
    getCurrentTime(currentTime);
    currentTime.tv_sec += 30;
    pReconnectLoginInfo->setExpireTime(currentTime);

    // debug message
    // cout << "+--------------------------------+" << endl << "| Incoming Connection Infomation |" << endl <<
    // "+--------------------------------+" << endl << "ClientIP : " << pPacket->getClientIP() << endl << "Auth Key : "
    // << authKey << endl;

    try {
        
        g_pReconnectLoginInfoManager->addReconnectLoginInfo(pReconnectLoginInfo);

        
        LGIncomingConnectionOK lgIncomingConnectionOK;
        lgIncomingConnectionOK.setPlayerID(pPacket->getPlayerID());
        lgIncomingConnectionOK.setTCPPort(g_pConfig->getPropertyInt("LoginServerPort"));
        lgIncomingConnectionOK.setKey(authKey);

        g_pGameServerManager->sendPacket(pPacket->getHost(), pPacket->getPort(), &lgIncomingConnectionOK);

        // cout << "GLIncomingConnectionHandler Send Packet to ServerIP : " << pPacket->getHost() << endl;
    } catch (DuplicatedException& de) {
        // cout << "Duplicated Exception Throwed Step 1" << endl;

        
        SAFE_DELETE(pReconnectLoginInfo);
        // cout << "Duplicated Exception Throwed Step 2" << endl;

        LGIncomingConnectionError lgIncomingConnectionError;
        lgIncomingConnectionError.setMessage(de.toString());
        lgIncomingConnectionError.setPlayerID(pPacket->getPlayerID());
        // cout << "Duplicated Exception Throwed Step 3" << endl;

        g_pGameServerManager->sendPacket(pPacket->getHost(), pPacket->getPort(), &lgIncomingConnectionError);

        // cout << "Duplicated Exception Throwed Step 4" << endl;

        // cout << "GLIncomingConnectionHandler Send Packet to ServerIP : " << pPacket->getHost() << endl;
    }

#endif

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}
