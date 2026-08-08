//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginPlyaer.cpp
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#include "LoginPlayer.h"

#include "Assert.h"
#include "DB.h"
#include "GameServerInfoManager.h"
#include "GameServerManager.h"
#include "LCLoginError.h"
#include "LCLoginOK.h"
#include "LCPCList.h"
#include "LGKickCharacter.h"
#include "LogClient.h"
#include "Packet.h"
#include "PacketFactoryManager.h"
#include "PacketProfile.h"
#include "PacketValidator.h"
#include "PreparedStatement.h"
#include "Profile.h"
#include "gameserver/billing/BillingPlayerManager.h"

// by sigi. 2002.11.12
const int defaultLoginPlayerInputStreamSize = 1024;
const int defaultLoginPlayerOutputStreamSize = 4096;

static int maxIdleSec = 60 * 15;


static uint maxWaitForKickCharacter = 3;
static uint maxWaitForKickCharacterCount = 3;


void addLoginPlayerData(const string& ID, const string& ip, const string& SSN, const string& zipcode);
void addLogoutPlayerData(Player* pPlayer);


//////////////////////////////////////////////////////////////////////
//
// constructor
//
//////////////////////////////////////////////////////////////////////
LoginPlayer::LoginPlayer(Socket* pSocket)
    : // Player(pSocket),
      m_PlayerStatus(LPS_NONE), m_FailureCount(0) {
    __BEGIN_TRY

    Assert(pSocket != NULL);
    m_pSocket = pSocket;

    // create socket input stream
    m_pInputStream = new SocketInputStream(m_pSocket, defaultLoginPlayerInputStreamSize);

    Assert(m_pInputStream != NULL);

    // create socket output stream
    m_pOutputStream = new SocketOutputStream(m_pSocket, defaultLoginPlayerOutputStreamSize);
    Assert(m_pOutputStream != NULL);


    m_Mutex.setName("LoginPlayer");

    m_ID = "NONE";

    Assert(m_PacketHistory.empty());


    getCurrentTime(m_ExpireTime);
    m_ExpireTime.tv_sec += maxIdleSec;

    m_bSetWorldGroupID = false;
    m_WorldID = 1;
    m_ServerGroupID = 0;
    m_LastSlot = 0;

    m_isAdult = true;

    m_KickCharacterCount = 0;

    m_bFreePass = false;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// destructor
//
//////////////////////////////////////////////////////////////////////
LoginPlayer::~LoginPlayer() {
    __BEGIN_TRY


    Assert(m_PlayerStatus == LPS_END_SESSION);

    // delete all previous packets
    while (!m_PacketHistory.empty()) {
        Packet* pPacket = m_PacketHistory.front();
        delete pPacket;
        m_PacketHistory.pop_front();
    }

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////
//


//
//////////////////////////////////////////////////////////////////////
void LoginPlayer::setExpireTimeForKickCharacter() {
    getCurrentTime(m_ExpireTimeForKickCharacter);

    m_ExpireTimeForKickCharacter.tv_sec += maxWaitForKickCharacter;
}

//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void LoginPlayer::processCommand(bool Option) {
    __BEGIN_TRY

    //	static Timeval currentTime;


    if (m_PlayerStatus == LPS_WAITING_FOR_GL_KICK_VERIFY) {
        Timeval currentTime;
        getCurrentTime(currentTime);


        if (currentTime >= m_ExpireTimeForKickCharacter) {
            sendLGKickCharacter();


            if (++m_KickCharacterCount >= maxWaitForKickCharacterCount) {
                sendLCLoginOK();
            }
        }

        return;
    }

    try {
        char header[szPacketHeader];
        PacketID_t packetID;
        PacketSize_t packetSize;
        Packet* pPacket;


        while (true) {
            if (!m_pInputStream->peek(header, szPacketHeader)) {
                Timeval currentTime;
                getCurrentTime(currentTime);
                if (currentTime >= m_ExpireTime)
                    throw DisconnectException("     .");
                break;
            }


            memcpy(&packetID, &header[0], szPacketID);
            memcpy(&packetSize, &header[szPacketID], szPacketSize);

            /*
            LOG4("RECV PACKET from %s, %s(%d) %d/%d\n",
                m_ID.c_str(),
                g_pPacketFactoryManager->getPacketName( packetID ).c_str(),
                packetID,
                szPacketHeader + packetSize,
                m_pInputStream->length()
            );

            */
            // DEBUG by tiancaiamao
            StringStream msg;
            msg << "RECV PACKET from " << m_ID << ", " << g_pPacketFactoryManager->getPacketName(packetID) << "("
                << packetID << ") " << szPacketHeader + packetSize << "/" << m_pInputStream->length() << eos;
            cout << msg.toString() << endl;


            if (packetID >= Packet::PACKET_MAX)

                throw InvalidProtocolException("too large packet id");

            try {
                if (!g_pPacketValidator->isValidPacketID(getPlayerStatus(), packetID)) {
                    // DEBUG by tiancaiamao
                    cout << "player status: " << getPlayerStatus() << " receive packet: " << packetID << endl;
                    throw InvalidProtocolException("invalid packet order");
                }


                if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID))
                    throw InvalidProtocolException("too large packet size");


                if (m_pInputStream->length() < szPacketHeader + packetSize)
                    //	throw InsufficientDataException();
                    break;


                getCurrentTime(m_ExpireTime);
                m_ExpireTime.tv_sec += maxIdleSec;


                pPacket = g_pPacketFactoryManager->createPacket(packetID);


                m_pInputStream->readPacket(pPacket);

                Timeval start, end;
                getCurrentTime(start);


                pPacket->execute(this);

                getCurrentTime(end);
                g_PacketProfileManager.addAccuTime(pPacket->getPacketName(), start, end);


                m_PacketHistory.push_back(pPacket);


                while (m_PacketHistory.size() > nPacketHistory) {
                    Packet* oldPacket = m_PacketHistory.front();
                    delete oldPacket;
                    m_PacketHistory.pop_front();
                }

            } catch (IgnorePacketException&) {
                if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID))
                    throw InvalidProtocolException("too large packet size");


                if (m_pInputStream->length() < szPacketHeader + packetSize)
                    throw InsufficientDataException();


                m_pInputStream->skip(szPacketHeader + packetSize);
            }
        }

    } catch (InsufficientDataException& ide) {
        Timeval currentTime;
        getCurrentTime(currentTime);
        if (currentTime >= m_ExpireTime)
            throw DisconnectException("     .");

    } catch (InvalidProtocolException& ipe) {
        throw;

    } catch (DisconnectException& de) {
        throw;
    }

    __END_CATCH
}


//--------------------------------------------------------------------------------
// disconnect player
//--------------------------------------------------------------------------------
void LoginPlayer::disconnect(bool bDisconnected) {
    __BEGIN_TRY

    if (bDisconnected == UNDISCONNECTED) {
        // GCDisconnect lcDisconnect;
        // sendPacket( lcDisconnect );


        m_pOutputStream->flush();
    }


    m_pSocket->close();


    if (m_PlayerStatus == LPS_WAITING_FOR_GL_KICK_VERIFY) {
        m_ID = "NONE";
    }


    Assert(m_PlayerStatus != LPS_END_SESSION);
    m_PlayerStatus = LPS_END_SESSION;


    if (m_ID != "NONE") {
        // Result*    pResult = NULL;

        try {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            //	pResult = pStmt->executeQuery( "SELECT LogOn FROM Player WHERE PlayerID='%s'" , m_ID.c_str() );


            //	pResult->next();
            //	string logon = pResult->getString(1);
            // cout << "logon = " << logon << endl;
            // Assert( logon == "LOGON" );


            PreparedStatement logoffPlayerStmt(pConn,
                                               "UPDATE Player SET LogOn = 'LOGOFF' WHERE PlayerID=? AND LogOn='LOGON'");
            logoffPlayerStmt.bindString(1, m_ID);
            logoffPlayerStmt.execute();

#if defined(__PAY_SYSTEM_LOGIN__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
            bool bClear = false;
            bool bDecreaseTime = false;
            logoutPayPlay(m_ID, bClear, bDecreaseTime);
#endif

            // cout << m_ID << " : LOGOFF" << endl;
        } catch (SQLQueryException& sqe) {
            filelog("DBError.log", "%s", sqe.toString().c_str());
            throw Error(sqe.toString());
        }
    }

    addLogoutPlayerData(this);

    __END_CATCH
}
//--------------------------------------------------------------------------------
// disconnect player no log

//--------------------------------------------------------------------------------
void LoginPlayer::disconnect_nolog(bool bDisconnected) {
    __BEGIN_TRY

    if (bDisconnected == UNDISCONNECTED) {
        // GCDisconnect lcDisconnect;
        // sendPacket( lcDisconnect );


        m_pOutputStream->flush();
    }


    m_pSocket->close();


    if (m_PlayerStatus == LPS_WAITING_FOR_GL_KICK_VERIFY) {
        m_ID = "NONE";
    }


    Assert(m_PlayerStatus != LPS_END_SESSION);
    m_PlayerStatus = LPS_END_SESSION;


    if (m_ID != "NONE") {
        // Result*    pResult = NULL;

        try {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            //	pResult = pStmt->executeQuery( "SELECT LogOn FROM Player WHERE PlayerID='%s'" , m_ID.c_str() );


            //	pResult->next();
            //	string logon = pResult->getString(1);
            // cout << "logon = " << logon << endl;
            // Assert( logon == "LOGON" );


            PreparedStatement logoffPlayerStmt(pConn,
                                               "UPDATE Player SET LogOn = 'LOGOFF' WHERE PlayerID=? AND LogOn='LOGON'");
            logoffPlayerStmt.bindString(1, m_ID);
            logoffPlayerStmt.execute();

#if defined(__PAY_SYSTEM_LOGIN__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
            bool bClear = false;
            bool bDecreaseTime = false;
            logoutPayPlay(m_ID, bClear, bDecreaseTime);
#endif

            // cout << m_ID << " : LOGOFF" << endl;
        } catch (SQLQueryException& sqe) {
            filelog("DBError.log", "%s", sqe.toString().c_str());
            throw Error(sqe.toString());
        }
    }


    __END_CATCH
}

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
void LoginPlayer::sendPacket(Packet* pPacket) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    // LOG4("SEND PACKET to %s : %s(%d) %d /%d\n", m_ID.c_str() , pPacket->getPacketName().c_str() ,
    // pPacket->getPacketID(), pPacket->getPacketSize(), m_pOutputStream->length() );
    Player::sendPacket(pPacket);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//

//

//
//////////////////////////////////////////////////////////////////////
Packet* LoginPlayer::getOldPacket(uint prev) {
    __BEGIN_TRY

    if (prev >= nPacketHistory)
        throw OutOfBoundException();
    if (prev >= m_PacketHistory.size())
        throw NoSuchElementException();
    return m_PacketHistory[m_PacketHistory.size() - prev - 1];

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
Packet* LoginPlayer::getOldPacket(PacketID_t packetID) {
    __BEGIN_TRY

    Packet* pPacket = NULL;

    deque<Packet*>::reverse_iterator ritr = m_PacketHistory.rbegin();
    for (; ritr != m_PacketHistory.rend(); ritr++) {
        if ((*ritr)->getPacketID() == packetID) {
            pPacket = (*ritr);
            break;
        }
    }

    if (pPacket == NULL)
        throw NoSuchElementException();

    return pPacket;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
//
// send LGKickCharacter
//

//
//////////////////////////////////////////////////////////////////////////////
void LoginPlayer::sendLGKickCharacter() {
    cout << "send LGKickCharacter" << endl;


    LGKickCharacter lgKickCharacter;

    Statement* pStmt = NULL;
    Statement* pStmt1 = NULL;

    string characterName = getLastCharacterName();
    int serverID, serverGroupID, worldID, lastSlot;

    string gameServerIP;
    uint gameServerPort;

    //----------------------------------------------------------------------


    //----------------------------------------------------------------------
    if (!isSetWorldGroupID()) {
        BEGIN_DB {
            Connection* pConn1 = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement selectWorldInfoStmt(
                pConn1, "SELECT CurrentWorldID, CurrentServerGroupID, LastSlot FROM Player where PlayerID=?");
            selectWorldInfoStmt.bindString(1, getID());
            Result* pResult = selectWorldInfoStmt.execute();

            if (pResult->next()) {
                serverID = 1;
                worldID = pResult->getInt(1);
                serverGroupID = pResult->getInt(2);
                lastSlot = pResult->getInt(3);

                setWorldID(worldID);
                setGroupID(serverGroupID);
                setLastSlot(lastSlot);

                setWorldGroupID(true);
            }
        }
        END_DB(pStmt1)
    } else {
        serverID = 1;
        worldID = getWorldID();
        serverGroupID = getGroupID();
    }


    //----------------------------------------------------------------------

    //----------------------------------------------------------------------
    if (characterName.size() == 0) {
        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection(m_WorldID);
            PreparedStatement selectCharacterNameStmt(pConn, "SELECT Name from Slayer where PlayerID=? AND Slot=?");
            selectCharacterNameStmt.bindString(1, getID());
            // Preserves the original "SLOT%d" formatting verbatim (not the
            // 0-indexed Slot2String[] table used elsewhere) since LastSlot is
            // stored as the raw 1-based suffix, not a Slot enum value.
            selectCharacterNameStmt.bindString(2, "SLOT" + std::to_string(lastSlot));
            Result* pResult = selectCharacterNameStmt.execute();

            if (pResult->next()) {
                characterName = pResult->getString(1);

                setLastCharacterName(characterName);
            } else {
                cout << "No CharacterName" << endl;

                LCLoginError lcLoginError;
                lcLoginError.setErrorID(ALREADY_CONNECTED);
                sendPacket(&lcLoginError);
                setPlayerStatus(LPS_BEGIN_SESSION);

                setID("NONE");

                return;
            }
        }
        END_DB(pStmt)
    }

    //----------------------------------------------------------------------

    //

    //----------------------------------------------------------------------
    for (int i = 0; i < g_pGameServerInfoManager->getMaxServerGroupID(); i++) {
        serverGroupID = i;

        try {
            cout << "World=" << worldID << ", "
                 << "Group=" << serverGroupID << ", "
                 << "Server=" << serverID << endl;

            GameServerInfo* pGameServerInfo =
                g_pGameServerInfoManager->getGameServerInfo(serverID, serverGroupID, worldID);

            if (pGameServerInfo != NULL) {
                gameServerIP = pGameServerInfo->getIP();
                gameServerPort = pGameServerInfo->getUDPPort();

                cout << "IP=" << gameServerIP.c_str() << ", Port=" << gameServerPort << endl;
            }
        } catch (NoSuchElementException&) {
            cout << "No GameServerInfo" << endl;

            //		LCLoginError lcLoginError;
            //		lcLoginError.setErrorID(ALREADY_CONNECTED);
            //		sendPacket(&lcLoginError);
            //		setPlayerStatus(LPS_BEGIN_SESSION);

            setID("NONE");

            return;
        }

        lgKickCharacter.setID(getSocket()->getSOCKET());
        lgKickCharacter.setPCName(characterName);

        cout << "( " << gameServerIP.c_str() << ", " << gameServerPort << " )" << endl;
        g_pGameServerManager->sendPacket(gameServerIP, gameServerPort, &lgKickCharacter);
    }

    setExpireTimeForKickCharacter();
    setPlayerStatus(LPS_WAITING_FOR_GL_KICK_VERIFY);
}


//////////////////////////////////////////////////////////////////////
//
// send LCLoginOK
//


//
//////////////////////////////////////////////////////////////////////
void LoginPlayer::sendLCLoginOK() {
    try {
        // cout << "Send LCLoginOK" << endl;

        Statement* pStmt = NULL;

        string connectIP = getSocket()->getHost();

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

            PreparedStatement loginPlayerStmt(pConn, "UPDATE Player SET LogOn = 'LOGON' WHERE PlayerID = ?");
            loginPlayerStmt.bindString(1, getID());
            loginPlayerStmt.execute();
            if (loginPlayerStmt.getAffectedRowCount() == 0) {
                filelog("MultiLogin.log", "     : [%s:%s]", getID().c_str(), connectIP.c_str());
                LCLoginError lcLoginError;
                // lcLoginError.setMessage("already connected");
                lcLoginError.setErrorID(ALREADY_CONNECTED);
                sendPacket(&lcLoginError);

                setPlayerStatus(LPS_BEGIN_SESSION);

                return;
            }

            PreparedStatement updateLoginIPStmt(pConn, "UPDATE Player SET LoginIP = ? WHERE PlayerID = ?");
            updateLoginIPStmt.bindString(1, connectIP);
            updateLoginIPStmt.bindString(2, getID());
            updateLoginIPStmt.execute();
        }
        END_DB(pStmt)


        LCLoginOK lcLoginOK;


        lcLoginOK.setAdult(isAdult());
        lcLoginOK.setLastDays(0xffff);

        sendPacket(&lcLoginOK);

        setPlayerStatus(LPS_WAITING_FOR_CL_GET_PC_LIST);


        addLoginPlayerData(m_ID, connectIP, m_SSN, m_Zipcode);

    } catch (Throwable& t) {
        filelog("loginOKError.txt", "%s", t.toString().c_str());
        throw;
    }
}


bool LoginPlayer::sendBillingLogin() {
    __BEGIN_TRY

    if (!m_ID.empty() && m_ID != "NONE") {
        Timeval currentTime;
        getCurrentTime(currentTime);

        if (currentTime > m_BillingNextLoginRequestTime) {
            g_pBillingPlayerManager->sendPayLogin(this);


            m_BillingLoginRequestCount++;


            m_BillingNextLoginRequestTime.tv_sec = currentTime.tv_sec + 10;
        }

        return true;
    }

    return false;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string LoginPlayer::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "LoginPlayer("
        << "ID:" << m_ID << ",SocketID:" << m_pSocket->getSOCKET() << ",Host:" << m_pSocket->getHost() << ")";
    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
//
// add LogoutPlayerdata
//


//
//////////////////////////////////////////////////////////////////////////////
void addLogoutPlayerData(Player* pPlayer) {}

void LoginPlayer::makePCList(LCPCList& lcPCList) {
    WorldID_t WorldID = getWorldID();

    try {
        Connection* pConn = g_pDatabaseManager->getConnection(WorldID);
        Connection* pConn2 = g_pDatabaseManager->getConnection(WorldID);

        //----------------------------------------------------------------------


        //


        //
        //----------------------------------------------------------------------
        PreparedStatement selectSlayerListStmt(
            pConn,
            "SELECT Race, Name, Slot, Sex, HairColor, SkinColor, AdvancementClass, STR, STRExp, DEX, DEXExp, INTE, "
            "INTExp, HP, CurrentHP, MP, CurrentMP, Fame, BladeLevel, SwordLevel, GunLevel, HealLevel, EnchantLevel, "
            "ETCLevel, Alignment, Shape, HelmetColor, JacketColor, PantsColor, WeaponColor, ShieldColor, `Rank` FROM "
            "Slayer WHERE PlayerID = ? AND Active = 'ACTIVE'");
        selectSlayerListStmt.bindString(1, getID());
        Result* pResult1 = selectSlayerListStmt.execute();

        // Prepared once, executed per matching row below (Vampire/Ousters
        // rows can't share a statement since their SQL text differs).
        PreparedStatement selectVampireStmt(
            pConn2, "SELECT Name, Slot, Sex, BatColor, SkinColor, AdvancementClass, STR, DEX, INTE, HP, CurrentHP, "
                    "`Rank`, GoalExp, Level, Bonus, Fame, Alignment, Shape, CoatColor FROM Vampire WHERE PlayerID = "
                    "? AND Active = 'ACTIVE' AND Name=?");
        PreparedStatement selectOustersStmt(
            pConn2, "SELECT Name, Slot, Sex, AdvancementClass, STR, DEX, INTE, HP, CurrentHP, `Rank`, Exp, Level, "
                    "Bonus, SkillBonus, Fame, Alignment, CoatType, ArmType, CoatColor, HairColor, ArmColor, BootsColor "
                    "FROM Ousters WHERE PlayerID = ? AND Active = 'ACTIVE' AND Name=?");

        DWORD shape;
        Color_t colors[PCSlayerInfo::SLAYER_COLOR_MAX];
        Color_t colorsVamp[PCVampireInfo::VAMPIRE_COLOR_MAX];

        while (pResult1->next()) {
            uint i = 0;
            string race = pResult1->getString(++i);
            string name = pResult1->getString(++i);

            if (race == "SLAYER") {
                PCSlayerInfo* pPCSlayerInfo = new PCSlayerInfo();


                pPCSlayerInfo->setName(name);
                pPCSlayerInfo->setSlot(pResult1->getString(++i));
                pPCSlayerInfo->setSex(pResult1->getString(++i));
                pPCSlayerInfo->setHairStyle(HAIR_STYLE1);
                pPCSlayerInfo->setHairColor(pResult1->getInt(++i));
                pPCSlayerInfo->setSkinColor(pResult1->getInt(++i));
                pPCSlayerInfo->setAdvancementLevel(pResult1->getInt(++i));
                pPCSlayerInfo->setSTR(pResult1->getInt(++i));
                pPCSlayerInfo->setSTRExp(pResult1->getInt(++i));
                pPCSlayerInfo->setDEX(pResult1->getInt(++i));
                pPCSlayerInfo->setDEXExp(pResult1->getInt(++i));
                pPCSlayerInfo->setINT(pResult1->getInt(++i));
                pPCSlayerInfo->setINTExp(pResult1->getInt(++i));
                pPCSlayerInfo->setHP(pResult1->getInt(++i), pResult1->getInt(++i));
                pPCSlayerInfo->setMP(pResult1->getInt(++i), pResult1->getInt(++i));
                pPCSlayerInfo->setFame(pResult1->getInt(++i));

                for (int j = 0; j < SKILL_DOMAIN_VAMPIRE; j++) {
                    pPCSlayerInfo->setSkillDomainLevel((SkillDomain)j, (SkillLevel_t)pResult1->getInt(++i));
                }

                pPCSlayerInfo->setAlignment(pResult1->getInt(++i));


                shape = pResult1->getDWORD(++i);

                colors[PCSlayerInfo::SLAYER_COLOR_HAIR] = pPCSlayerInfo->getHairColor();
                colors[PCSlayerInfo::SLAYER_COLOR_SKIN] = pPCSlayerInfo->getSkinColor();
                colors[PCSlayerInfo::SLAYER_COLOR_HELMET] = pResult1->getInt(++i);
                colors[PCSlayerInfo::SLAYER_COLOR_JACKET] = pResult1->getInt(++i);
                colors[PCSlayerInfo::SLAYER_COLOR_PANTS] = pResult1->getInt(++i);
                colors[PCSlayerInfo::SLAYER_COLOR_WEAPON] = pResult1->getInt(++i);
                colors[PCSlayerInfo::SLAYER_COLOR_SHIELD] = pResult1->getInt(++i);

                pPCSlayerInfo->setShapeInfo(shape, colors);

                pPCSlayerInfo->setRank(pResult1->getInt(++i));

                lcPCList.setPCInfo(pPCSlayerInfo->getSlot(), pPCSlayerInfo);
            } else if (race == "VAMPIRE") {
                //----------------------------------------------------------------------


                //
                //    - Name
                //    - Slot
                //    - Sex
                //    - BatColor
                //    - SkinColor


                //    - CurrentHP/MaxHP
                //    - Gold
                //    - ZoneID
                //
                //----------------------------------------------------------------------

                selectVampireStmt.bindString(1, getID());
                selectVampireStmt.bindString(2, name);
                Result* pResult2 = selectVampireStmt.execute();

                if (pResult2->getRowCount() == 0) {
                    throw DisconnectException("No Vampire");
                }

                pResult2->next();


                PCVampireInfo* pPCVampireInfo = new PCVampireInfo();


                uint i = 0;

                pPCVampireInfo->setName(pResult2->getString(++i));
                pPCVampireInfo->setSlot(pResult2->getString(++i));

                string sex = pResult2->getString(++i);
                pPCVampireInfo->setSex(sex);
                pPCVampireInfo->setBatColor(pResult2->getInt(++i));
                pPCVampireInfo->setSkinColor(pResult2->getInt(++i));
                pPCVampireInfo->setAdvancementLevel(pResult2->getInt(++i));
                pPCVampireInfo->setSTR(pResult2->getInt(++i));
                pPCVampireInfo->setDEX(pResult2->getInt(++i));
                pPCVampireInfo->setINT(pResult2->getInt(++i));
                pPCVampireInfo->setHP(pResult2->getInt(++i), pResult2->getInt(++i));
                pPCVampireInfo->setRank(pResult2->getInt(++i));
                pPCVampireInfo->setExp(pResult2->getInt(++i));
                pPCVampireInfo->setLevel(pResult2->getInt(++i));
                pPCVampireInfo->setBonus(pResult2->getInt(++i));
                pPCVampireInfo->setFame(pResult2->getInt(++i));
                pPCVampireInfo->setAlignment(pResult2->getInt(++i));


                shape = pResult2->getDWORD(++i);

                colorsVamp[0] = pResult2->getInt(++i); // CoatColor
                pPCVampireInfo->setShapeInfo(shape, colorsVamp);

                lcPCList.setPCInfo(pPCVampireInfo->getSlot(), pPCVampireInfo);
            } else {
                //----------------------------------------------------------------------


                //
                //    - Name
                //    - Slot
                //    - Sex
                //    - HairColor
                //    - SkinColor


                //    - CurrentHP/MaxHP
                //    - Gold
                //    - ZoneID
                //
                //----------------------------------------------------------------------

                selectOustersStmt.bindString(1, getID());
                selectOustersStmt.bindString(2, name);
                Result* pResult2 = selectOustersStmt.execute();

                if (pResult2->getRowCount() == 0) {
                    throw DisconnectException("No Ousters");
                }

                pResult2->next();


                PCOustersInfo* pPCOustersInfo = new PCOustersInfo();


                uint i = 0;

                pPCOustersInfo->setName(pResult2->getString(++i));
                pPCOustersInfo->setSlot(pResult2->getString(++i));

                string sex = pResult2->getString(++i);
                pPCOustersInfo->setSex(sex);
                pPCOustersInfo->setAdvancementLevel(pResult2->getInt(++i));
                pPCOustersInfo->setSTR(pResult2->getInt(++i));
                pPCOustersInfo->setDEX(pResult2->getInt(++i));
                pPCOustersInfo->setINT(pResult2->getInt(++i));
                pPCOustersInfo->setHP(pResult2->getInt(++i), pResult2->getInt(++i));
                pPCOustersInfo->setRank(pResult2->getInt(++i));
                pPCOustersInfo->setExp(pResult2->getInt(++i));
                pPCOustersInfo->setLevel(pResult2->getInt(++i));
                pPCOustersInfo->setBonus(pResult2->getInt(++i));
                pPCOustersInfo->setSkillBonus(pResult2->getInt(++i));
                pPCOustersInfo->setFame(pResult2->getInt(++i));
                pPCOustersInfo->setAlignment(pResult2->getInt(++i));
                pPCOustersInfo->setCoatType((OustersCoatType)pResult2->getInt(++i));
                pPCOustersInfo->setArmType((OustersArmType)pResult2->getInt(++i));
                pPCOustersInfo->setCoatColor(pResult2->getInt(++i));
                pPCOustersInfo->setHairColor(pResult2->getInt(++i));
                pPCOustersInfo->setArmColor(pResult2->getInt(++i));
                pPCOustersInfo->setBootsColor(pResult2->getInt(++i));

                lcPCList.setPCInfo(pPCOustersInfo->getSlot(), pPCOustersInfo);
            }
        }
    } catch (SQLQueryException& sce) {
        throw DisconnectException(sce.toString());
    }
}
