//////////////////////////////////////////////////////////////////////////////
// Filename    : CommonBillingPacketHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "BillingPlayer.h"
#include "CommonBillingPacket.h"

#ifdef __GAME_SERVER__
#include "BillingResultLoginErrorMessage.h"
#include "EventKick.h"
#include "EventSystemMessage.h"
#include "GamePlayer.h"
#include "PCFinder.h"
#include "PlayerCreature.h"
#include "ResurrectLocationManager.h"
#include "StringPool.h"
#include "VSDateTime.h"
#include "ZoneUtil.h"
#elif defined(__LOGIN_SERVER__)
#include "LoginPlayer.h"
#include "LoginPlayerManager.h"
#endif

#include "GCSystemMessage.h"


extern void sendSystemMessage(GamePlayer* pGamePlayer, const string& msg);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CommonBillingPacketHandler::execute(CommonBillingPacket* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

        // #ifdef __GAME_SERVER__

        
        filelog(LOGFILE_BILLING_PACKET, "%s", pPacket->toString().c_str());

    int code = pPacket->Packet_Type;

    switch (code) {
    
    case BILLING_PACKET_LOGIN:
        executeBillingLoginVerify(pPacket, pPlayer);
        break;

    
    case BILLING_PACKET_REMAIN:
        executeBillingRemain(pPacket, pPlayer);
        break;

    
    case BILLING_PACKET_CHECK:
        executeBillingCheck(pPacket, pPlayer);
        break;

    
    case BILLING_PACKET_LOGIN_CHECK:
        executeBillingLoginCheckVerify(pPacket, pPlayer);
        break;


    default:
        break;
    }


    // #endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CommonBillingPacketHandler::executeBillingLoginVerify(CommonBillingPacket* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        int result = pPacket->Result;

    __ENTER_CRITICAL_SECTION((*g_pPCFinder))

    Creature* pCreature = g_pPCFinder->getCreatureByID_LOCKED(pPacket->User_ID);

    if (pCreature != NULL) {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());

        
        pGamePlayer->setBillingLoginVerified();

        if (result == BILLING_RESULT_LOGIN_OK) {
            char userStatus[10];
            memcpy(userStatus, pPacket->User_Status, 2);
            userStatus[2] = '\0';

            if (strcmp(userStatus, "PM") != 0) {
                char billMethod[10], expireDate[20];
                memcpy(billMethod, pPacket->Bill_Method, 2);
                billMethod[2] = '\0';
                memcpy(expireDate, pPacket->Expire_Date, 12);
                expireDate[12] = '\0';

                filelog(LOGFILE_BILLING_PLAYER, "LoginVerify: PayPlay (%s, %s, %s, %d)", pPacket->User_ID, billMethod,
                        expireDate, pPacket->Remain_Time);
#ifdef __COUT_BILLING_SYSTEM__
                cout << "LoginVerify: PayPlay ( " << pPacket->User_ID << ", " << billMethod << ", " << expireDate
                     << ", " << pPacket->Remain_Time << " )" << endl;
#endif
            } else {
                
                filelog(LOGFILE_BILLING_PLAYER, "LoginVerify: PayPlay (%s, PCRoom)", pPacket->User_ID);
#ifdef __COUT_BILLING_SYSTEM__
                cout << "LoginVerify: PayPlay ( " << pPacket->User_ID << ", PCRoom )" << endl;
#endif
            }

            
            pGamePlayer->setBillingUserStatus(pPacket->User_Status);
            pGamePlayer->setPremiumPlay();


#ifdef __PAY_SYSTEM_FREE_LIMIT__
            if (pGamePlayer->isPayPlaying()) {
                bool bClear = true; 
                pGamePlayer->logoutPayPlay(pGamePlayer->getID(), bClear);
            }
#endif

            
            sendBillingRemainMessage(pPacket, pGamePlayer);
        } else if (result == BILLING_RESULT_LOGIN_NO_ACCOUNT) {
            
            
            
            // by bezz 2003.04.22

            filelog(LOGFILE_BILLING_PLAYER, "LoginVerify: No Account(%s)", pPacket->User_ID);

#ifdef __COUT_BILLING_SYSTEM__
            cout << "LoginVerify: No Account(" << pPacket->User_ID << ")" << endl;
#endif

            
            
            // pGamePlayer->setPenaltyFlag( PENALTY_TYPE_KICKED );

            // #ifdef __PAY_SYSTEM_FREE_LIMIT__
             
        } else {
            BillingResultLoginErrorMessage* pMessage = BillingResultLoginErrorMessage::Instance();
            sendSystemMessage(pGamePlayer, pMessage->getMessage(result));

            filelog(LOGFILE_BILLING_PLAYER, "LoginVerify: Disconnect (%s)", pPacket->User_ID);

#ifdef __COUT_BILLING_SYSTEM__
            cout << "LoginVerify: Disconnect (" << pPacket->User_ID << ")" << endl;
#endif

            disconnectGamePlayer(pGamePlayer, 10 * 10); 
        }
    } else {
        filelog(LOGFILE_BILLING_PLAYER, "LoginVerify: No Such Player(%s)", pPacket->User_ID);

#ifdef __COUT_BILLING_SYSTEM__
        cout << "LoginVerify: No Such Player(" << pPacket->User_ID << ")" << endl;
#endif
    }

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

#else
        filelog(LOGFILE_BILLING_PLAYER, "[Error] BillingLoginVerify: Wrong Packet(%s)", pPacket->User_ID);
#endif

    __END_DEBUG_EX __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CommonBillingPacketHandler::executeBillingLoginCheckVerify(CommonBillingPacket* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        int result = pPacket->Result;

    __ENTER_CRITICAL_SECTION((*g_pLoginPlayerManager))

    LoginPlayer* pLoginPlayer = g_pLoginPlayerManager->getPlayer_NOLOCKED(pPacket->User_ID);

    if (pLoginPlayer != NULL) {
        
        pLoginPlayer->setBillingLoginVerified();

        if (result == BILLING_RESULT_LOGIN_OK) {
            char billMethod[10], expireDate[20];
            memcpy(billMethod, pPacket->Bill_Method, 2);
            billMethod[2] = '\0';
            memcpy(expireDate, pPacket->Expire_Date, 12);
            expireDate[12] = '\0';

            filelog(LOGFILE_BILLING_PLAYER, "LoginVerify: PayPlayCheck (%s, %s, %s, %d)", pPacket->User_ID, billMethod,
                    expireDate, pPacket->Remain_Time);
#ifdef __COUT_BILLING_SYSTEM__
            cout << "LoginVerify: PayPlayCheck ( " << pPacket->User_ID << ", " << billMethod << ", " << expireDate
                 << ", " << pPacket->Remain_Time << " )" << endl;
#endif

            
            pLoginPlayer->setBillingUserStatus(pPacket->User_Status);


// if (pLoginPlayer->getBillingUserStatus().empty())
//{
//	pLoginPlayer->setBillingUserStatus( "HO" );
//}


// pLoginPlayer->setPremiumPlay();


#ifdef __PAY_SYSTEM_FREE_LIMIT__
            if (pLoginPlayer->isPayPlaying()) {
                bool bClear = true; 
                pLoginPlayer->logoutPayPlay(pLoginPlayer->getID(), bClear);
            }
#endif
        } else {
            if (pLoginPlayer->isPayPlaying()) {
                
                
                pLoginPlayer->setBillingUserStatus("XX");

                filelog(LOGFILE_BILLING_PLAYER, "LoginCheckVerify: Can FreePlay (%s)", pPacket->User_ID);

#ifdef __COUT_BILLING_SYSTEM__
                cout << "LoginCheckVerify: Can FreePlay (" << pPacket->User_ID << ")" << endl;
#endif

            } else {
                
                cout << pPacket->User_ID << " cannot play by billing" << endl;

                filelog(LOGFILE_BILLING_PLAYER, "LoginCheckVerify: Cannot BillingPlay (%s)", pPacket->User_ID);

#ifdef __COUT_BILLING_SYSTEM__
                cout << "LoginCheckVerify: Cannot BillingPlay (" << pPacket->User_ID << ")" << endl;
#endif
            }
        }
    } else {
        filelog(LOGFILE_BILLING_PLAYER, "LoginCheckVerify: No Such Player(%s)", pPacket->User_ID);

#ifdef __COUT_BILLING_SYSTEM__
        cout << "LoginCheckVerify: No Such Player(" << pPacket->User_ID << ")" << endl;
#endif
    }

    __LEAVE_CRITICAL_SECTION((*g_pLoginPlayerManager))

#else
        filelog(LOGFILE_BILLING_PLAYER, "[Error] BillingLoginCheckVerify: Wrong Packet(%s)", pPacket->User_ID);
#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CommonBillingPacketHandler::executeBillingRemain(CommonBillingPacket* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__
        __ENTER_CRITICAL_SECTION((*g_pPCFinder))

            Creature* pCreature = g_pPCFinder->getCreatureByID_LOCKED(pPacket->User_ID);

    if (pCreature != NULL) {
        GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pCreature->getPlayer());

        int result = pPacket->Result;

        switch (result) {
        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        case BILLING_RESULT_REMAIN_DISCONNECT: 
        case BILLING_RESULT_REMAIN_DUPLICATE:  
        case BILLING_RESULT_REMAIN_REFUND:     

            
            
            // pGamePlayer->setPenaltyFlag( PENALTY_TYPE_KICKED );
            
            char session[40];
            memcpy(session, pPacket->Session, 32);
            session[32] = '\0';
            if (pGamePlayer->getBillingSession() == session) {
#ifdef __COUT_BILLING_SYSTEM__
                cout << "***** Disconnect (" << pPacket->User_ID << ":" << session << ")" << endl;
#endif
                disconnectGamePlayer(pGamePlayer, 5 * 10);
                filelog(LOGFILE_BILLING_PLAYER, "BillingRemain: Disconnect(%s:%s), Result=%d", pPacket->User_ID,
                        session, result);
            } else {
#ifdef __COUT_BILLING_SYSTEM__
                cout << "*****  No Such Session(" << pPacket->User_ID << ":" << session << ")" << endl;
#endif

                filelog(LOGFILE_BILLING_PLAYER, "BillingRemain: Disconnect: No Such Session(%s:%s)", pPacket->User_ID,
                        session);
            }
            break;

        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        case BILLING_RESULT_REMAIN_NONE: {
            // char message[40];
            
            // sendSystemMessage(pGamePlayer, message);

            
            pGamePlayer->setBillingUserStatus("");
        } break;

        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        case BILLING_RESULT_REMAIN_RESERVE: 
        {
            char billMethod[10];
            // char expireDate[20];
            memcpy(billMethod, pPacket->Bill_Method, 2);
            billMethod[2] = '\0';
            // memcpy(expireDate, pPacket->Expire_Date, 12); expireDate[12]='\0';

            filelog(LOGFILE_BILLING_PLAYER, "BillingRemain: (%s, %dm)", pPacket->User_ID, (pPacket->Remain_Time / 60));

            
            GCSystemMessage gcSystemMessage;
            gcSystemMessage.setMessage(g_pStringPool->getString(STRID_APPLY_NEW_BILLING_INFO));
            pGamePlayer->sendPacket(&gcSystemMessage);

            
            sendBillingRemainMessage(pPacket, pGamePlayer);

        } break;

        //-----------------------------------------------------------------
        
        //-----------------------------------------------------------------
        case BILLING_RESULT_REMAIN_TIME: {
            char billMethod[10];
            // char expireDate[20];
            memcpy(billMethod, pPacket->Bill_Method, 2);
            billMethod[2] = '\0';
            // memcpy(expireDate, pPacket->Expire_Date, 12); expireDate[12]='\0';

            filelog(LOGFILE_BILLING_PLAYER, "BillingRemain: (%s, %dm)", pPacket->User_ID, (pPacket->Remain_Time / 60));

            int sec = pPacket->Remain_Time;
            int hour = sec / 3600;
            int min = (sec - (hour * 3600)) / 60;

            StringStream msg;
            msg << "     ";

            if (hour > 0) {
                msg << hour << " ";
            }

            msg << min << " .";

            if (strcmp(billMethod, "FM") == 0)
                msg << "()";
            else if (strcmp(billMethod, "FD") == 0)
                msg << "()";
            else if (strcmp(billMethod, "TH") == 0)
                msg << "()";

#ifdef __COUT_BILLING_SYSTEM__
            cout << "[" << pPacket->User_ID << "] " << msg.toString().c_str() << endl;

            //<< billMethod << ", "
            
#endif

            sendSystemMessage(pGamePlayer, msg.toString());
        } break;
        }
    } else {
#ifdef __COUT_BILLING_SYSTEM__
        cout << "BillingRemain: No Such Player(" << pPacket->User_ID << ")" << endl;
#endif
        filelog(LOGFILE_BILLING_PLAYER, "BillingRemain: No Such Player(%s)", pPacket->User_ID);
    }

    __LEAVE_CRITICAL_SECTION((*g_pPCFinder))
#else
        filelog(LOGFILE_BILLING_PLAYER, "[Error] BillingRemain: Wrong Packet(%s)", pPacket->User_ID);
#endif

    __END_DEBUG_EX __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void CommonBillingPacketHandler::executeBillingCheck(CommonBillingPacket* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        BillingPlayer* pBillingPlayer = dynamic_cast<BillingPlayer*>(pPlayer);
    Assert(pBillingPlayer != NULL);

    
    pBillingPlayer->sendPayCheck(pPacket);

#else
        filelog(LOGFILE_BILLING_PLAYER, "[Error] BillingCheck: Wrong Packet(%s)", pPacket->User_ID);
#endif

    __END_DEBUG_EX __END_CATCH
}

void CommonBillingPacketHandler::disconnectGamePlayer(GamePlayer* pGamePlayer, Turn_t delay) {
    __BEGIN_TRY

#ifdef __GAME_SERVER__

    if (pGamePlayer->getEvent(Event::EVENT_CLASS_KICK) != NULL)
        return;

    EventKick* pEventKick = new EventKick(pGamePlayer);

    pEventKick->setDeadline(delay);

    
    pEventKick->sendMessage();

    pGamePlayer->addEvent(pEventKick);

#endif

    __END_CATCH
}

void CommonBillingPacketHandler::sendBillingRemainMessage(CommonBillingPacket* pPacket, GamePlayer* pGamePlayer) {
    __BEGIN_TRY

#ifdef __GAME_SERVER__

    char userStatus[10];
    memcpy(userStatus, pPacket->User_Status, 2);
    userStatus[2] = '\0';

    char billMethod[10];
    memcpy(billMethod, pPacket->Bill_Method, 2);
    billMethod[2] = '\0';

    StringStream msg;
    msg << "    ";

    
    if (strcmp(userStatus, "PM") == 0) {
        msg << "[PC]";
    }
    
    else if (strcmp(billMethod, "FM") == 0 || strcmp(billMethod, "FD") == 0) 
    {
        char expireDate[20];
        memcpy(expireDate, pPacket->Expire_Date, 12);
        expireDate[12] = '\0';
        string sExpireDate(expireDate);

        VSDateTime vsdtExpireDate = VSDateTime::fromYYYYMMDDHHMMSS(sExpireDate);
        VSDateTime currentDateTime = VSDateTime::currentDateTime();
        int day = currentDateTime.daysTo(vsdtExpireDate);
#ifdef __COUT_BILLING_SYSTEM__
        cout << "[" << pPacket->User_ID << "] ExpireDateTime : " << vsdtExpireDate.toString()
             << " CurrentDateTime : " << currentDateTime.toString() << " Remain Days : " << day << endl;
#endif

        // int day = pPacket->Remain_Time;
        if (day == 0 && currentDateTime.date() == vsdtExpireDate.date()) {
            msg << "  ";
        } else {
            msg << " " << (day == 0 ? 1 : day) << " .";
        }

        
        
        if (sExpireDate.size() < 7) {
            filelog(LOGFILE_BILLING_PLAYER, "LoginRemain: (%s, Netmarble PCRoom)", pPacket->User_ID);
            return;
        }

        filelog(LOGFILE_BILLING_PLAYER, "LoginRemain: (%s, %d day)", pPacket->User_ID, day);
    }
    
    else if (strcmp(billMethod, "TH") == 0) 
    {
        
        int sec = pPacket->Remain_Time;
        int hour = sec / 3600;
        int min = (sec - (hour * 3600)) / 60;

        msg << " ";

        if (hour > 0) {
            msg << hour << " ";
        }

        msg << min << " .";

        filelog(LOGFILE_BILLING_PLAYER, "LoginRemain: (%s, %dh %dm)", pPacket->User_ID, hour, min);
    }

#ifdef __COUT_BILLING_SYSTEM__
    cout << "[" << pPacket->User_ID << "] " << msg.toString().c_str() << endl;
#endif

    sendSystemMessage(pGamePlayer, msg.toString());

#endif

    __END_CATCH
}
