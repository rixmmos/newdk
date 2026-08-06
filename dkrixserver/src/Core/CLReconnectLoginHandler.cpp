//////////////////////////////////////////////////////////////////////////////
// Filename    : CLReconnectLoginHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLReconnectLogin.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "GCDisconnect.h"
#include "GameServerInfoManager.h"
#include "LCPCList.h"
#include "LoginPlayer.h"
#include "OptionInfo.h"
#include "Properties.h"
#include "ReconnectLoginInfoManager.h"

#endif

//////////////////////////////////////////////////////////////////////////////
// CLReconnectLoginHandler::execute()
//




//



//

//////////////////////////////////////////////////////////////////////////////
void CLReconnectLoginHandler::execute(CLReconnectLogin* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    // cout << "CLReconnectLogin : STARTING HANDLING PROCESS" << endl;

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);
    pLoginPlayer->setWorldID(pPacket->isWebLogin());
    // pLoginPlayer->setWebLogin(g_pConfig->getPropertyInt("WebLogin") != 0);

    string PlayerID;

    //----------------------------------------------------------------------
    
    
    //----------------------------------------------------------------------
    try {
        ReconnectLoginInfo* pReconnectLoginInfo =
            g_pReconnectLoginInfoManager->getReconnectLoginInfo(pLoginPlayer->getSocket()->getHost());

        PlayerID = pReconnectLoginInfo->getPlayerID();

        
        pLoginPlayer->setID(PlayerID);

        
        if (pPacket->getKey() != pReconnectLoginInfo->getKey())
            throw InvalidProtocolException("invalid key");

        
        Timeval currentTime;
        getCurrentTime(currentTime);
        if (pReconnectLoginInfo->getExpireTime() < currentTime) {
            
            g_pReconnectLoginInfoManager->deleteReconnectLoginInfo(pReconnectLoginInfo->getClientIP());
            throw InvalidProtocolException("session already expired");
        }

        
        g_pReconnectLoginInfoManager->deleteReconnectLoginInfo(pReconnectLoginInfo->getClientIP());

    } catch (NoSuchElementException& nsee) 
    {
        
        
        
        
        GCDisconnect gcDisconnect;
        gcDisconnect.setMessage(nsee.toString());

        pLoginPlayer->sendPacket(&gcDisconnect);

        
        throw InvalidProtocolException("fuck");
    } catch (InvalidProtocolException& ipe) {
        cout << endl
             << "+-----------------------+" << endl
             << "| Level 2 Access Denied |" << endl
             << "+-----------------------+" << endl
             << endl;

        GCDisconnect gcDisconnect;
        gcDisconnect.setMessage(ipe.toString());

        pLoginPlayer->sendPacket(&gcDisconnect);

        
        throw;
    }

    ServerGroupID_t CurrentServerGroupID = 0;

    Statement* pGStmt = NULL;
    Result* pGResult = NULL;

    
    PayType payType;
    string payPlayDate;
    uint payPlayHours;
    uint payPlayFlag;


    try {
        pGStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
        pGResult = pGStmt->executeQuery("SELECT CurrentWorldID, CurrentServerGroupID, LogOn, Access, PayType, "
                                        "PayPlayDate, PayPlayHours, PayPlayFlag FROM Player WHERE PlayerID = '%s'",
                                        PlayerID.c_str());

        
        pGResult->next();

        WorldID_t CurrentWorldID = pGResult->getInt(1);

        CurrentServerGroupID = pGResult->getInt(2);

        pLoginPlayer->setWorldID(CurrentWorldID);
        pLoginPlayer->setServerGroupID(CurrentServerGroupID);

        string logon = pGResult->getString(3);

        payType = (PayType)(pGResult->getInt(5));
        payPlayDate = pGResult->getString(6);
        payPlayHours = pGResult->getInt(7);
        payPlayFlag = pGResult->getInt(8);


        if (logon == "GAME" || logon == "LOGON") {
            string msg = "ReconnectLogin verify failed: LogOn = ";
            msg += logon;
            pLoginPlayer->setID("NONE");
            throw SQLQueryException(msg);
        }

        if (logon == "LOGOFF") // || logon == "GAME") // by sigi. 2002.5.15
        {
            Statement* pStmt = NULL;
            try {
                pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

                
                pStmt->executeQuery(
                    "UPDATE Player SET LogOn='LOGON', CurrentLoginServerID=%d WHERE PlayerID='%s' AND LogOn='LOGOFF'",
                    g_pConfig->getPropertyInt("LoginServerID"), PlayerID.c_str());

                
                
                if (pStmt->getAffectedRowCount() == 0) {
                    // cout << endl << "+-----------------------+" << endl << "|    Deny Multi Login   |" << endl <<
                    // "+-----------------------+" << endl << endl;

                    throw SQLQueryException("Deny MultiLogin");
                }

                SAFE_DELETE(pStmt);
            } catch (SQLQueryException& sqe) {
                SAFE_DELETE(pStmt);
                throw;
            }
        }

        string access = pGResult->getString(4);

        //		if (access == "DENY" || access == "WAIT") {
        if (access != "ALLOW") {
            throw SQLQueryException("ReconnectLogin verify failed ");
        }

#ifdef __THAILAND_SERVER__
        if (strPermission != "ALLOW" && onChildGuardTimeArea(g_pConfig->getPropertyInt("CHILDGUARD_START_TIME"),
                                                             g_pConfig->getPropertyInt("CHILDGUARD_END_TIME"),
                                                             g_pConfig->getProperty("CHILDGUARD"))) {
            throw SQLQueryException("Player Permission is DENY (child guard) running. ");
        }
#endif

#ifdef __PAY_SYSTEM_LOGIN__
        string connectIP = pLoginPlayer->getSocket()->getHost();
        
        if (!pLoginPlayer->loginPayPlay(payType, payPlayDate, payPlayHours, payPlayFlag, connectIP, PlayerID)) {
            SAFE_DELETE(pGStmt);
            throw InvalidProtocolException("Pay First!");
        }
#elif defined(__PAY_SYSTEM_FREE_LIMIT__)
        string connectIP = pLoginPlayer->getSocket()->getHost();
        
        if (pLoginPlayer->loginPayPlay(payType, payPlayDate, payPlayHours, payPlayFlag, connectIP, PlayerID)) {
            
            
            // by sigi. 2002.11.22
            
            
            // #ifdef __CONNECT_BILLING_SYSTEM__
            // pLoginPlayer->setBillingLoginVerified();
            // pLoginPlayer->setBillingUserStatus("XX");
            // #endif
        } else {
            
            // by sigi. 2002.11.22
            
            
            // #ifdef __CONNECT_BILLING_SYSTEM__
            // pLoginPlayer->setBillingLoginVerified();
            // pLoginPlayer->setBillingUserStatus("HO");
            // #endif
        }
#else // elif defined(__PAY_SYSTEM_ZONE__)
        pLoginPlayer->setPayPlayValue(payType, payPlayDate, payPlayHours, payPlayFlag);
#endif


        SAFE_DELETE(pGStmt);
    } catch (SQLQueryException& sce) {
        // cout << sce.toString() << endl;
        
        SAFE_DELETE(pGStmt);
        throw DisconnectException(sce.toString());
    }

    // cout << "CLReconnectLogin : ReconnectLoginInfo verified" << endl;

    pLoginPlayer->setServerGroupID(CurrentServerGroupID);

    pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);


    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    LCPCList lcPCList;

    pLoginPlayer->makePCList(lcPCList);
    pLoginPlayer->sendPacket(&lcPCList);
    pLoginPlayer->setPlayerStatus(LPS_PC_MANAGEMENT);

#ifdef __CONNECT_CBILLING_SYSTEM__
    pLoginPlayer->setCBillingVerified();
    pLoginPlayer->setPayPlayer();
#endif

#endif

    __END_DEBUG_EX __END_CATCH
}
#ifdef __THAILAND_SERVER__
bool CLReconnectLoginHandler::onChildGuardTimeArea(int pm, int am, string enable) {
    bool returnValue = false;
    tm Timem;
    time_t daytime = time(0);
    localtime_r(&daytime, &Timem);

    int Hour = Timem.tm_hour;
    int Min = Timem.tm_min;

    int timeValue = (Hour * 100) + Min;
    bool bSwitch = (enable == "ENABLE" || enable == "enable" || enable == "Enable");

    if ((timeValue >= pm && timeValue <= am) && bSwitch) {
        returnValue = true;
    } else if ((timeValue <= pm && timeValue <= am) && bSwitch) {
        if (am > 1200)
            returnValue = false;
        else
            returnValue = true;
    } else if ((timeValue <= pm && timeValue <= am) && bSwitch) {
        returnValue = false;
    } else if ((timeValue >= pm && timeValue >= am) && bSwitch) {
        if (am > 1200)
            returnValue = false;
        else
            returnValue = true;
    }


    return returnValue;
}
#endif
