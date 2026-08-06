//----------------------------------------------------------------------
//
// Filename    : LGIncomingConnectionHandler.cpp
// Written By  : Reiot
// Description :
//
//----------------------------------------------------------------------

// include files
#include "LGIncomingConnection.h"
#include "Properties.h"

#ifdef __GAME_SERVER__

#include "ConnectionInfo.h"
#include "ConnectionInfoManager.h"
#include "GLIncomingConnectionError.h"
#include "GLIncomingConnectionOK.h"
#include "LogDef.h"
#include "LoginServerManager.h"

#endif

//----------------------------------------------------------------------
//
// LGIncomingConnectionHander::execute()
//


//
//----------------------------------------------------------------------
void LGIncomingConnectionHandler::execute(LGIncomingConnection* pPacket)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

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

        DWORD authKey = rand() << ((time(0) % 10) + rand()) >> (time(0) % 10);

    
    ConnectionInfo* pConnectionInfo = new ConnectionInfo();
    pConnectionInfo->setClientIP(pPacket->getClientIP());
    pConnectionInfo->setKey(authKey);
    pConnectionInfo->setPlayerID(pPacket->getPlayerID());
    pConnectionInfo->setPCName(pPacket->getPCName());

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
    pConnectionInfo->setExpireTime(currentTime);

    // debug message
    /*
    cout << "+--------------------------------+" << endl
         << "| Incoming Connection Infomation |" << endl
         << "+--------------------------------+" << endl
         << "ClientIP : " << pPacket->getClientIP() << endl
         << "Auth Key : " << authKey << endl
         << "P C Name : " << pPacket->getPCName() << endl;
     */

    try {
        
        g_pConnectionInfoManager->addConnectionInfo(pConnectionInfo);

        // by sigi. 2002.12.7
        FILELOG_INCOMING_CONNECTION("connectionInfo.log", "Add [%s:%s] %s (%u)", pPacket->getPlayerID().c_str(),
                                    pPacket->getPCName().c_str(), pPacket->getClientIP().c_str(), authKey);


        
        GLIncomingConnectionOK glIncomingConnectionOK;
        glIncomingConnectionOK.setPlayerID(pPacket->getPlayerID());
        glIncomingConnectionOK.setTCPPort(g_pConfig->getPropertyInt("TCPPort"));
        glIncomingConnectionOK.setKey(authKey);

        g_pLoginServerManager->sendPacket(pPacket->getHost(), pPacket->getPort(), &glIncomingConnectionOK);

        cout << "LGIncomingConnectionHandler Send Packet to ServerIP : " << pPacket->getHost() << endl;
        cout << "LGIncomingConnectionHandler Send Packet to ServerPort : " << pPacket->getPort() << endl;

    } catch (DuplicatedException& de) {
        
        SAFE_DELETE(pConnectionInfo);

        //		GLIncomingConnectionError glIncomingConnectionError;
        //		glIncomingConnectionError.setMessage(de.toString());
        //		glIncomingConnectionError.setPlayerID(pPacket->getPlayerID());

        //		cout << "Step 5" << endl;
        //		g_pLoginServerManager->sendPacket(pPacket->getHost() , pPacket->getPort() , &glIncomingConnectionError);
        //		cout << "LGIncomingConnectionHandler Send Packet to ServerIP : " << pPacket->getHost() << endl;
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
