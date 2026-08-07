//////////////////////////////////////////////////////////////////////////////
// Filename    : CGConnectHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGConnect.h"

#ifdef __GAME_SERVER__
#include <stdio.h>

#include <fstream>
#include <list>

#include "BillingPlayerManager.h"
#include "CastleInfoManager.h"
#include "CreatureUtil.h"
#include "DB.h"
#include "EffectGhost.h"
#include "Encrypter.h"
#include "EventHeadCount.h"
#include "EventRegeneration.h"
#include "EventSave.h"
#include "EventSystemMessage.h"
#include "EventZoneInfo.h"
#include "GCDisconnect.h"
#include "GCSystemMessage.h"
#include "GCUpdateInfo.h"
#include "GSGuildMemberLogOn.h"
#include "GamePlayer.h"
#include "Guild.h"
#include "GuildManager.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "LogDef.h"
#include "NPCInfo.h"
#include "NicknameBook.h"
#include "Ousters.h"
#include "PCFinder.h"
#include "PCSlayerInfo2.h"
#include "PCVampireInfo2.h"
#include "PKZoneInfoManager.h"
#include "PacketUtil.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "ResurrectLocationManager.h"
#include "SharedServerManager.h"
#include "Slayer.h"
#include "TelephoneCenter.h"
#include "Thread.h"
#include "Vampire.h"
#include "WeatherManager.h"
#include "Zone.h"
#include "ZoneGroup.h"
#include "ZoneGroupManager.h"
#include "ZoneInfoManager.h"
#include "ZonePlayerManager.h"
#include "ZoneUtil.h"
#include "mission/QuestManager.h"
#include "skill/EffectGnomesWhisper.h"
// #include "GCLoadInventory.h"
#include "GDRLairManager.h"
#include "SystemAvailabilitiesManager.h"
#include "chinabilling/CBillingInfo.h"
#include "types/ServerType.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
#include "EventCBilling.h"
#include "chinabilling/CBillingPlayerManager.h"
#endif

#include "DynamicZoneManager.h"
#include "EventAuth.h"

#endif

bool isAdultByBirthdayDate(const string& birthday);


//////////////////////////////////////////////////////////////////////////////
// CGConnectHandler::execute()
//




//



//

//////////////////////////////////////////////////////////////////////////////
void CGConnectHandler::execute(CGConnect* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX __BEGIN_DEBUG

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    FILELOG_INCOMING_CONNECTION("cgconnect_trace.log", "begin host=%s key=%u pc=%s",
                                pGamePlayer->getSocket()->getHost().c_str(), pPacket->getKey(),
                                pPacket->getPCName().c_str());

    // set MAC Address
    pGamePlayer->setMacAddress(pPacket->getMacAddress());

    
    
    try {
        ConnectionInfo* pConnectionInfo =
            g_pConnectionInfoManager->getConnectionInfo(pGamePlayer->getSocket()->getHost());
        FILELOG_INCOMING_CONNECTION("cgconnect_trace.log", "found connection info host=%s ci_ip=%s ci_key=%u ci_pc=%s",
                                    pGamePlayer->getSocket()->getHost().c_str(),
                                    pConnectionInfo->getClientIP().c_str(), pConnectionInfo->getKey(),
                                    pConnectionInfo->getPCName().c_str());

        
        if (pPacket->getKey() != pConnectionInfo->getKey()) {
            FILELOG_INCOMING_CONNECTION("connectionError.log", "Wrong Key: [%s] %s",
                                        pConnectionInfo->getPCName().c_str(),
                                        pGamePlayer->getSocket()->getHost().c_str());
            throw InvalidProtocolException("invalid key");
        }

        
        if (pPacket->getPCName() != pConnectionInfo->getPCName()) {
            FILELOG_INCOMING_CONNECTION("connectionError.log", "Wrong PCName: [%s] %s",
                                        pConnectionInfo->getPCName().c_str(),
                                        pGamePlayer->getSocket()->getHost().c_str());
            throw InvalidProtocolException("invalid pc name");
        }

        
        pGamePlayer->setID(pConnectionInfo->getPlayerID());

        
        
        
        Timeval currentTime;
        getCurrentTime(currentTime);
        if (pConnectionInfo->getExpireTime() < currentTime) {
            FILELOG_INCOMING_CONNECTION("connectionError.log", "Expired: [%s] %s", pConnectionInfo->getPCName().c_str(),
                                        pGamePlayer->getSocket()->getHost().c_str());

            
            g_pConnectionInfoManager->deleteConnectionInfo(pConnectionInfo->getClientIP());
            throw InvalidProtocolException("session already expired");
        }

        // by sigi. 2002.12.7
        FILELOG_INCOMING_CONNECTION("connectionInfo.log", "Login [%s:%s] %s (%u)",
                                    pConnectionInfo->getPlayerID().c_str(), pConnectionInfo->getPCName().c_str(),
                                    pConnectionInfo->getClientIP().c_str(), pConnectionInfo->getKey());

        
        try {
            g_pConnectionInfoManager->deleteConnectionInfo(pConnectionInfo->getClientIP());
        } catch (NoSuchElementException& nsee) {
            FILELOG_INCOMING_CONNECTION("connectionInfoDelete.log", "DeleteNoSuch [%s:%s] %s (%u)",
                                        pConnectionInfo->getPlayerID().c_str(), pConnectionInfo->getPCName().c_str(),
                                        pConnectionInfo->getClientIP().c_str(), pConnectionInfo->getKey());
        }
    } catch (NoSuchElementException& nsee) 
    {
        FILELOG_INCOMING_CONNECTION("cgconnect_trace.log", "no connection info for host=%s",
                                    pGamePlayer->getSocket()->getHost().c_str());
        FILELOG_INCOMING_CONNECTION("connectionError.log", "NoSuchConnectionInfo: %s",
                                    pGamePlayer->getSocket()->getHost().c_str());

        
        
        
        
        GCDisconnect gcDisconnect;
        gcDisconnect.setMessage(nsee.toString());

        pGamePlayer->sendPacket(&gcDisconnect);

        
        throw InvalidProtocolException(nsee.toString().c_str());
    } catch (InvalidProtocolException& ipe) {
        FILELOG_INCOMING_CONNECTION("cgconnect_trace.log", "invalid protocol host=%s reason=%s",
                                    pGamePlayer->getSocket()->getHost().c_str(), ipe.toString().c_str());
        FILELOG_INCOMING_CONNECTION("connectionError.log", "%s: %s", ipe.toString().c_str(),
                                    pGamePlayer->getSocket()->getHost().c_str());
        cout << endl
             << "+-----------------------+" << endl
             << "| Level 2 Access Denied |" << endl
             << "+-----------------------+" << endl
             << endl;

        GCDisconnect gcDisconnect;
        gcDisconnect.setMessage(ipe.toString());

        pGamePlayer->sendPacket(&gcDisconnect);

        
        throw;
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    FILELOG_INCOMING_CONNECTION("cgconnect_trace.log", "connection validated host=%s playerID=%s",
                                pGamePlayer->getSocket()->getHost().c_str(), pGamePlayer->getID().c_str());

    Statement* pStmt = NULL;
    Result* pResult = NULL;

    
    PayType payType;
    string payPlayDate;
    uint payPlayHours;
    uint payPlayFlag;
    int billingUserKey = 0; // by sigi. 2002.11.18
    string familyPayPlayDate;


    try {


        PreparedStatement nameStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                   "SELECT PlayerID,Race FROM Slayer WHERE Name = ?");
        nameStmt.bindString(1, pPacket->getPCName());
        pResult = nameStmt.execute();


        if (pResult->getRowCount() != 1) {
            StringStream msg;
            msg << "Failed to load PlayerCreature data from DB. Not 1 PlayerID (" << pPacket->getPCName().c_str()
                << ")";

            filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

            throw ProtocolException(msg.toString().c_str());
        }

        if (pResult->next()) {
            string spID = pResult->getString(1);
            string spRace = pResult->getString(2);

            if (spRace == "SLAYER") {
                pPacket->setPCType(PC_SLAYER);
            } else if (spRace == "VAMPIRE") {
                pPacket->setPCType(PC_VAMPIRE);
            } else if (spRace == "OUSTERS") {
                pPacket->setPCType(PC_OUSTERS);
            } else {
                StringStream msg;
                msg << "Failed to load PlayerCreature data from DB. No Character(" << spID.c_str()
                    << "!=" << pGamePlayer->getID().c_str() << ")";

                filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

                throw ProtocolException(msg.toString().c_str());
            }


            if (strcasecmp(spID.c_str(), pGamePlayer->getID().c_str()) != 0) {
                StringStream msg;
                msg << "Failed to load PlayerCreature data from DB. No Character(" << spID.c_str()
                    << "!=" << pGamePlayer->getID().c_str() << ")";

                filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

                throw ProtocolException(msg.toString().c_str());
            }
        }

#ifdef __THAILAND_SERVER__

        PreparedStatement playerStmt(g_pDatabaseManager->getDistConnection("PLAYER_DB"),
                                     "SELECT PlayerID, CurrentServerGroupID, LogOn, SpecialEventCount, PayType, "
                                     "PayPlayDate, PayPlayHours, PayPlayFlag, BillingUserKey, FamilyPayPlayDate, "
                                     "Birthday FROM Player WHERE PlayerID = ?");
        playerStmt.bindString(1, pGamePlayer->getID());
        pResult = playerStmt.execute();

#else

        PreparedStatement playerStmt(g_pDatabaseManager->getDistConnection("PLAYER_DB"),
                                     "SELECT PlayerID, CurrentServerGroupID, LogOn, SpecialEventCount, PayType, "
                                     "PayPlayDate, PayPlayHours, PayPlayFlag, BillingUserKey, FamilyPayPlayDate "
                                     "FROM Player WHERE PlayerID = ?");
        playerStmt.bindString(1, pGamePlayer->getID());
        pResult = playerStmt.execute();

#endif
        if (pResult->getRowCount() != 1) {
            StringStream msg;
            msg << "Failed to load PlayerCreature data from DB. No Player(" << pPacket->getPCName().c_str() << ")";

            filelog("connectDB_BUG.txt", "%s", msg.toString().c_str());

            throw ProtocolException(msg.toString().c_str());
        }

        pResult->next();

        int i = 0;

        string playerID = pResult->getString(++i);
        ServerGroupID_t GID = pResult->getInt(++i);
        string logon = pResult->getString(++i);
        uint scount = pResult->getDWORD(++i);

        payType = (PayType)pResult->getInt(++i);
        payPlayDate = pResult->getString(++i);
        payPlayHours = pResult->getInt(++i);
        payPlayFlag = pResult->getInt(++i);
        billingUserKey = pResult->getInt(++i);
        familyPayPlayDate = pResult->getString(++i);

#ifdef __THAILAND_SERVER__
        string Birthday = pResult->getString(++i);
        pGamePlayer->setPermission(isAdultByBirthdayDate(Birthday));
#endif

        pGamePlayer->setServerGroupID(GID);
        pGamePlayer->setSpecialEventCount(scount);
        pGamePlayer->setBillingUserKey(billingUserKey);

        if (logon != "LOGOFF") {
            char str[80];
            sprintf(str, "Already connected player ID: %s, %s", playerID.c_str(), logon.c_str());
            throw ProtocolException(str);
        }

        PreparedStatement gameLogonStmt(g_pDatabaseManager->getDistConnection("PLAYER_DB"),
                                        "UPDATE Player SET LogOn='GAME' WHERE PlayerID = ? AND LogOn='LOGOFF'");
        gameLogonStmt.bindString(1, playerID);
        gameLogonStmt.execute();


        if (gameLogonStmt.getAffectedRowCount() == 0) {
            char str[80];
            sprintf(str, "Already connected player ID2: %s, %s", playerID.c_str(), logon.c_str());
            throw ProtocolException(str);
        }

        string connectIP = pGamePlayer->getSocket()->getHost();


#if defined(__CONNECT_BILLING_SYSTEM__)
        if (payType == PAY_TYPE_FREE) {
            pGamePlayer->setMetroFreePlayer();
        }
#elif defined(__PAY_SYSTEM_LOGIN__)
        if (pGamePlayer->loginPayPlay(payType, payPlayDate, payPlayHours, payPlayFlag, connectIP, playerID)) {
            sendPayInfo(pGamePlayer);
        } else {
            throw ProtocolException("no pay account");
        }

#elif defined(__PAY_SYSTEM_FREE_LIMIT__)
        if (pGamePlayer->loginPayPlay(payType, payPlayDate, payPlayHours, payPlayFlag, connectIP, playerID)) {
            sendPayInfo(pGamePlayer);
        }
#else // defined(__PAY_SYSTEM_ZONE__)
        pGamePlayer->setPayPlayValue(payType, payPlayDate, payPlayHours, payPlayFlag, familyPayPlayDate);
#endif

    } catch (SQLQueryException& sqe) {
        throw Error(sqe.toString());
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    Slayer* pSlayer = NULL;
    Vampire* pVampire = NULL;
    Ousters* pOusters = NULL;

    bool bAlreadyConnected = false;

    // try
    //{
    if (pPacket->getPCType() == PC_SLAYER) {
        pSlayer = new Slayer();
        pSlayer->setName(pPacket->getPCName());
        pSlayer->setPlayer(pGamePlayer);

        if (!pSlayer->load()) {
            filelog("connectDB_BUG.txt", "Failed to load SLAYER(%s) data from DB", pPacket->getPCName().c_str());
            cout << "  " << endl;
            throw ProtocolException("Failed to load SLAYER data from DB");
        }

        
        // pSlayer->loadItem();
        // Assert(pSlayer->getName() == pPacket->getPCName());
        if (pSlayer->getName() != pPacket->getPCName()) {
            cout << "Different Name : " << pSlayer->getName().c_str() << ", " << pPacket->getPCName().c_str() << endl;

            Assert(pSlayer->getName() == pPacket->getPCName());
        }

        pGamePlayer->setCreature(pSlayer);

        
        // g_pTelephoneCenter->addSlayer(pSlayer);

        
        
        
        EventRegeneration* pEventRegeneration = new EventRegeneration(pGamePlayer);
        pEventRegeneration->setDeadline(10 * 10);
        pGamePlayer->addEvent(pEventRegeneration);

        
        
        try {
            g_pPCFinder->addCreature(pSlayer);
        } catch (DuplicatedException& de) {
            bAlreadyConnected = true;
        }

        
        if (!bAlreadyConnected) {
            
            if (pSlayer->getGuildID() != 99) {
                Guild* pGuild = g_pGuildManager->getGuild(pSlayer->getGuildID());
                if (pGuild != NULL) {
                    
                    try {
                        pGuild->addCurrentMember(pSlayer->getName());

                        GSGuildMemberLogOn gsGuildMemberLogOn;
                        gsGuildMemberLogOn.setGuildID(pGuild->getID());
                        gsGuildMemberLogOn.setName(pSlayer->getName());
                        gsGuildMemberLogOn.setLogOn(true);
                        gsGuildMemberLogOn.setServerID(g_pConfig->getPropertyInt("ServerID"));

                        g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);

                        
                        BEGIN_DB {
                            PreparedStatement logOnStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                                        "UPDATE GuildMember SET LogOn = 1 WHERE Name = ?");
                            logOnStmt.bindString(1, pSlayer->getName());
                            logOnStmt.execute();
                        }
                        END_DB(pStmt)

                    } catch (DuplicatedException& t) {
                        
                        filelog("guildBug.log", "%s", t.toString().c_str());
                    }
                } else
                    filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pSlayer->getGuildID(),
                            pSlayer->getName().c_str());
            }
        }
    } else if (pPacket->getPCType() == PC_VAMPIRE) {
        pVampire = new Vampire();
        pVampire->setName(pPacket->getPCName());
        pVampire->setPlayer(pGamePlayer);

        if (!pVampire->load()) {
            filelog("connectDB_BUG.txt", "Failed to load VAMPIRE(%s) data from DB", pPacket->getPCName().c_str());
            throw ProtocolException("Failed to VAMPIRE data from DB");
        }

        
        // pVampire->loadItem();
        Assert(pVampire->getName() == pPacket->getPCName());

        pGamePlayer->setCreature(pVampire);

        
        
        
        EventRegeneration* pEventRegeneration = new EventRegeneration(pGamePlayer);
        pEventRegeneration->setDeadline(10 * 10);
        pGamePlayer->addEvent(pEventRegeneration);

        
        
        try {
            g_pPCFinder->addCreature(pVampire);
        } catch (DuplicatedException& de) {
            bAlreadyConnected = true;
        }

        
        if (!bAlreadyConnected) {
            
            if (pVampire->getGuildID() != 0) {
                Guild* pGuild = g_pGuildManager->getGuild(pVampire->getGuildID());
                if (pGuild != NULL) {
                    
                    try {
                        pGuild->addCurrentMember(pVampire->getName());

                        GSGuildMemberLogOn gsGuildMemberLogOn;
                        gsGuildMemberLogOn.setGuildID(pGuild->getID());
                        gsGuildMemberLogOn.setName(pVampire->getName());
                        gsGuildMemberLogOn.setLogOn(true);
                        gsGuildMemberLogOn.setServerID(g_pConfig->getPropertyInt("ServerID"));

                        g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);

                        
                        BEGIN_DB {
                            PreparedStatement logOnStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                                        "UPDATE GuildMember SET LogOn = 1 WHERE Name = ?");
                            logOnStmt.bindString(1, pVampire->getName());
                            logOnStmt.execute();
                        }
                        END_DB(pStmt)
                    } catch (DuplicatedException& t) {
                        
                        filelog("guildBug.log", "%s", t.toString().c_str());
                    }
                } else
                    filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pVampire->getGuildID(),
                            pVampire->getName().c_str());
            }
        }
    } else if (pPacket->getPCType() == PC_OUSTERS) {
        pOusters = new Ousters();
        pOusters->setName(pPacket->getPCName());
        pOusters->setPlayer(pGamePlayer);

        if (!pOusters->load()) {
            filelog("connectDB_BUG.txt", "Failed to load VAMPIRE(%s) data from DB", pPacket->getPCName().c_str());
            throw ProtocolException("Failed to VAMPIRE data from DB");
        }

        
        // pVampire->loadItem();
        Assert(pOusters->getName() == pPacket->getPCName());
        // filelog("Ousters.txt","CGConectHandler.cpp 0,HP:%d,MAXHP:%d,MP:%d,MAXMP:%d",  (int)pOusters->getHP(
        // ATTR_CURRENT), (int)pOusters->getHP(ATTR_MAX),(int)pOusters->getMP(ATTR_CURRENT),
        // (int)pOusters->getMP(ATTR_MAX));

        pGamePlayer->setCreature(pOusters);

        
        
        
        EventRegeneration* pEventRegeneration = new EventRegeneration(pGamePlayer);
        pEventRegeneration->setDeadline(10 * 10);
        pGamePlayer->addEvent(pEventRegeneration);

        
        
        try {
            g_pPCFinder->addCreature(pOusters);
        } catch (DuplicatedException& de) {
            bAlreadyConnected = true;
        }

        
        if (!bAlreadyConnected) {
            
            if (pOusters->getGuildID() != 66) {
                Guild* pGuild = g_pGuildManager->getGuild(pOusters->getGuildID());
                if (pGuild != NULL) {
                    
                    try {
                        pGuild->addCurrentMember(pOusters->getName());

                        GSGuildMemberLogOn gsGuildMemberLogOn;
                        gsGuildMemberLogOn.setGuildID(pGuild->getID());
                        gsGuildMemberLogOn.setName(pOusters->getName());
                        gsGuildMemberLogOn.setLogOn(true);
                        gsGuildMemberLogOn.setServerID(g_pConfig->getPropertyInt("ServerID"));

                        g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);

                        
                        BEGIN_DB {
                            PreparedStatement logOnStmt(g_pDatabaseManager->getConnection("DARKEDEN"),
                                                        "UPDATE GuildMember SET LogOn = 1 WHERE Name = ?");
                            logOnStmt.bindString(1, pOusters->getName());
                            logOnStmt.execute();
                        }
                        END_DB(pStmt)
                    } catch (DuplicatedException& t) {
                        
                        filelog("guildBug.log", "%s", t.toString().c_str());
                    }
                } else
                    filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n", (int)pOusters->getGuildID(),
                            pOusters->getName().c_str());
            }
        }
    }
    //}

    cout << "  2" << endl;

    
    
    
    
    
    // by sigi. 2002.8.29
    if (bAlreadyConnected) {
        SAFE_DELETE(pSlayer);
        SAFE_DELETE(pVampire);
        SAFE_DELETE(pOusters);

        pGamePlayer->setID(string("")); 
        pGamePlayer->setCreature(NULL); 

        char str[80];
        sprintf(str, "Already connected player ID3(Dup): %s", pPacket->getPCName().c_str());
        throw ProtocolException(str);
    }

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------
    Creature* pCreature = pGamePlayer->getCreature();
    Assert(pCreature != NULL);

    EventHeadCount* pEventHeadCount = new EventHeadCount(pGamePlayer);
    pEventHeadCount->setDeadline(18000);
    pGamePlayer->addEvent(pEventHeadCount);
    cout << "  3" << endl;
    if (pCreature->isGOD()) {
        EffectGhost* pEffect = new EffectGhost(pCreature);
        pCreature->getEffectManager()->addEffect(pEffect);
        pCreature->setFlag(Effect::EFFECT_CLASS_GHOST);
        pCreature->setMoveMode(Creature::MOVE_MODE_FLYING);
    } else {
#ifdef __TEST_SERVER__
        EventAuth* pAuth = new EventAuth(pGamePlayer);
        pGamePlayer->addEvent(pAuth);
        cout << "    : " << pGamePlayer->getID() << endl;
#endif
    }

    if (pCreature->getCompetenceShape() == 0) {
        EffectGnomesWhisper* pEffect = new EffectGnomesWhisper(pCreature);
        pEffect->setLevel(30);

        pCreature->addEffect(pEffect);
        pCreature->setFlag(pEffect->getEffectClass());
    }
    cout << "  4" << endl;
    
    ZoneID_t castleZoneID;
    ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pCreature->getZoneID());

    bool isCastleZone = g_pCastleInfoManager->getCastleZoneID(pCreature->getZoneID(), castleZoneID);
    bool isMasterLair = pZoneInfo->isMasterLair() || GDRLairManager::Instance().isGDRLairZone(pCreature->getZoneID());
    bool isPKZone = g_pPKZoneInfoManager->isPKZone(pCreature->getZoneID());
    bool isMaze = (pCreature->getZoneID() == 3001) || (pCreature->getZoneID() == 3002) ||
                  (pCreature->getZoneID() == 3003) || (pCreature->getZoneID() == 1013); // add by sonic 2006.10.30
    bool isEventZone = EventZoneInfoManager::Instance().getEventZoneInfo(pCreature->getZoneID()) != NULL;
    bool isBeginnerZone = pCreature->getZoneID() == 1122 && !canEnterBeginnerZone(pCreature);
    bool isDynamicZone = g_pDynamicZoneManager->isDynamicZone(pCreature->getZoneID());

    if (pCreature->isPC()) //&& pCreature->isPLAYER() )
    {
        ZONE_COORD zoneCoord;
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        Assert(pPC != NULL);

        if (isCastleZone || isMasterLair || isPKZone || isMaze || isEventZone || isBeginnerZone || isDynamicZone) {
            if (g_pResurrectLocationManager->getPosition(pPC, zoneCoord)) {
                pCreature->setZoneID(zoneCoord.id);
                pCreature->setXY(zoneCoord.x, zoneCoord.y);
            }
        }
    }
    cout << "  5" << endl;
    Zone* pZone = pCreature->getZone();
    Assert(pZone != NULL);

#if defined(__PAY_SYSTEM_FREE_LIMIT__)
    try {
        ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());
        Assert(pZoneInfo != NULL);

        Assert(pCreature->isPC());
        PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
        Assert(pPC != NULL);

        
        if ((pGamePlayer->isPayPlaying() || pGamePlayer->isPremiumPlay()) &&
            pGamePlayer->getPayType() == PAY_TYPE_TIME && pPC->canPlayFree() &&
            !(pZoneInfo->isPayPlay() || pZone->isPremiumZone())) {
            pGamePlayer->logoutPayPlay(pGamePlayer->getID(), false, false);
        }

        
        
        if ((pZoneInfo->isPayPlay()) //|| pZoneInfo->isPremiumZone())
            && !pGamePlayer->isPayPlaying()) {
            
            
            
            ZONE_COORD zoneCoord;

            if (g_pResurrectLocationManager->getRaceDefaultPosition(pPC->getRace(), zoneCoord)) {
                pCreature->setZoneID(zoneCoord.id);
                pCreature->setXY(zoneCoord.x, zoneCoord.y);
            }
        }
    } catch (NoSuchElementException&) {
        throw Error("ZoneInfo zoneID .");
    }
#elif defined(__CONNECT_BILLING_SYSTEM__)
    if (pGamePlayer->isMetroFreePlayer()) {
        pGamePlayer->setBillingUserStatus("MT");
        pGamePlayer->setPremiumPlay();
    } else {
        try {
            ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());
            Assert(pZoneInfo != NULL);

            Assert(pCreature->isPC());
            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPC != NULL);

            
            
            
            if (pZoneInfo->isPayPlay()) {
                pGamePlayer->setPremiumPlay();
            }

        } catch (NoSuchElementException&) {
            throw Error("ZoneInfo zoneID .");
        }

        
        pGamePlayer->setBillingSession();

        
        pGamePlayer->sendBillingLogin();
    }

#elif defined(__PAY_SYSTEM_ZONE__)
    try {
        ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(pZone->getZoneID());

        
        if (pZoneInfo != NULL && (pZoneInfo->isPayPlay() || pZoneInfo->isPremiumZone()) &&
            !pGamePlayer->isPayPlaying()) {
            bool bEnterZone = true;

            string connectIP = pGamePlayer->getSocket()->getHost();

            
            
            if (!pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID()) && pZoneInfo->isPayPlay()) {
                bEnterZone = false;
            }

            if (!pGamePlayer->loginPayPlay(connectIP, pGamePlayer->getID()) &&
                (pZone->getZoneID() == 1131 || pZone->getZoneID() == 1132 || pZone->getZoneID() == 1133 ||
                 pZone->getZoneID() == 1134)) {
                bEnterZone = false;
            }

            Assert(pCreature->isPC());
            PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
            Assert(pPC != NULL);

            if (!bEnterZone) {
                
                
                
                ZONE_COORD zoneCoord;
                //				bool bFindPos = false;

                if (g_pResurrectLocationManager->getRaceDefaultPosition(pPC->getRace(), zoneCoord)) {
                    pCreature->setZoneID(zoneCoord.id);
                    pCreature->setXY(zoneCoord.x, zoneCoord.y);
                }
            }
        }
    } catch (NoSuchElementException&) {
        throw Error("ZoneInfo zoneID .");
    }
#elif defined(__CONNECT_CBILLING_SYSTEM__)
    
    EventCBilling* pEvent = new EventCBilling(pGamePlayer);
    pEvent->setDeadline(g_pCBillingPlayerManager->getMinusIntervalInt() * 600);
    pGamePlayer->addEvent(pEvent);

    
    pGamePlayer->setPayPlayer(true);
    pGamePlayer->setPremiumPlay();

    // send Login
    pGamePlayer->setCBillingVerified(g_pCBillingPlayerManager->sendLogin(pGamePlayer));
#else
    
    
    pGamePlayer->setPremiumPlay();
#endif

    // test code
     

    
    // 2002.8.26. by sigi
    cout << "        ." << endl;
    if (pPacket->getPCType() == PC_SLAYER) {
        Assert(pSlayer != NULL);
        pSlayer->loadItem(true);
    } else if (pPacket->getPCType() == PC_VAMPIRE) {
        Assert(pVampire != NULL);
        pVampire->loadItem(true);
    } else if (pPacket->getPCType() == PC_OUSTERS) {
        Assert(pOusters != NULL);
        pOusters->loadItem(true);
    }


    
    
    if (pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(pCreature);

        switch (pZone->getZoneID()) {
        case 2000:
        case 2001:
        case 2002:
        case 2003:
        case 2004:
        case 2010:
        case 2011:
        case 2012:
        case 2013:
        case 2014:
        case 2020:
        case 2021:
        case 2022:
        case 2023:
        case 2024:
            pSlayer->setHP(pSlayer->getHP(ATTR_MAX), ATTR_CURRENT);
            break;
        default:
            break;
        }
    } else if (pCreature->isVampire()) {
        Vampire* pVampire = dynamic_cast<Vampire*>(pCreature);

        switch (pZone->getZoneID()) {
        case 1003:
        case 1007:
            pVampire->setHP(pVampire->getHP(ATTR_MAX), ATTR_CURRENT);
            break;
        default:
            break;
        }
    } else if (pCreature->isOusters()) {
        Ousters* pOusters = dynamic_cast<Ousters*>(pCreature);

        switch (pZone->getZoneID()) {
        case 1311:
            // filelog("Ousters.txt","CGConectHandler.cpp 1,HP:%d,MAXHP:%d,MP:%d,MAXMP:%d",  (int)pOusters->getHP(
            // ATTR_CURRENT), (int)pOusters->getHP(ATTR_MAX),(int)pOusters->getMP(ATTR_CURRENT),
            // (int)pOusters->getMP(ATTR_MAX));
            pOusters->setHP(pOusters->getHP(ATTR_MAX), ATTR_CURRENT);
            // chengh add 2005 10 02
            pOusters->setMP(pOusters->getMP(ATTR_MAX), ATTR_CURRENT);
            // filelog("Ousters.txt","CGConectHandler.cpp 2,HP:%d,MAXHP:%d,MP:%d,MAXMP:%d",  (int)pOusters->getHP(
            // ATTR_CURRENT), (int)pOusters->getHP(ATTR_MAX),(int)pOusters->getMP(ATTR_CURRENT),
            // (int)pOusters->getMP(ATTR_MAX));

        default:
            break;
        }
    }

    
    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pCreature);
    pPC->getQuestManager()->load();

    
    
    pCreature->registerObject();

    
    
    
    // EventSave* pEventSave = new EventSave(pGamePlayer);
    // pEventSave->setDeadline(600* 10);
    // pGamePlayer->addEvent(pEventSave);

#ifdef __USE_ENCRYPTER__
    
    pGamePlayer->setEncryptCode();
#endif

    //	pGamePlayer->sendPacket( SystemAvailabilitiesManager::getInstance()->getAvailabilitiesPacket() );
    SEND_SYSTEM_AVAILABILITIES(pGamePlayer);

    //----------------------------------------------------------------------
    
    //----------------------------------------------------------------------

    cout << "  7" << endl;

    GCUpdateInfo gcUpdateInfo;

    makeGCUpdateInfo(&gcUpdateInfo, pCreature);

    FILELOG_INCOMING_CONNECTION("cgconnect_trace.log", "sending GCUpdateInfo host=%s playerID=%s",
                                pGamePlayer->getSocket()->getHost().c_str(), pGamePlayer->getID().c_str());
    pGamePlayer->sendPacket(&gcUpdateInfo);

    sendPetInfo(pGamePlayer);

    string host = pGamePlayer->getSocket()->getHost();

    IP_t IP = pGamePlayer->getSocket()->getHostIP();

    

    //--------------------------------------------------
    // change player status
    //--------------------------------------------------
    pGamePlayer->setPlayerStatus(GPS_WAITING_FOR_CG_READY);

#endif

    __END_DEBUG
    __END_DEBUG_EX __END_CATCH
}


#ifdef __GAME_SERVER__

//////////////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////////////
bool isAdultByBirthdayDate(const string& birthday) {
    StringStream AdultSSN;

    time_t daytime = time(0);
    tm Timec;
    localtime_r(&daytime, &Timec);

    AdultSSN << Timec.tm_year - 18;
    // tm_mon - range 0 to 11
    if ((Timec.tm_mon + 1) < 10)
        AdultSSN << "0";
    AdultSSN << (Timec.tm_mon + 1);
    if (Timec.tm_mday < 10)
        AdultSSN << "0";
    AdultSSN << Timec.tm_mday;

    cout << "Birthday = " << birthday.c_str() << " ADULTSSN = " << AdultSSN.toString().c_str() << endl;

    int year = atoi(birthday.substr(0, 4).c_str());
    int month = atoi(birthday.substr(5, 2).c_str());
    int day = atoi(birthday.substr(8, 2).c_str());

    if (Timec.tm_year - 18 + 1900 > year) {
        cout << "" << endl;
        return true;
    } else if (Timec.tm_year - 18 + 1900 == year) {
        if (Timec.tm_mon + 1 > month) {
            cout << "" << endl;
            return true;
        } else if (Timec.tm_mon + 1 == month) {
            if (Timec.tm_mday >= day) {
                cout << "" << endl;
                return true;
            }
        }
    }

    cout << "" << endl;
    return false;
}

#endif
