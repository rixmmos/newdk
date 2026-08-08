//////////////////////////////////////////////////////////////////////////////
// Filename    : GamePlyaer.cpp
// Written By  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GamePlayer.h"

#include <fstream>

#include "Assert.h"
#include "BillingInfo.h"
#include "BillingPlayerManager.h"
#include "CGConnect.h"
#include "Creature.h"
#include "DB.h"
#include "EventKick.h"
#include "GCKickMessage.h"
#include "GCSystemMessage.h"
#include "GSGuildMemberLogOn.h"
#include "Guild.h"
#include "GuildManager.h"
#include "IncomingPlayerManager.h"
#include "LogClient.h"
#include "Ousters.h"
#include "PCFinder.h"
#include "PacketFactoryManager.h"
#include "PacketValidator.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "RelicUtil.h"
#include "ServiceDeadline.h"
#include "SharedServerManager.h"
#include "Slayer.h"
#include "StringPool.h"
#include "TelephoneCenter.h"
#include "Thread.h"
#include "VSDateTime.h"
#include "Vampire.h"
#include "VariableManager.h"
#include "Zone.h"
#include "chinabilling/CBillingInfo.h"
#ifdef __CONNECT_CBILLING_SYSTEM__
#include "chinabilling/CBillingPlayerManager.h"
#endif

#include <stdio.h>


//////////////////////////////////////////////////////////////////////////////

//


//////////////////////////////////////////////////////////////////////////////
// #define __PROFILE_PACKETS__

#ifdef __PROFILE_PACKETS__
#include "Profile.h"
#endif

// by sigi. 2002.11.12
const int defaultGamePlayerInputStreamSize = 1024;
const int defaultGamePlayerOutputStreamSize = 20480;

static int maxIdleSec = 60 * 5;

static int maxVerifyCount = 3;
static int maxTimeGap = 5;
static int SpeedCheckDelay = 60;

const int PCRoomLottoSec = 3600;
const int PCRoomLottoMaxAmount = 3;

void addLogoutPlayerData(Player* pPlayer);


//////////////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////////////

GamePlayer::GamePlayer(Socket* pSocket)
    : // Player(pSocket), 	// by sigi. 2002.11.12
      m_pCreature(NULL), m_PlayerStatus(GPS_NONE), m_pReconnectPacket(NULL), m_Sequence(0) {
    __BEGIN_TRY

    Assert(pSocket != NULL);
    m_pSocket = pSocket;

#ifdef __USE_ENCRYPTER__
    // create socket input stream
    m_pInputStream = new SocketEncryptInputStream(m_pSocket, defaultGamePlayerInputStreamSize);
    Assert(m_pInputStream != NULL);

    // create socket output stream
    m_pOutputStream = new SocketEncryptOutputStream(m_pSocket, defaultGamePlayerOutputStreamSize);
    Assert(m_pOutputStream != NULL);
#else
    // create socket input stream
    m_pInputStream = new SockettInputStream(m_pSocket, defaultGamePlayerInputStreamSize);
    Assert(m_pInputStream != NULL);

    // create socket output stream
    m_pOutputStream = new SockettOutputStream(m_pSocket, defaultGamePlayerOutputStreamSize);
    Assert(m_pOutputStream != NULL);
#endif

    m_Mutex.setName("GamePlayer");

    getCurrentTime(m_ExpireTime);
    m_ExpireTime.tv_sec += maxIdleSec;


    // getCurrentTime(m_SpeedVerify);
    m_SpeedVerify.tv_sec = 0;
    ;
    m_SpeedVerify.tv_usec = 0;
    ;
    getCurrentTime(m_MoveSpeedVerify);
    getCurrentTime(m_AttackSpeedVerify);

    m_VerifyCount = 0;

    m_SpecialEventCount = 0;

    m_bKickForLogin = false;

    m_bFreePass = false;

    m_bMetroFreePlayer = false;

    //	if ( m_bPCRoomPlay )
    //		m_ItemRatioBonusPoint = g_pVariableManager->getPCRoomItemRatioBonusPercent();
    //	else
    m_ItemRatioBonusPoint = 0;

    m_PCRoomLottoStartTime.tv_sec = 0;
    m_PCRoomLottoStartTime.tv_usec = 0;
    m_PCRoomLottoSumTime = 0;

    m_bPacketLog = false;


    m_LoginDateTime = VSDateTime::currentDateTime();

#ifdef __THAILAND_SERVER__
    m_bPermission = false;
#endif

    //	m_NProtectCSAuth.Init();

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////////////

GamePlayer::~GamePlayer() {
    __BEGIN_TRY

    //__ENTER_CRITICAL_SECTION(m_Mutex)


    Assert(m_PlayerStatus == GPS_END_SESSION);

    try {
        // Delete creature
        if (m_pCreature != NULL) {
            if (m_pCreature->hasRelicItem()) {
                dropRelicToZone(m_pCreature, false);
            }

            dropFlagToZone(m_pCreature, false);
            dropSweeperToZone(m_pCreature);

            // try
            //{

            g_pPCFinder->deleteCreature(m_pCreature->getName());
            //}
            // catch (NoSuchElementException & t)
            //{
            //}

#ifdef __CONNECT_BILLING_SYSTEM__

            if (isBillingPlayAvaiable() && !m_bMetroFreePlayer) // by sigi. 2002.11.23
            {
                g_pBillingPlayerManager->sendPayLogout(this);
            }
#elif defined(__CONNECT_CBILLING_SYSTEM__)

            g_pCBillingPlayerManager->sendLogout(this);
#endif

            Statement* pStmt = NULL;


            if (m_pCreature->isSlayer()) {
                Slayer* pSlayer = dynamic_cast<Slayer*>(m_pCreature);
                if (pSlayer->getGuildID() != 99) {
                    Guild* pGuild = g_pGuildManager->getGuild(pSlayer->getGuildID());
                    if (pGuild != NULL) {
                        pGuild->deleteCurrentMember(pSlayer->getName());

                        GSGuildMemberLogOn gsGuildMemberLogOn;
                        gsGuildMemberLogOn.setGuildID(pGuild->getID());
                        gsGuildMemberLogOn.setName(pSlayer->getName());
                        gsGuildMemberLogOn.setLogOn(false);

                        g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);


                        BEGIN_DB {
                            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
                            PreparedStatement updateGuildMemberLogOffStmt(
                                pConn, "UPDATE GuildMember SET LogOn = 0 WHERE Name = ?");
                            updateGuildMemberLogOffStmt.bindString(1, pSlayer->getName());
                            updateGuildMemberLogOffStmt.execute();
                        }
                        END_DB(pStmt)
                    } else
                        filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n",
                                (int)pSlayer->getGuildID(), pSlayer->getName().c_str());
                }
            } else if (m_pCreature->isVampire()) {
                Vampire* pVampire = dynamic_cast<Vampire*>(m_pCreature);
                if (pVampire->getGuildID() != 0) {
                    Guild* pGuild = g_pGuildManager->getGuild(pVampire->getGuildID());
                    if (pGuild != NULL) {
                        pGuild->deleteCurrentMember(pVampire->getName());

                        GSGuildMemberLogOn gsGuildMemberLogOn;
                        gsGuildMemberLogOn.setGuildID(pGuild->getID());
                        gsGuildMemberLogOn.setName(pVampire->getName());
                        gsGuildMemberLogOn.setLogOn(false);

                        g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);


                        BEGIN_DB {
                            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
                            PreparedStatement updateGuildMemberLogOffStmt(
                                pConn, "UPDATE GuildMember SET LogOn = 0 WHERE Name = ?");
                            updateGuildMemberLogOffStmt.bindString(1, pVampire->getName());
                            updateGuildMemberLogOffStmt.execute();
                        }
                        END_DB(pStmt)
                    } else
                        filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n",
                                (int)pVampire->getGuildID(), pVampire->getName().c_str());
                }
            } else if (m_pCreature->isOusters()) {
                Ousters* pOusters = dynamic_cast<Ousters*>(m_pCreature);
                if (pOusters->getGuildID() != 66) {
                    Guild* pGuild = g_pGuildManager->getGuild(pOusters->getGuildID());
                    if (pGuild != NULL) {
                        pGuild->deleteCurrentMember(pOusters->getName());

                        GSGuildMemberLogOn gsGuildMemberLogOn;
                        gsGuildMemberLogOn.setGuildID(pGuild->getID());
                        gsGuildMemberLogOn.setName(pOusters->getName());
                        gsGuildMemberLogOn.setLogOn(false);

                        g_pSharedServerManager->sendPacket(&gsGuildMemberLogOn);


                        BEGIN_DB {
                            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
                            PreparedStatement updateGuildMemberLogOffStmt(
                                pConn, "UPDATE GuildMember SET LogOn = 0 WHERE Name = ?");
                            updateGuildMemberLogOffStmt.bindString(1, pOusters->getName());
                            updateGuildMemberLogOffStmt.execute();
                        }
                        END_DB(pStmt)
                    } else
                        filelog("GuildMissing.log", "[NoSuchGuild] GuildID : %d, Name : %s\n",
                                (int)pOusters->getGuildID(), pOusters->getName().c_str());
                }
            }

            int PartyID = m_pCreature->getPartyID();

            if (PartyID != 0) {
                Zone* pZone = m_pCreature->getZone();
                if (pZone != NULL) {
                    LocalPartyManager* pLocalPartyManager = pZone->getLocalPartyManager();
                    pLocalPartyManager->deletePartyMember(PartyID, m_pCreature);
                }
            }

            SAFE_DELETE(m_pCreature);
            // m_pCreature->setPlayer( NULL );
        }
    } catch (NoSuchElementException& nsee) {
        cerr << "GamePlayer::~GamePlayer() : " << nsee.toString() << endl;
        throw("GamePlayer::~GamePlayer() : NoSuchElementException");
    } catch (Throwable& t) {
        t.addStack(__PRETTY_FUNCTION__);
        throw;
    }


    while (!m_PacketHistory.empty()) {
        Packet* pPacket = m_PacketHistory.front();
        SAFE_DELETE(pPacket);
        m_PacketHistory.pop_front();
    }

    SAFE_DELETE(m_pReconnectPacket);

    //__LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH_NO_RETHROW
}
//////////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////////
void GamePlayer::tv_sub(struct timeval* out, struct timeval* in) {
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}
/*------------- The End -----------*/
//////////////////////////////////////////////////////////////////////
//
// parse packet and execute handler for the packet
//
//////////////////////////////////////////////////////////////////////
void GamePlayer::processCommand(bool Option) {
    __BEGIN_TRY


    char header[szPacketHeader];
    PacketID_t packetID;
    PacketSize_t packetSize;

    SequenceSize_t packetSequence;

    Packet* pPacket = NULL;

    try {
        if (isPenaltyFlag(PENALTY_TYPE_KICKED)) {
            filelog("GamePlayer.txt", "Penalty Kicked. Name[%s],Host[%s],Type[%d]",
                    ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
                    ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()), m_ItemRatioBonusPoint);

            throw DisconnectException("He is had penalty");
        }

        if (Option) {
            m_EventManager.heartbeat();
        }


        while (true) {
            if (!m_pInputStream->peek(&header[0], szPacketHeader)) {
                Timeval currentTime;
                getCurrentTime(currentTime);
                if (currentTime >= m_ExpireTime) {
                    filelog("GamePlayer.txt", "Timeout Disconnect1. Name[%s],Host[%s]",
                            ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
                            ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()));

                    throw DisconnectException("    ,  .");
                }

                break;
            }


            memcpy(&packetID, &header[0], szPacketID);
            memcpy(&packetSize, &header[szPacketID], szPacketSize);


            memcpy(&packetSequence, &header[szPacketID + szPacketSize], szSequenceSize);

            if (packetSequence != m_Sequence) {
                filelog("SequenceError.txt", "Timeout Disconnect1. Name[%s],Host[%s]",
                        ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
                        ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()));
                throw DisconnectException("");
            }
            m_Sequence++;


            if (packetID >= (int)Packet::PACKET_MAX) {
                filelog("GamePlayer.txt", "Packet ID exceed MAX, RECV [%d/%d],ID[%s],Host[%s]", packetID,
                        Packet::PACKET_MAX, m_ID.c_str(),
                        //					getCreature()->getName().c_str(),
                        getSocket()->getHost().c_str());

                throw InvalidProtocolException("too large packet id");
            }

            try {
                if (!g_pPacketValidator->isValidPacketID(getPlayerStatus(), packetID)) {
                    filelog("GamePlayer.txt", "Not Valid Packet, RECV [%d],ID[%s],Host[%s]", packetID, m_ID.c_str(),
                            //						getCreature()->getName().c_str(),
                            getSocket()->getHost().c_str());
                    throw InvalidProtocolException("invalid packet order");
                }


                if (packetID == Packet::PACKET_GC_OTHER_STORE_INFO || packetID == Packet::PACKET_GC_MY_STORE_INFO) {
                    filelog("GamePlayer.txt", "Not Valid Packet, RECV [%d],ID[%s],Host[%s]", packetID, m_ID.c_str(),
                            //						getCreature()->getName().c_str(),
                            getSocket()->getHost().c_str());
                    throw InvalidProtocolException("invalid packet order");
                }


                if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID)) {
                    filelog("GamePlayer.txt", "Too Larget Packet Size, RECV [%d],PacketSize[%d/%d],ID[%s],Host[%s]",
                            packetID, packetSize, g_pPacketFactoryManager->getPacketMaxSize(packetID), m_ID.c_str(),
                            //						getCreature()->getName().c_str(),
                            getSocket()->getHost().c_str());
                    throw InvalidProtocolException("too large packet size");
                }


                if (m_pInputStream->length() < szPacketHeader + packetSize)
                    // throw InsufficientDataException();
                    break;


                getCurrentTime(m_ExpireTime);
                m_ExpireTime.tv_sec += maxIdleSec;


                pPacket = g_pPacketFactoryManager->createPacket(packetID);


                m_pInputStream->readPacket(pPacket);


                m_PacketHistory.push_back(pPacket);


                if (m_bPacketLog) {
                    Timeval currentTime;
                    getCurrentTime(currentTime);

                    if (currentTime >= m_PacketLogEndTime) {
                        m_bPacketLog = false;
                    } else {
                        filelog(m_PacketLogFileName.c_str(), "%s", pPacket->toString().c_str());
                    }
                }

                // cout << "[" << (int)Thread::self() << "] execute before : " << pPacket->getPacketName().c_str() <<
                // endl;


                try {
#ifdef __PROFILE_PACKETS__

                    beginProfileEx(pPacket->getPacketName().c_str());
                    pPacket->execute(this);
                    endProfileEx(pPacket->getPacketName().c_str());

#else
                    verifySpeed(pPacket);
                    pPacket->execute(this);
#endif
                } /*catch ( Throwable& t )
               {
                   filelog( "GPPC.txt", "%s PacketID : %d", t.toString().c_str(), packetID );
                   throw DisconnectException("GamePlayer Error 1!");
               }*/
                catch (...) {
                    filelog("GamePlayerError.txt", "Player:[%s], IP:[%s],MAC:[%02x%02x%02x%02x%02x%02x],Packet is:%s",
                            m_ID.c_str(), getSocket()->getHost().c_str(), m_MacAddress[0], m_MacAddress[1],
                            m_MacAddress[2], m_MacAddress[3], m_MacAddress[4], m_MacAddress[5],
                            pPacket->toString().c_str());
                    throw DisconnectException("GamePlayer Error 2!");
                }
                // cout << "[" << (int)Thread::self() << "] execute after : " << pPacket->getPacketName().c_str() <<
                // endl;


                while (m_PacketHistory.size() > nPacketHistorySize) {
                    Packet* oldPacket = m_PacketHistory.front();
                    SAFE_DELETE(oldPacket);
                    m_PacketHistory.pop_front();
                }
            } catch (IgnorePacketException& igpe) {
                if (packetSize > g_pPacketFactoryManager->getPacketMaxSize(packetID)) {
                    filelog("GamePlayer.txt",
                            "Too Larget Packet Size[Ignore], RECV [%d],PacketSize[%d],Name[%s],Host[%s]", packetID,
                            packetSize, ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
                            ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()));
                    throw InvalidProtocolException("too large packet sizeIgnore");
                }


                if (m_pInputStream->length() < szPacketHeader + packetSize)
                    throw InsufficientDataException();


                m_pInputStream->skip(szPacketHeader + packetSize);
            }
        }
    } catch (InsufficientDataException& ide) {
        Timeval currentTime;
        getCurrentTime(currentTime);
        if (currentTime >= m_ExpireTime) {
            filelog("GamePlayer.txt", "Timeout Diconnect. Name[%s],Host[%s]",
                    ((getCreature() == NULL) ? "NULL" : getCreature()->getName().c_str()),
                    ((getSocket() == NULL) ? "NULL" : getSocket()->getHost().c_str()));

            throw DisconnectException("    ,  .");
        }
    }


    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// flush output buffer to socket's send buffer
//


//
//////////////////////////////////////////////////////////////////////
void GamePlayer::processOutput() {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    int i = 0;

    try {
        Player::processOutput();

        i = 100000;
    } catch (InvalidProtocolException& It) {
        // cerr << "GamePlayer::processOutput Exception Check!!" << endl;
        // cerr << It.toString() << endl;

        throw DisconnectException("Pipe    ");
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//
// send packet to player's output buffer
//
//////////////////////////////////////////////////////////////////////
void GamePlayer::sendPacket(Packet* pPacket) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    try {
        if (m_bPacketLog) {
            Timeval currentTime;
            getCurrentTime(currentTime);

            if (currentTime >= m_PacketLogEndTime) {
                m_bPacketLog = false;
            } else {
                filelog(m_PacketLogFileName.c_str(), "%s", pPacket->toString().c_str());
            }
        }

        Player::sendPacket(pPacket);

        // cout << "GamePlayer::sendPacket() : " << pPacket->toString() << endl;
        // cout << "GamePlayer::sendPacket() PACKET SIZE : " << pPacket->getPacketSize() << endl;

        /*
        if (getCreature() != NULL)
        {
            PacketID_t packetID = pPacket->getPacketID();
            switch (packetID)
            {
                case Packet::PACKET_GC_UPDATE_INFO:
                case Packet::PACKET_GC_MOVE_OK:
                case Packet::PACKET_GC_SET_POSITION:
                    filelog("GamePlayer.txt", "SEND [%s],Name[%s],Host[%s]",
                            pPacket->getPacketName().c_str(),
                            getCreature()->getName().c_str(),
                            getSocket()->getHost().c_str());
                    break;
                default:
                    break;
            }
        }
        else
        {
            PacketID_t packetID = pPacket->getPacketID();
            switch (packetID)
            {
                case Packet::PACKET_GC_UPDATE_INFO:
                case Packet::PACKET_GC_MOVE_OK:
                case Packet::PACKET_GC_SET_POSITION:
                    filelog("GamePlayer.txt", "SEND [%s],Name[NULL],Host[%s]",
                            pPacket->getPacketName().c_str(),
                            getSocket()->getHost().c_str());
                    break;
                default:
                    break;
            }
        }
        */
    } catch (InvalidProtocolException& It) {
        // cout << "GamePlayer::sendPacket Exception Check!!" << endl;
        // cout << It.toString() << endl;
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//--------------------------------------------------------------------------------
//
// disconnect player
//


//
//--------------------------------------------------------------------------------
void GamePlayer::disconnect(bool bDisconnected) {
    __BEGIN_TRY

    __ENTER_CRITICAL_SECTION(m_Mutex)

    //--------------------------------------------------------------------------------


    //--------------------------------------------------------------------------------
    string CreatureName = "";
    if (m_pCreature != NULL) {
        CreatureName = m_pCreature->getName();

        try {
            // *CAUTION*


            if (getPlayerStatus() == GPS_NORMAL) {
                //----------------------------------

                //----------------------------------
                Zone* pZone = m_pCreature->getZone();
                Assert(pZone != NULL);
                pZone->deleteQueuePC(m_pCreature);
                pZone->deleteCreature(m_pCreature, m_pCreature->getX(), m_pCreature->getY());
                //--------------------------------------------------------------------------------

                //--------------------------------------------------------------------------------
                m_pCreature->save();
            }
        } catch (Throwable& t) {
        }
    }

    setPlayerStatus(GPS_END_SESSION);

    //--------------------------------------------------------------------------------

    //--------------------------------------------------------------------------------
    if (m_ID != "") {
        Statement* pStmt1 = NULL;
        Statement* pStmt2 = NULL;

        BEGIN_DB {
            //            pStmt = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();

            // pStmt1 = g_pDatabaseManager->getConnection("DARKEDEN")->createStatement();
            Connection* pConn1 = g_pDatabaseManager->getDistConnection("PLAYER_DB");

            PreparedStatement logoffPlayerStmt(
                pConn1, "UPDATE Player SET LogOn='LOGOFF', LastLogoutDate=now() WHERE PlayerID = ? AND LogOn='GAME'");
            logoffPlayerStmt.bindString(1, m_ID);
            logoffPlayerStmt.execute();


            //	addLogoutPlayerData(this);

            if (logoffPlayerStmt.getAffectedRowCount() == 0) {
            }


#if defined(__PAY_SYSTEM_LOGIN__) || defined(__PAY_SYSTEM_ZONE__) || defined(__PAY_SYSTEM_FREE_LIMIT__)
            if (isPayPlaying() || isPremiumPlay()) {
                logoutPayPlay(m_ID);
            }
#endif
        }
        END_DB(pStmt1)
        BEGIN_DB {
            Connection* pConn2 = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement deleteUserIPInfoStmt(pConn2, "DELETE FROM UserIPInfo WHERE Name = ?");
            deleteUserIPInfoStmt.bindString(1, CreatureName);
            deleteUserIPInfoStmt.execute();
        }
        END_DB(pStmt2)
    }


    if (m_pReconnectPacket != NULL) {
        // cout << "[SendReconnect] " << m_pReconnectPacket->toString().c_str() << endl;

        try {
            // sendPacket( m_pReconnectPacket );
            Player::sendPacket(m_pReconnectPacket);

            m_pOutputStream->flush();
        } catch (Throwable& t) {
        }

        SAFE_DELETE(m_pReconnectPacket);
    }


    if (bDisconnected == UNDISCONNECTED) {
        try {
            // GCDisconnect gcDisconnect;
            // sendPacket(gcDisconnect);


            m_pOutputStream->flush();
        } catch (Throwable& t) {
            // cerr << "GamePlayer::disconnect() : GamePlayer::disconnect Exception Check!!" << endl;
            // cerr << t.toString() << endl;
        }
    }


    m_pSocket->close();

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//

//

//
//////////////////////////////////////////////////////////////////////
Packet* GamePlayer::getOldPacket(uint prev) {
    __BEGIN_TRY

    if (prev >= nPacketHistorySize)
        throw OutOfBoundException();

    // if prev == 0 , return m_PacketHistory[9]
    // if prev == 9 , return m_PacketHistory[0]
    Packet* pPacket = m_PacketHistory[nPacketHistorySize - prev - 1];

    if (pPacket == NULL)
        throw NoSuchElementException("packet history is empty");

    return pPacket;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
//

//
//////////////////////////////////////////////////////////////////////
Packet* GamePlayer::getOldPacket(PacketID_t packetID) {
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
        throw NoSuchElementException("packet history is empty");

    return pPacket;

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void GamePlayer::addEvent(Event* pEvent) {
    __BEGIN_TRY

    m_EventManager.addEvent(pEvent);

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void GamePlayer::deleteEvent(Event::EventClass EClass) {
    __BEGIN_TRY

    m_EventManager.deleteEvent(EClass);

    __END_CATCH
}

//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
Event* GamePlayer::getEvent(Event::EventClass EClass) {
    __BEGIN_TRY

    return m_EventManager.getEvent(EClass);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// get debug string
//
//////////////////////////////////////////////////////////////////////
string GamePlayer::toString() const {
    __BEGIN_TRY

    StringStream msg;

    //////////////////////////////////////////////////
    // enter critical section
    //////////////////////////////////////////////////
    __ENTER_CRITICAL_SECTION(m_Mutex)

    msg << "GamePlayer(SocketID:" << m_pSocket->getSOCKET() << ",Host:" << m_pSocket->getHost() << ")";

    //////////////////////////////////////////////////
    // leave critical section
    //////////////////////////////////////////////////
    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return msg.toString();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
//
// verifySpeed
//
//////////////////////////////////////////////////////////////////////
bool GamePlayer::verifySpeed(Packet* pPacket) {
    __BEGIN_TRY
    PacketID_t PacketID = pPacket->getPacketID();

    bool SpeedCheck = false;

    Timeval CurrentTime;
    getCurrentTime(CurrentTime);

    //////////////////////////////////////////////////////////////////////////


    //

    if (PacketID == Packet::PACKET_CG_VERIFY_TIME) {
        if (m_SpeedVerify.tv_sec == 0) {
            m_SpeedVerify.tv_sec = CurrentTime.tv_sec + SpeedCheckDelay;

            SpeedCheck = true;
        } else {
            if (CurrentTime.tv_sec > m_SpeedVerify.tv_sec - maxTimeGap) {
                m_SpeedVerify.tv_sec = CurrentTime.tv_sec + SpeedCheckDelay;

                SpeedCheck = true;

                m_VerifyCount = max(0, m_VerifyCount - 1);
            } else {
                m_SpeedVerify.tv_sec = CurrentTime.tv_sec + SpeedCheckDelay;

                if (m_VerifyCount > maxVerifyCount) {
                    SpeedCheck = false;
                } else {
                    SpeedCheck = true;
                }
                m_VerifyCount++;
            }
        }
    }
    //

    // Add by Coffee 2007-6-25 kf_168@hotmail.com
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////

    // Add by Coffee 2007-6-25 E-mail: kf_168@hotmail.com
    if (PacketID == Packet::PACKET_CG_MOVE) {
        if (CurrentTime <= m_MoveSpeedVerify) {
        }
        // Timeval UseTimer=CurrentTime-m_MoveSpeedVerify;
        tv_sub(&CurrentTime, &m_MoveSpeedVerify);
        double rtt;

        rtt = CurrentTime.tv_sec * 1000 + CurrentTime.tv_usec / 1000;

        getCurrentTime(m_MoveSpeedVerify);
        // add by viva for notice
        // filelog("MoveLog.txt", "MoveTime:=%.3f ms\n",rtt);
        // end
    }

    // End by Coffee
    //////////////////////////////////////////////////////////////////////////


    return SpeedCheck;

    __END_CATCH
}

void GamePlayer::loadSpecialEventCount(void) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        //		pConn   = g_pDatabaseManager->getConnection("DARKEDEN");
        Connection* pConn = g_pDatabaseManager->getDistConnection("PLAYER_DB");

        PreparedStatement selectSpecialEventCountStmt(pConn, "SELECT SpecialEventCount FROM Player WHERE PlayerID=?");
        selectSpecialEventCountStmt.bindString(1, m_ID);
        Result* pResult = selectSpecialEventCountStmt.execute();

        if (pResult->getRowCount() != 0) {
            pResult->next();
            m_SpecialEventCount = pResult->getDWORD(1);
        } else {
            throw("GamePlayer::loadSpecialEventCount() : unable to dispatch data");
            return;
        }
    }
    END_DB(pStmt);

    __END_CATCH
}

void GamePlayer::saveSpecialEventCount(void) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getDistConnection("PLAYER_DB");
        //		pConn = g_pDatabaseManager->getConnection( (int)Thread::self() );

        PreparedStatement updateSpecialEventCountStmt(pConn, "UPDATE Player SET SpecialEventCount=? WHERE PlayerID=?");
        updateSpecialEventCountStmt.bindUInt(1, m_SpecialEventCount);
        updateSpecialEventCountStmt.bindString(2, m_ID);
        updateSpecialEventCountStmt.execute();
    }
    END_DB(pStmt);

    __END_CATCH
}

bool GamePlayer::sendBillingLogin() {
    __BEGIN_TRY

    if (!m_ID.empty() && m_ID != "NONE") {
        Timeval currentTime;
        getCurrentTime(currentTime);

        if (currentTime > m_BillingNextLoginRequestTime) {
            g_pBillingPlayerManager->sendPayLogin(this);

            Timeval afterTime;
            getCurrentTime(afterTime);


            if (afterTime.tv_sec > currentTime.tv_sec + 1) {
                filelog("billingLoginTime.txt", "PlayerID : %s, CallTime : %d sec, Try : %d", m_ID.c_str(),
                        (int)(afterTime.tv_sec - currentTime.tv_sec), m_BillingLoginRequestCount);
            }


            m_BillingLoginRequestCount++;


            m_BillingNextLoginRequestTime.tv_sec = currentTime.tv_sec + 60;
        }

        return true;
    }

    return false;

    __END_CATCH
}

void GamePlayer::sendCBillingPayInfo() {
    __BEGIN_TRY

    char m[200];
    if (m_CBPlayerType == CBILLING_PLAYER_TYPE_MONTHLY) {
        VSDate currentDate;
        currentDate = VSDate::currentDate();

        sprintf(m, g_pStringPool->c_str(STRID_CB_MONTHLY_PLAYER), m_CBEndDateTime.date().year(),
                m_CBEndDateTime.date().month(), m_CBEndDateTime.date().day(), currentDate.year(), currentDate.month(),
                currentDate.day());

        GCSystemMessage msg;
        msg.setMessage(m);

        sendPacket(&msg);
    } else if (m_CBPlayerType == CBILLING_PLAYER_TYPE_POINT) {
        sprintf(m, g_pStringPool->c_str(STRID_CB_POINT_PLAYER), m_CBLeftTime / 10);

        GCSystemMessage msg;
        msg.setMessage(m);

        sendPacket(&msg);
    }

    __END_CATCH
}


void GamePlayer::setEncryptCode() {
    __BEGIN_TRY

#ifdef __USE_ENCRYPTER__
    Assert(m_pCreature != NULL);


    // ObjectID_t 	objectID 	= m_pCreature->getObjectID();


    //	ZoneID_t 	zoneID 		= m_pCreature->getZone()->getZoneID();
    //	static int	serverID	= g_pConfig->getPropertyInt("ServerID");

    //	if (objectID!=0)
    //	{
    // uchar code = (uchar)(objectID / zoneID + objectID);
    // uchar code = (uchar)( ( ( zoneID >> 8 ) ^ zoneID ) ^ ( ( serverID + 1 ) << 4 ) );
    uchar code = m_pCreature->getZone()->getEncryptCode();

#ifdef __ACTIVE_SERVICE_DEADLINE__

    VSDate date = VSDate::currentDate();

    if (date.year() >= DEADLINE_YEAR && date.month() > DEADLINE_MONTH)
        code += (date.year() + date.month()) / 11;
#endif

    SocketEncryptOutputStream* pEOS = dynamic_cast<SocketEncryptOutputStream*>(m_pOutputStream);
    Assert(pEOS != NULL);

    SocketEncryptInputStream* pEIS = dynamic_cast<SocketEncryptInputStream*>(m_pInputStream);
    Assert(pEIS != NULL);

    pEOS->setEncryptCode(code);
    pEIS->setEncryptCode(code);
//	}
#endif

    __END_CATCH
}

void GamePlayer::kickPlayer(uint nSeconds, uint KickMessageType) {
    __BEGIN_TRY


    if (m_EventManager.getEvent(Event::EVENT_CLASS_KICK) != NULL)
        return;

    EventKick* pEventKick = new EventKick(this);
    pEventKick->setDeadline(nSeconds * 10);
    addEvent(pEventKick);


    GCKickMessage gcKickMessage;
    gcKickMessage.setType(KickMessageType);
    gcKickMessage.setSeconds(nSeconds);
    sendPacket(&gcKickMessage);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
bool GamePlayer::loginPayPlay(PayType payType, const string& PayPlayDate, int PayPlayHours, uint payPlayFlag,
                              const string& ip, const string& playerID) {
    __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
    return BillingPlayerInfo::isBillingPlayAvaiable();
#else
    return PaySystem::loginPayPlay(payType, PayPlayDate, PayPlayHours, payPlayFlag, ip, playerID);
#endif
    __END_CATCH
}

bool GamePlayer::loginPayPlay(const string& ip, const string& playerID) {
    __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
    return BillingPlayerInfo::isBillingPlayAvaiable();
#else
    bool bRet = PaySystem::loginPayPlay(ip, playerID);

    if (bRet)
        setPCRoomLottoStartTime();

    return bRet;
#endif
    __END_CATCH
}

bool GamePlayer::updatePayPlayTime(const string& playerID, const VSDateTime& currentDateTime,
                                   const Timeval& currentTime) {
    __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
    return BillingPlayerInfo::isBillingPlayAvaiable();
#else
    checkPCRoomLotto(currentTime);

    return PaySystem::updatePayPlayTime(playerID, currentDateTime, currentTime);
#endif
    __END_CATCH
}

void GamePlayer::logoutPayPlay(const string& playerID, bool bClear, bool bDecreaseTime) {
    __BEGIN_TRY
#ifdef __CONNECT_BILLING_SYSTEM__
    if (!BillingPlayerInfo::isBillingPlayAvaiable())
        setPremiumPlay(false);
#else
    savePCRoomLottoTime();

    PaySystem::logoutPayPlay(playerID, bClear, bDecreaseTime);
#endif
    __END_CATCH
}

bool GamePlayer::isPayPlaying() const {
#ifdef __CONNECT_BILLING_SYSTEM__
    return BillingPlayerInfo::isBillingPlayAvaiable();
#elif defined(__CONNECT_CBILLING_SYSTEM__)
    return CBillingPlayerInfo::isPayPlayer();
#elif !defined(__PAY_SYSTEM_ZONE__) && !defined(__PAY_SYSTEM_LOGIN__) && !defined(__PAY_SYSTEM_FREE_LIMIT__)
    // if there is no Pay defines, all users are pay player
    return true;
#else
    return PaySystem::isPayPlaying();
#endif
}

//////////////////////////////////////////////////////////////////////////////
//
// add LogoutPlayerdata
//


//
//////////////////////////////////////////////////////////////////////////////
void addLogoutPlayerData(Player* pPlayer) {
    Statement* pStmt = NULL;

    Connection* pConn = g_pDatabaseManager->getUserInfoConnection();


    BEGIN_DB {
        string ID = pPlayer->getID();
        string ip = pPlayer->getSocket()->getHost();


        int year, month, day, hour, minute, second;
        getCurrentTimeEx(year, month, day, hour, minute, second);
        string currentDT = VSDateTime::currentDateTime().toDateTime();

        PreparedStatement insertLogoutPlayerDataStmt(
            pConn, "INSERT INTO USERINFO.LogoutPlayerData (PlayerID,IP,Date,Time) VALUES (?, ?, ?, ?)");
        insertLogoutPlayerDataStmt.bindString(1, ID);
        insertLogoutPlayerDataStmt.bindString(2, ip);
        insertLogoutPlayerDataStmt.bindString(3, currentDT.substr(0, 10));
        insertLogoutPlayerDataStmt.bindString(4, currentDT.substr(11));
        insertLogoutPlayerDataStmt.execute();
    }
    END_DB(pStmt)
}

void GamePlayer::setPCRoomLottoStartTime() {
    if (!g_pVariableManager->isPCRoomLottoEvent())
        return;

    if (!m_bPCRoomPlay)
        return;

    m_PCRoomLottoStartTime.tv_sec = m_PayPlayStartTime.tv_sec - m_PCRoomLottoSumTime;
}

void GamePlayer::savePCRoomLottoTime() {
    if (!g_pVariableManager->isPCRoomLottoEvent())
        return;

    if (!m_bPCRoomPlay)
        return;

    Timeval currentTime;
    getCurrentTime(currentTime);

    m_PCRoomLottoSumTime = currentTime.tv_sec - m_PCRoomLottoStartTime.tv_sec;
    m_PCRoomLottoStartTime.tv_sec = 0;
}

void GamePlayer::checkPCRoomLotto(const Timeval& currentTime) {
    if (!g_pVariableManager->isPCRoomLottoEvent())
        return;

    if (!m_bPCRoomPlay)
        return;

    if (m_PCRoomLottoStartTime.tv_sec == 0) {
        m_PCRoomLottoStartTime.tv_sec = currentTime.tv_sec - m_PCRoomLottoSumTime;
    }

    int time = currentTime.tv_sec - m_PCRoomLottoStartTime.tv_sec;

    if (time >= PCRoomLottoSec) {
        giveLotto();


        m_PCRoomLottoStartTime.tv_sec = currentTime.tv_sec;
        m_PCRoomLottoSumTime = 0;
    }
}

void GamePlayer::giveLotto() {
    if (m_pCreature == NULL)
        return;

    Statement* pStmt = NULL;

    BEGIN_DB {
        static uint DimensionID = g_pConfig->getPropertyInt("Dimension");
        static uint WorldID = g_pConfig->getPropertyInt("WorldID");
        string PlayerID = getID();
        string Name = m_pCreature->getName();
        Race_t Race = m_pCreature->getRace();
        int Amount = 0;
        Connection* pConn = g_pDatabaseManager->getDistConnection("PLAYER_DB");

        PreparedStatement selectLottoAmountStmt(pConn,
                                                "SELECT Amount FROM PCRoomLottoObject WHERE PlayerID = ? AND Name = ? "
                                                "AND DimensionID = ? AND WorldID = ?");
        selectLottoAmountStmt.bindString(1, PlayerID);
        selectLottoAmountStmt.bindString(2, Name);
        selectLottoAmountStmt.bindUInt(3, DimensionID);
        selectLottoAmountStmt.bindUInt(4, WorldID);
        Result* pResult = selectLottoAmountStmt.execute();

        if (pResult->next()) {
            Amount = pResult->getInt(1);

            if (Amount < PCRoomLottoMaxAmount) {
                PreparedStatement updateLottoAmountStmt(
                    pConn, "UPDATE PCRoomLottoObject SET Amount = ? WHERE PlayerID = ? AND Name = ? "
                           "AND DimensionID = ? AND WorldID = ?");
                updateLottoAmountStmt.bindInt(1, Amount + 1);
                updateLottoAmountStmt.bindString(2, PlayerID);
                updateLottoAmountStmt.bindString(3, Name);
                updateLottoAmountStmt.bindUInt(4, DimensionID);
                updateLottoAmountStmt.bindUInt(5, WorldID);
                updateLottoAmountStmt.execute();
            }
        } else {
            PreparedStatement insertLottoStmt(pConn, "INSERT INTO PCRoomLottoObject VALUES ( 0, ?, ?, ?, ?, ?, ?, 1 )");
            insertLottoStmt.bindUInt(1, m_PCRoomID);
            insertLottoStmt.bindString(2, PlayerID);
            insertLottoStmt.bindUInt(3, DimensionID);
            insertLottoStmt.bindUInt(4, WorldID);
            insertLottoStmt.bindString(5, Name);
            insertLottoStmt.bindUInt(6, (uint)Race);
            insertLottoStmt.execute();
        }

        if (Amount < PCRoomLottoMaxAmount) {
            char msg[100];
            sprintf(msg, g_pStringPool->c_str(STRID_GIVE_LOTTO), Amount + 1);

            GCSystemMessage gcMsg;
            gcMsg.setMessage(msg);
            sendPacket(&gcMsg);

            if (Amount >= PCRoomLottoMaxAmount - 1) {
                gcMsg.setMessage(g_pStringPool->getString(STRID_CANNOT_GIVE_LOTTO));
                sendPacket(&gcMsg);
            }
        }
    }
    END_DB(pStmt)
}

bool GamePlayer::startPacketLog(uint sec) {
    if (m_pCreature == NULL)
        return false;

    m_bPacketLog = true;
    getCurrentTime(m_PacketLogEndTime);
    m_PacketLogEndTime.tv_sec += sec;

    char filename[100];
    sprintf(filename, "log/%s.log", m_pCreature->getName().c_str());
    m_PacketLogFileName = filename;

    return true;
}

void GamePlayer::logLoginoutDateTime() {
    if (m_pCreature == NULL)
        return;


    uint dimensionID = g_pConfig->getPropertyInt("Dimension");
    if (g_pConfig->getPropertyInt("IsNetMarble") == 0) {
        dimensionID = 2;
    }

    // WorldID
    uint worldID = g_pConfig->getPropertyInt("WorldID");


    uint racecode;
    uint str, dex, inte;
    if (m_pCreature->isSlayer()) {
        Slayer* pSlayer = dynamic_cast<Slayer*>(m_pCreature);
        Assert(pSlayer != NULL);

        racecode = (uint)pSlayer->getHighestSkillDomain();

        str = pSlayer->getSTR();
        dex = pSlayer->getDEX();
        inte = pSlayer->getINT();
    } else if (m_pCreature->isVampire()) {
        racecode = 10;

        Vampire* pVampire = dynamic_cast<Vampire*>(m_pCreature);
        Assert(pVampire != NULL);

        str = pVampire->getSTR();
        dex = pVampire->getDEX();
        inte = pVampire->getINT();
    } else if (m_pCreature->isOusters()) {
        racecode = 20;

        Ousters* pOusters = dynamic_cast<Ousters*>(m_pCreature);
        Assert(pOusters != NULL);

        str = pOusters->getSTR();
        dex = pOusters->getDEX();
        inte = pOusters->getINT();
    } else {
        return;
    }


    uint level = (uint)m_pCreature->getLevel();


    VSDateTime logoutDateTime = VSDateTime::currentDateTime();

    // filename
    char filename[20];
    sprintf(filename, "log/%s.txt", logoutDateTime.toStringforWeb().c_str());

    try {
        ofstream file(filename, ios::out | ios::app);
        file << dimensionID << "\t" << worldID << "\t" << m_ID << "\t" << m_pCreature->getName() << "\t" << racecode
             << "\t" << level << "\t" << str << "\t" << dex << "\t" << inte << "\t" << m_LoginDateTime.toDateTime()
             << "\t" << logoutDateTime.toDateTime() << endl;
        file.close();
    } catch (...) {
    }
}
