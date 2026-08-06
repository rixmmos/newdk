//
// Filename    : ZonePlayerManager.cpp
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "ZonePlayerManager.h"

#include <stdio.h>

#include <algorithm>

#include "Assert.h"
#include "CGLogout.h"
#include "GamePlayer.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "PaySystem.h"
#include "PlayerCreature.h"
#include "Profile.h"
#include "ResurrectLocationManager.h"
#include "Slayer.h"
#include "Socket.h"
#include "SocketAPI.h"
#include "StringStream.h"
#include "Thread.h"
#include "Zone.h"
#include "ZoneInfoManager.h"
#include "ZoneUtil.h"
#include "billing/BillingInfo.h"
// #include "UserGateway.h"
#include "BroadcastFilter.h"
#include "DB.h"
#include "DefaultOptionSetInfo.h"
#include "GCKickMessage.h"
#include "GCSystemMessage.h"
#include "Party.h"
#include "StringPool.h"
#include "VariableManager.h"
#include "ZoneGroup.h"
#include "chinabilling/CBillingInfo.h"

#ifdef __THAILAND_SERVER__

#include "Properties.h"
#include "TimeChecker.h"

#endif

// #define __FULL_PROFILE__

#ifndef __FULL_PROFILE__
#undef beginProfileEx
#define beginProfileEx(name) ((void)0)
#undef endProfileEx
#define endProfileEx(name) ((void)0)
#endif

bool checkZonePlayerManager(GamePlayer* pGamePlayer, ZonePlayerManager* pZPM, const string& str);

//////////////////////////////////////////////////////////////////////////////
// constructor

//////////////////////////////////////////////////////////////////////////////
ZonePlayerManager::ZonePlayerManager()

    : m_MinFD(-1), m_MaxFD(-1) {
    __BEGIN_TRY

    m_Mutex.setName("ZonePlayerManager");
    m_MutexBroadcast.setName("ZonePlayerManagerBroadcast");
    m_PlayerListQueue.clear();
    m_BroadcastQueue.clear();

    
    FD_ZERO(&m_ReadFDs[0]);
    FD_ZERO(&m_WriteFDs[0]);
    FD_ZERO(&m_ExceptFDs[0]);

    
    
    m_Timeout[0].tv_sec = 0;
    m_Timeout[0].tv_usec = 0;
    /*
    #if defined(__THAILAND_SERVER__)

        // add by inthesky for THAILAND ChildGuard rule
        string strChildGuardSwitch = g_pConfig->getProperty("CHILDGUARD");

        cout << "ChildGuard Policy : " << strChildGuardSwitch << endl;

        if(strChildGuardSwitch == "Enable" || strChildGuardSwitch == "ENABLE" || strChildGuardSwitch == "enable")
        {
            m_bChildGuard   = true;
        }
        else m_bChildGuard      = false;

        m_nChildGuardStartTime  = g_pConfig->getPropertyInt("CHILDGUARD_START_TIME");
        m_nChildGuardEndTime    = g_pConfig->getPropertyInt("CHILDGUARD_END_TIME");
        m_nChildGuardKickTime   = g_pConfig->getPropertyInt("CHILDGUARD_REMAIN_TIME");

        m_nChildGuardCheckTerm  = g_pConfig->getPropertyInt("CHILDGUARD_CHECKTERM");
        getCurrentTime(m_tmChildGuardCheckTerm);
        m_tmChildGuardCheckTerm.tv_sec += m_nChildGuardCheckTerm;

        cout << "ChildGuard TimeArea : "<<(int)m_nChildGuardStartTime << " - " << (int)m_nChildGuardEndTime << endl;
        cout << "ChildGuard CheckTerm : "<<(int)m_nChildGuardCheckTerm << "/sec" << endl;

    #endif
    */
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////
ZonePlayerManager::~ZonePlayerManager()

{
    __BEGIN_TRY

    

    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::broadcastPacket(Packet* pPacket)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    PlayerManager::broadcastPacket(pPacket);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::broadcastPacket_NOBLOCKED(Packet* pPacket)

{
    __BEGIN_TRY

    PlayerManager::broadcastPacket(pPacket);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::pushBroadcastPacket(Packet* pPacket, BroadcastFilter* pFilter)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_MutexBroadcast)

    
    
    

    
    
    
    SocketOutputStream* pStream = new SocketOutputStream(NULL, szPacketHeader + pPacket->getPacketSize());
    pPacket->writeHeaderNBody(*pStream);

    m_BroadcastQueue.push_back(PairFilterStream(pFilter->Clone(), pStream));

    __LEAVE_CRITICAL_SECTION(m_MutexBroadcast)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::flushBroadcastPacket()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_MutexBroadcast)

    list<PairFilterStream>::iterator itr = m_BroadcastQueue.begin();
    list<PairFilterStream>::iterator endItr = m_BroadcastQueue.end();

    for (; itr != endItr; ++itr) {
        BroadcastFilter* pFilter = itr->first;
        SocketOutputStream* pStream = itr->second;

        if (pStream == NULL) {
            filelog("ZoneBug.txt", "%s : %s", "Zone::flushBroadcastPacket", "pStream NULL.");
            continue;
        }

        for (uint i = 0; i < nMaxPlayers; ++i) {
            if (m_pPlayers[i] != NULL) {
                GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                if (pFilter == NULL || pFilter->isSatisfy(pGamePlayer)) {
                    try {
                        pGamePlayer->sendStream(pStream);
                    } catch (Throwable& t) {
                        filelog("ZonePlayerManager.log", "broadcastPacket: %s", t.toString().c_str());
                    }
                }
            }
        }
    }

     
    m_BroadcastQueue.clear();

    __LEAVE_CRITICAL_SECTION(m_MutexBroadcast)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::copyPlayers()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    PlayerManager::copyPlayers();

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// call select() system call

//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::select() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    
    m_Timeout[1].tv_sec = m_Timeout[0].tv_sec;
    m_Timeout[1].tv_usec = m_Timeout[0].tv_usec;

    
    m_ReadFDs[1] = m_ReadFDs[0];
    m_WriteFDs[1] = m_WriteFDs[0];
    m_ExceptFDs[1] = m_ExceptFDs[0];

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    try {
        
        SocketAPI::select_ex(m_MaxFD + 1, &m_ReadFDs[1], &m_WriteFDs[1], &m_ExceptFDs[1], &m_Timeout[1]);
    }
    
    // catch (TimeoutException&)
    //{
    // do nothing
    //}
    catch (InterruptedException& ie) {
        
        log(LOG_GAMESERVER_ERROR, "", "", ie.toString());
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// process all players' inputs
//



//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::processInputs() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        
        if (FD_ISSET(i, &m_ReadFDs[1])) {
            if (m_pPlayers[i] != NULL && m_pPlayers[i] == m_pPlayers[i]) {
                GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                Assert(pTempPlayer != NULL);
                Assert(m_pPlayers[i] != NULL);

                if (g_pVariableManager->getVariable(PCROOM_ITEM_RATIO_BONUS) == 100 &&
                    !checkZonePlayerManager(pTempPlayer, this, "PI")) {
                    try {
                        CGLogoutHandler::execute(NULL, pTempPlayer);
                    } catch (DisconnectException& de) {
                        deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                        pushOutPlayer(pTempPlayer);
                    }
                } else if (pTempPlayer->getSocket()->getSockError()) {
                    pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                    pTempPlayer->setItemRatioBonusPoint(7);

                    try {
                        CGLogoutHandler::execute(NULL, pTempPlayer);
                    } catch (DisconnectException& de) {
                        filelog("DIFF_ZG.log", "%s ZPM+PI+SOCKERR", de.toString().c_str());
                        deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                        pushOutPlayer(pTempPlayer);
                    }

                    // by sigi. 2002.12.30
                    //					UserGateway::getInstance()->passUser( UserGateway::USER_OUT_ZPM_INPUT_ERROR );

                     
                } else {
                    try {
                        pTempPlayer->processInput();
                    } catch (ConnectException& ce) {
                        pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                        pTempPlayer->setItemRatioBonusPoint(8);

                        try {
                            CGLogoutHandler::execute(NULL, pTempPlayer);
                        } catch (DisconnectException& de) {
                            filelog("DIFF_ZG.log", "%s ZPM+PI+CE", de.toString().c_str());
                            deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                            pushOutPlayer(pTempPlayer);
                        }

                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_ZPM_INPUT_DISCONNECT );

                         
                    } catch (IOException& ioe) {
                        pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                        pTempPlayer->setItemRatioBonusPoint(9);

                        try {
                            CGLogoutHandler::execute(NULL, pTempPlayer);
                        } catch (DisconnectException& de) {
                            filelog("DIFF_ZG.log", "%s ZPM+PI+IOE", de.toString().c_str());
                            deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                            pushOutPlayer(pTempPlayer);
                        }

                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_ZPM_INPUT_DISCONNECT2 );

                         
                    }
                }
            }
        }
    }

    //	__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// process all players' commands
//////////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::processCommands() {
    __BEGIN_TRY
    __BEGIN_DEBUG

    // test code
    /*
    for (int i=0; i<100; i++)
    {
        PaySystem ps;

        try {
            ps.loginPayPlay("111.111.222.333", "sdfdf");
            cout << "[" << (int)(long)Thread::self() << "] " << i << endl;
        } catch (Throwable&t)
        {
            cout << t.toString().c_str() << endl;
        }
    }
    */

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    VSDateTime currentDateTime(VSDate::currentDate(), VSTime::currentTime());

    Timeval currentTime;
    getCurrentTime(currentTime);

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL) {
            GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
            Assert(pTempPlayer);
            Assert(m_pPlayers[i] != NULL);

            if (pTempPlayer->getSocket()->getSockError()) {
                pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                pTempPlayer->setItemRatioBonusPoint(10);

                try {
                    CGLogoutHandler::execute(NULL, pTempPlayer);
                } catch (DisconnectException& de) {
                    filelog("DIFF_ZG.log", "%s ZPM+PC+SOCKERR", de.toString().c_str());
                    deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                    pushOutPlayer(pTempPlayer);
                }

                // by sigi. 2002.12.30
                //				UserGateway::getInstance()->passUser( UserGateway::USER_OUT_ZPM_COMMAND_ERROR );

                 
            } else {
                bool IsPayPlayEnd = false;

                try {
                    beginProfileEx("ZPM_PACKET");
                    pTempPlayer->processCommand();
                    endProfileEx("ZPM_PACKET");

                    if (g_pVariableManager->getVariable(PCROOM_ITEM_RATIO_BONUS) == 100 &&
                        !checkZonePlayerManager(pTempPlayer, this, "PC")) {
                        try {
                            CGLogoutHandler::execute(NULL, pTempPlayer);
                        } catch (DisconnectException& de) {
                            deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                            pushOutPlayer(pTempPlayer);
                        }
                    }

                    
                    
                    
                    // by sigi. 2002.12.10
                    else if (pTempPlayer->getPlayerStatus() == GPS_NORMAL) {
#ifdef __CONNECT_BILLING_SYSTEM__
                        
                        if (!pTempPlayer->isBillingLoginVerified()) {
                            if (!pTempPlayer->isMetroFreePlayer())
                                pTempPlayer->sendBillingLogin();
                        }
                        
                        
                        
                        else if (pTempPlayer->isBillingLoginVerified() && !pTempPlayer->isBillingPlayAvaiable())
#endif

                            
                            if (pTempPlayer->isFamilyFreePassEnd()) {
                                Creature* pCreature = pTempPlayer->getCreature();
                                Zone* pZone = pCreature->getZone();
                                Assert(pZone != NULL);

                                if (pZone->isPayPlay()) {
                                    
                                    pTempPlayer->setPremiumPlay();
                                }
                            }

                        
                        
                        if ((pTempPlayer->isPayPlaying() || pTempPlayer->isPremiumPlay() ||
                             pTempPlayer->isFamilyPayAvailable()) &&
                            !pTempPlayer->updatePayPlayTime(pTempPlayer->getID(), currentDateTime, currentTime)) {
                            Creature* pCreature = pTempPlayer->getCreature();
                            Zone* pZone = pCreature->getZone();
                            Assert(pZone != NULL);

                            
                            pTempPlayer->logoutPayPlay(pTempPlayer->getID());

                            
                            
                            if (pTempPlayer->isFamilyPayAvailable()) {
                                pTempPlayer->setFamilyPayAvailable(false);

                                
                                int PartyID = pCreature->getPartyID();
                                if (PartyID != 0) {
                                    g_pGlobalPartyManager->refreshFamilyPay(PartyID);
                                }

                                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                                Assert(pPC != NULL);

                                pPC->removeDefaultOptionSet(DEFAULT_OPTION_SET_FAMILY_PAY);
                                pPC->setFlag(Effect::EFFECT_CLASS_INIT_ALL_STAT);
                            }

                            // by sigi. 2002.12.30
                            IsPayPlayEnd = true;


#if defined(__PAY_SYSTEM_ZONE__)

                            
                            //
                            
                            
                            if (pZone->isPayPlay()) {
                                ZONE_COORD zoneCoord;

                                Assert(pCreature->isPC());
                                PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                                Assert(pPC != NULL);

                                
                                
                                
                                Statement* pStmt = NULL;

                                BEGIN_DB {
                                    uint strID = STRID_END_PAY_PLAY;

                                    
                                    if (pTempPlayer->isFamilyFreePassEnd())
                                        strID = STRID_FAMILY_FREE_PLAY_END;

                                    pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
                                    pStmt->executeQuery(
                                        "INSERT INTO Messages ( Receiver, Message ) VALUES ( '%s', '%s')",
                                        pPC->getName().c_str(), g_pStringPool->c_str(strID));

                                    if (pCreature->isFlag(Effect::EFFECT_CLASS_LOGIN_GUILD_MESSAGE))
                                        pCreature->removeFlag(Effect::EFFECT_CLASS_LOGIN_GUILD_MESSAGE);

                                    SAFE_DELETE(pStmt);
                                }
                                END_DB(pStmt)

                                
                                if (g_pResurrectLocationManager->getRaceDefaultPosition(pPC->getRace(), zoneCoord)) {
                                    transportCreature(pCreature, zoneCoord.id, zoneCoord.x, zoneCoord.y, true);
                                } else {
                                    
                                    throw Error("Critical Error : ResurrectInfo is not established!1");
                                }
                            }
#elif defined(__PAY_SYSTEM_FREE_LIMIT__) || defined(__PAY_SYSTEM_LOGIN__)
                            // cout << "Pay timeout" << endl;
                            // throw DisconnectException();

                            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                            Assert(pPC != NULL);

                            if (pPC->isPayPlayAvaiable()) {
                                
                                if (pZone->isPayPlay()) {
                                    ZONE_COORD zoneCoord;

                                    Assert(pCreature->isPC());
                                    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
                                    Assert(pPC != NULL);

                                    if (g_pResurrectLocationManager->getRaceDefaultPosition(pPC->getRace(),
                                                                                            zoneCoord)) {
                                        transportCreature(pCreature, zoneCoord.id, zoneCoord.x, zoneCoord.y, true);
                                    } else {
                                        
                                        throw Error("Critical Error : ResurrectInfo is not established!1");
                                    }
                                } else {
                                    GCSystemMessage gcSystemMessage;
                                    gcSystemMessage.setMessage(g_pStringPool->getString(STRID_EXPIRE_PREMIUM_SERVICE));
                                    pTempPlayer->sendPacket(&gcSystemMessage);
                                }
                            } else {
                                pTempPlayer->kickPlayer(30, KICK_MESSAGE_PAY_TIMEOUT);
                                 
                            }

#endif
                        }

                         
#ifdef __THAILAND_SERVER__
                        // child guard check !
                        if (!pTempPlayer->getPermission() && g_pTimeChecker->isInPeriod(TIME_PERIOD_CHILD_GUARD)) {
                            pTempPlayer->kickPlayer(30, KICK_MESSAGE_CHILDGUARD);
                        }
#endif

                        
                        if (pTempPlayer->isFamilyFreePassEnd()) {
                            pTempPlayer->setFamilyPayPartyType(FAMILY_PAY_PARTY_TYPE_NONE);
                        }

#ifdef __CONNECT_CBILLING_SYSTEM__
                        if (!pTempPlayer->isPayPlayer()) {
                            
                            pTempPlayer->kickPlayer(30, KICK_MESSAGE_PAY_TIMEOUT);
                        } else {
                            
                            if (pTempPlayer->isCBillingVerified() && !pTempPlayer->isCBShowPayInfo()) {
                                pTempPlayer->sendCBillingPayInfo();
                                pTempPlayer->setCBShowPayInfo();
                            }
                        }
#endif
                    }
                } catch (ProtocolException& pe) {
                    pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                    pTempPlayer->setItemRatioBonusPoint(11);

                    try {
                        CGLogoutHandler::execute(NULL, pTempPlayer);
                    } catch (DisconnectException& de) {
                        filelog("DIFF_ZG.log", "%s ZPM+PC+PE", de.toString().c_str());
                        deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                        pushOutPlayer(pTempPlayer);
                    }

                    // by sigi. 2002.12.30
                    if (IsPayPlayEnd) {
                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_ZPM_COMMAND_PAYPLAY_END );
                    } else {
                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_ZPM_COMMAND_ERROR );
                    }

                     
                }
            }
        }
    }

    //	__ENTER_CRITICAL_SECTION(m_Mutex)
     


    //	__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_DEBUG
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// process all players' outputs
//
//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::processOutputs() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }

    // copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_WriteFDs[1])) {
            if (m_pPlayers[i] != NULL) {
                GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                Assert(pTempPlayer);
                Assert(m_pPlayers[i] != NULL);

                if (pTempPlayer->getSocket()->getSockError()) {
                    pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                    pTempPlayer->setItemRatioBonusPoint(12);

                    try {
                        CGLogoutHandler::execute(NULL, pTempPlayer);
                    } catch (DisconnectException& de) {
                        filelog("DIFF_ZG.log", "%s ZPM+PO+SOCKERR", de.toString().c_str());
                        deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                        pushOutPlayer(pTempPlayer);
                    }

                    // by sigi. 2002.12.30
                    //					UserGateway::getInstance()->passUser( UserGateway::USER_OUT_ZPM_OUTPUT_ERROR );
                     
                } else {
                    try {
                        pTempPlayer->processOutput();
                    } catch (ConnectException& ce) {
                        pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                        pTempPlayer->setItemRatioBonusPoint(13);

                        try {
                            CGLogoutHandler::execute(NULL, pTempPlayer);
                        } catch (DisconnectException& de) {
                            filelog("DIFF_ZG.log", "%s ZPM+PO+CE", de.toString().c_str());
                            deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                            pushOutPlayer(pTempPlayer);
                        }

                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_ZPM_OUTPUT_DISCONNECT );

                         
                    } catch (ProtocolException& cp) {
                        pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                        pTempPlayer->setItemRatioBonusPoint(14);

                        try {
                            CGLogoutHandler::execute(NULL, pTempPlayer);
                        } catch (DisconnectException& de) {
                            filelog("DIFF_ZG.log", "%s ZPM+PO+PE", de.toString().c_str());
                            deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                            pushOutPlayer(pTempPlayer);
                        }

                        // by sigi. 2002.12.30
                        //						UserGateway::getInstance()->passUser(
                        // UserGateway::USER_OUT_ZPM_OUTPUT_DISCONNECT2 );

                         
                    }
                }
            }
        }
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// process all players' exceptions
//


//
//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::processExceptions() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)

    if (m_MinFD == -1 && m_MaxFD == -1) // no player exist
    {
        // m_Mutex.unlock();
        return;
    }
    //	copyPlayers();

    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (FD_ISSET(i, &m_ExceptFDs[1])) {
            if (m_pPlayers[i] != NULL && m_pPlayers[i] == m_pPlayers[i]) {
                GamePlayer* pTempPlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);
                Assert(pTempPlayer != NULL);
                Assert(m_pPlayers[i] != NULL);

                pTempPlayer->setPenaltyFlag(PENALTY_TYPE_KICKED);
                pTempPlayer->setItemRatioBonusPoint(15);

                try {
                    CGLogoutHandler::execute(NULL, pTempPlayer);
                } catch (DisconnectException& de) {
                    filelog("DIFF_ZG.log", "%s ZPM+PE", de.toString().c_str());
                    deletePlayer(pTempPlayer->getSocket()->getSOCKET());
                    pushOutPlayer(pTempPlayer);
                }

                // by sigi. 2002.12.30
                //				UserGateway::getInstance()->passUser( UserGateway::USER_OUT_ZPM_EXCEPTION );

                 
            }
        }
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::addPlayer(GamePlayer* pGamePlayer) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    PlayerManager::addPlayer(pGamePlayer);


    SOCKET fd = pGamePlayer->getSocket()->getSOCKET();

    
    if (m_MinFD == -1 && m_MaxFD == -1) {
        
        m_MinFD = m_MaxFD = fd;
    } else {
        m_MinFD = min(fd, m_MinFD);
        m_MaxFD = max(fd, m_MaxFD);
    }

    
    
    FD_SET(fd, &m_ReadFDs[0]);
    FD_SET(fd, &m_WriteFDs[0]);
    FD_SET(fd, &m_ExceptFDs[0]);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::addPlayer_NOBLOCKED(GamePlayer* pGamePlayer) {
    __BEGIN_TRY

    PlayerManager::addPlayer(pGamePlayer);


    SOCKET fd = pGamePlayer->getSocket()->getSOCKET();

    
    if (m_MinFD == -1 && m_MaxFD == -1) {
        
        m_MinFD = m_MaxFD = fd;
    } else {
        m_MinFD = min(fd, m_MinFD);
        m_MaxFD = max(fd, m_MaxFD);
    }

    
    
    FD_SET(fd, &m_ReadFDs[0]);
    FD_SET(fd, &m_WriteFDs[0]);
    FD_SET(fd, &m_ExceptFDs[0]);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::deletePlayer_NOBLOCKED(SOCKET fd) {
    __BEGIN_TRY

    
    PlayerManager::deletePlayer(fd);

    Assert(m_pPlayers[fd] == NULL);

    
    
    if (fd == m_MinFD) {
        
        
        int i = m_MinFD;
        for (i = m_MinFD; i <= m_MaxFD; i++) {
            if (m_pPlayers[i] != NULL) {
                m_MinFD = i;
                break;
            }
        }

        
        
        
        if (i > m_MaxFD)
            m_MinFD = m_MaxFD = -1;

    } else if (fd == m_MaxFD) {
        
        int i = m_MaxFD;
        for (i = m_MaxFD; i >= m_MinFD; i--) {
            if (m_pPlayers[i] != NULL) {
                m_MaxFD = i;
                break;
            }
        }

        
        if (i < m_MinFD) {
            filelog("ZonePlayerManagerBug.txt", "%s : %s", "ZonePlayerManager::deletePlayer_NOBLOCKED()",
                    "MinMaxFD problem");
            throw UnknownError("m_MinFD & m_MaxFD problem.");
        }
    }

    
    
    
    FD_CLR(fd, &m_ReadFDs[0]);
    FD_CLR(fd, &m_ReadFDs[1]);
    FD_CLR(fd, &m_WriteFDs[0]);
    FD_CLR(fd, &m_WriteFDs[1]);
    FD_CLR(fd, &m_ExceptFDs[0]);
    FD_CLR(fd, &m_ExceptFDs[1]);


    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::deletePlayer(SOCKET fd) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    try {
        
        PlayerManager::deletePlayer(fd);
    } catch (OutOfBoundException& o) {
        filelog("ZPMError.txt", "OOB: %s, Socket: %d", o.toString().c_str(), fd);
        throw;
    } catch (NoSuchElementException& n) {
        filelog("ZPMError.txt", "NSEE: %s, Socket: %d", n.toString().c_str(), fd);
        throw;
    } catch (Error& e) {
        filelog("ZPMError.txt", "Error: %s, Socket: %d", e.toString().c_str(), fd);
        throw;
    } catch (...) {
        filelog("ZPMError.txt", " . Socket: %d", fd);
        throw;
    }

    Assert(m_pPlayers[fd] == NULL);

    
    
    if (fd == m_MinFD) {
        
        
        int i = m_MinFD;
        for (i = m_MinFD; i <= m_MaxFD; i++) {
            if (m_pPlayers[i] != NULL) {
                m_MinFD = i;
                break;
            }
        }

        
        
        
        if (i > m_MaxFD)
            m_MinFD = m_MaxFD = -1;

    } else if (fd == m_MaxFD) {
        
        int i = m_MaxFD;
        for (i = m_MaxFD; i >= m_MinFD; i--) {
            if (m_pPlayers[i] != NULL) {
                m_MaxFD = i;
                break;
            }
        }

        
        if (i < m_MinFD) {
            filelog("ZonePlayerManagerBug.txt", "%s : %s", "ZonePlayerManager::deletePlayer()", "MinMaxFD problem");
            throw UnknownError("m_MinFD & m_MaxFD problem.");
        }
    }

    
    
    
    FD_CLR(fd, &m_ReadFDs[0]);
    FD_CLR(fd, &m_ReadFDs[1]);
    FD_CLR(fd, &m_WriteFDs[0]);
    FD_CLR(fd, &m_WriteFDs[1]);
    FD_CLR(fd, &m_ExceptFDs[0]);
    FD_CLR(fd, &m_ExceptFDs[1]);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
Player* ZonePlayerManager::getPlayer(SOCKET fd) {
    __BEGIN_TRY

    Player* pPlayer = NULL;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    pPlayer = PlayerManager::getPlayer(fd);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return pPlayer;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
Player* ZonePlayerManager::getPlayerByPhoneNumber(PhoneNumber_t PhoneNumber) {
    __BEGIN_TRY
    return NULL;
    __END_CATCH
}

//////////////////////////////////////////////////////////////////////




// ZonePlayerManager:: save()
//////////////////////////////////////////////////////////////////////
void ZonePlayerManager::save()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    unsigned int i;

    for (i = 0; i < nMaxPlayers; i++) {
        if (m_pPlayers[i] != NULL) {
            Assert(m_pPlayers[i] != NULL); // by sigi

            Creature* pCreature = ((GamePlayer*)m_pPlayers[i])->getCreature();

            Assert(pCreature != NULL); // by sigi

            pCreature->save();
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void ZonePlayerManager::pushPlayer(GamePlayer* pGamePlayer)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    m_PlayerListQueue.push_back(pGamePlayer);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void ZonePlayerManager::pushOutPlayer(GamePlayer* pGamePlayer)

{
    __BEGIN_TRY

    m_PlayerOutListQueue.push_back(pGamePlayer);

    __END_CATCH
}

void ZonePlayerManager::processPlayerListQueue()

{
    __BEGIN_TRY

    
    while (!m_PlayerListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerListQueue.front();

        if (pGamePlayer == NULL) {
            filelog("ZoneBug.txt", "%s : %s", "Zone::heartbeat(1)", "pGamePlayer NULL.");
            continue;
        }

        addPlayer_NOBLOCKED(pGamePlayer);

        m_PlayerListQueue.pop_front();

        Creature* pCreature = pGamePlayer->getCreature();

        Assert(pCreature != NULL);

        
        Zone* pZone = pCreature->getZone();
        Assert(pZone != NULL);

         

        pZone->addPC(pCreature, pCreature->getX(), pCreature->getY(), DOWN);
    }

    __END_CATCH
}

void ZonePlayerManager::heartbeat()

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    processPlayerListQueue();

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    
    
    while (!m_PlayerOutListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerOutListQueue.front();

        m_PlayerOutListQueue.pop_front();

        Assert(pGamePlayer != NULL);

        g_pIncomingPlayerManager->pushPlayer(pGamePlayer);
    }

    
    if (!m_BroadcastQueue.empty())
        flushBroadcastPacket();

    __END_CATCH
}

void ZonePlayerManager::deleteQueuePlayer(GamePlayer* pGamePlayer) {
    __BEGIN_TRY

    
    
    //__ENTER_CRITICAL_SECTION(m_Mutex)

    Assert(pGamePlayer != NULL);

    list<GamePlayer*>::iterator itr =
        find_if(m_PlayerOutListQueue.begin(), m_PlayerOutListQueue.end(), isSamePlayer(pGamePlayer));

    if (itr != m_PlayerOutListQueue.end()) {
        m_PlayerOutListQueue.erase(itr);
    }

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

void ZonePlayerManager::removeFlag(Effect::EffectClass EC)

{
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    for (uint i = 0; i < nMaxPlayers; i++) {
        Player* pPlayer = m_pPlayers[i];

        if (pPlayer != NULL) {
            GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
            Assert(pGamePlayer != NULL);

            Creature* pCreature = pGamePlayer->getCreature();
            Assert(pCreature != NULL);

            pCreature->removeFlag(EC);
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}

////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
void ZonePlayerManager::clearPlayers()

{
    __BEGIN_TRY

    
    while (!m_PlayerListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerListQueue.front();

        m_PlayerListQueue.pop_front();

        if (pGamePlayer != NULL) {
            try {
                pGamePlayer->disconnect();
            } catch (Throwable& t) {
                
            }

            SAFE_DELETE(pGamePlayer);
        }
    }

    
    while (!m_PlayerOutListQueue.empty()) {
        GamePlayer* pGamePlayer = m_PlayerOutListQueue.front();

        m_PlayerOutListQueue.pop_front();

        if (pGamePlayer != NULL) {
            try {
                pGamePlayer->disconnect();
            } catch (Throwable& t) {
                
            }

            SAFE_DELETE(pGamePlayer);
        }
    }

    if (m_MinFD == -1 && m_MaxFD == -1)
        return;

    
    for (int i = m_MinFD; i <= m_MaxFD; i++) {
        if (m_pPlayers[i] != NULL) {
            GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(m_pPlayers[i]);

            if (pGamePlayer != NULL) {
                try {
                    pGamePlayer->disconnect();
                } catch (Throwable& t) {
                    
                }

                SAFE_DELETE(pGamePlayer);
            }
        }
    }

    __END_CATCH
}

bool checkZonePlayerManager(GamePlayer* pGamePlayer, ZonePlayerManager* pZPM, const string& str) {
    if (pGamePlayer == NULL)
        return true;

    Creature* pCreature = pGamePlayer->getCreature();
    if (pCreature == NULL)
        return true;

    Zone* pZone = pCreature->getZone();
    if (pZone == NULL)
        return true;

    ZoneGroup* pZoneGroup = pZone->getZoneGroup();
    if (pZoneGroup == NULL)
        return true;

    ZonePlayerManager* pZonePlayerManager = pZoneGroup->getZonePlayerManager();
    if (pZonePlayerManager == NULL)
        return true;

    if (pZPM != pZonePlayerManager) {
        filelog("ZPMCheck.log", "CZPM:%u GZPM:%u SOCK:%d ZID:%u NAME:%s P:%s", pZPM->getZGID(),
                pZonePlayerManager->getZGID(), pGamePlayer->getSocket()->getSOCKET(), pZone->getZoneID(),
                pCreature->getName().c_str(), str.c_str());

        return false;
    }

    return true;
}
#if defined(__THAILAND_SERVER__)


//


//


//
//


bool ZonePlayerManager::onChildGuardTimeArea(int pm, int am, bool bSwitch) {
    bool returnValue = false;
    tm Timem;
    time_t daytime = time(0);
    localtime_r(&daytime, &Timem);

    int Hour = Timem.tm_hour;
    int Min = Timem.tm_min;

    int timeValue = (Hour * 100) + Min;

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

// external variable definition
ZonePlayerManager* g_pZonePlayerManager = NULL;
