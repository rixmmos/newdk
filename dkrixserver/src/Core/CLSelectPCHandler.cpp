//////////////////////////////////////////////////////////////////////////////
// Filename    : CLSelectPCHandler.cpp
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CLSelectPC.h"

#ifdef __LOGIN_SERVER__
#include "Assert1.h"
#include "DB.h"
#include "GameServerInfo.h"
#include "GameServerInfoManager.h"
#include "GameServerManager.h"
#include "LCReconnect.h"
#include "LCSelectPCError.h"
#include "LGIncomingConnection.h"
#include "LoginPlayer.h"
#include "PreparedStatement.h"
#include "Properties.h"
#include "ZoneGroupInfoManager.h"
#include "ZoneInfoManager.h"
#include "chinabilling/CBillingInfo.h"
#include "gameserver/billing/BillingInfo.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// CLSelectPCHandler::execute()
//


//////////////////////////////////////////////////////////////////////////////
void CLSelectPCHandler::execute(CLSelectPC* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __LOGIN_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    LoginPlayer* pLoginPlayer = dynamic_cast<LoginPlayer*>(pPlayer);

    bool bCheckATTR = false;


#ifdef __NETMARBLE_SERVER__
    if (!pLoginPlayer->isAgree()) {
        LCSelectPCError lcSelectPCError;
        lcSelectPCError.setCode(SELECT_PC_DIDNOT_AGREE);
        pLoginPlayer->sendPacket(&lcSelectPCError);

        return;
    }
#endif

#ifdef __CONNECT_CBILLING_SYSTEM__
    if (pLoginPlayer->isCBillingVerified()) {
        if (!pLoginPlayer->isPayPlayer()) {
            LCSelectPCError lcSelectPCError;
            lcSelectPCError.setCode(SELECT_PC_CANNOT_PLAY);
            pLoginPlayer->sendPacket(&lcSelectPCError);

            return;
        }
    } else {
        LCSelectPCError lcSelectPCError;
        lcSelectPCError.setCode(SELECT_PC_NOT_BILLING_CHECK);
        pLoginPlayer->sendPacket(&lcSelectPCError);

        return;
    }

#endif


#ifdef __PAY_SYSTEM_FREE_LIMIT__

    if (!pLoginPlayer->isPayPlaying()) {
        bCheckATTR = true;
    }
#endif


    if (pLoginPlayer->getPlayerStatus() != LPS_PC_MANAGEMENT)
        throw DisconnectException("invalid player status");


    Result* pResult;
    WorldID_t WorldID = pLoginPlayer->getWorldID();

    try {
        Connection* pConn1 = g_pDatabaseManager->getConnection("DARKEDEN"); // (!)
        Connection* pConn = g_pDatabaseManager->getConnection((int)WorldID);

        //----------------------------------------------------------------------

        //----------------------------------------------------------------------


        /*
        pResult = pStmt->executeQuery(
            "SELECT ZoneID, Slot FROM %s WHERE Name = '%s' AND PlayerID = '%s' AND Active = 'ACTIVE'",
            pPacket->getPCType() == PC_SLAYER ? "Slayer" : "Vampire" ,
            pPacket->getPCName().c_str() ,
            pLoginPlayer->getID().c_str()
        );
        */

        bool isSlayer = (pPacket->getPCType() == PC_SLAYER);
        bool isVampire = (pPacket->getPCType() == PC_VAMPIRE);

        // A PreparedStatement owns the Result that execute() hands back and
        // deletes it in its destructor, so the statement must outlive every use
        // of pResult -- which runs to the getInt(4) below. Select the query
        // here and construct the statement in the same scope as those uses:
        // declaring it inside the branches frees the Result at the branch's
        // closing brace and leaves pResult dangling. Same defect as Bug 18-B in
        // CLLoginHandler; all three branches bind the same two parameters.
        const char* pcSelectSql = NULL;
        if (isSlayer) {
            pcSelectSql = "SELECT ZoneID, Slot, GREATEST(SwordLevel,BladeLevel,GunLevel,EnchantLevel,HealLevel), "
                          "Competence FROM Slayer WHERE Name = ? AND PlayerID = ? AND Active = 'ACTIVE'";
        } else if (isVampire) {
            pcSelectSql = "SELECT ZoneID, Slot, Level, Competence FROM Vampire WHERE Name = ? "
                          "AND PlayerID = ? AND Active = 'ACTIVE'";
        } else {
            pcSelectSql = "SELECT ZoneID, Slot, Level, Competence FROM Ousters WHERE Name = ? "
                          "AND PlayerID = ? AND Active = 'ACTIVE'";
        }

        PreparedStatement pcSelectStmt(pConn, pcSelectSql);
        pcSelectStmt.bindString(1, pPacket->getPCName());
        pcSelectStmt.bindString(2, pLoginPlayer->getID());
        pResult = pcSelectStmt.execute();


        if (pResult->getRowCount() != 1) {
            throw InvalidProtocolException("no such PC exist.");
        }

        pResult->next();

        ZoneID_t zoneID = pResult->getWORD(1);
        string slotStr = pResult->getString(2);


        if (bCheckATTR) {
#ifdef __PAY_SYSTEM_FREE_LIMIT__
            if (isSlayer) {
                static int slayerSum = g_pConfig->getPropertyInt("FreePlaySlayerDomainSum");

                int DomainSUM = pResult->getInt(3);

                if (DomainSUM > slayerSum) {
                    LCSelectPCError lcSelectPCError;
                    lcSelectPCError.setCode(SELECT_PC_CANNOT_PLAY_BY_ATTR);
                    pLoginPlayer->sendPacket(&lcSelectPCError);
                    return;
                }
            } else {
                static int vampireLevel = g_pConfig->getPropertyInt("FreePlayVampireLevel");

                int Level = pResult->getInt(3);

                if (Level > vampireLevel) {
                    LCSelectPCError lcSelectPCError;
                    lcSelectPCError.setCode(SELECT_PC_CANNOT_PLAY_BY_ATTR);
                    pLoginPlayer->sendPacket(&lcSelectPCError);
                    return;
                }
            }
#endif
        }

        //////////////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////////////
        bool bNonPKServer =
            g_pGameServerInfoManager->getGameServerInfo(1, pLoginPlayer->getServerGroupID(), pLoginPlayer->getWorldID())
                ->isNonPKServer();
        if (bNonPKServer) {
            cout << "WorldID:" << (int)(pLoginPlayer->getWorldID())
                 << " ServerGroupID:" << (int)(pLoginPlayer->getServerGroupID()) << endl;

            int playerLevel = pResult->getInt(3);
            int competence = pResult->getInt(4);


            if (playerLevel > 80 && competence == 3) {
                LCSelectPCError lcSelectPCError;
                lcSelectPCError.setCode(SELECT_PC_CANNOT_PLAY_BY_ATTR);
                pLoginPlayer->sendPacket(&lcSelectPCError);
                return;
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////

        if (slotStr.size() != 5) {
            throw InvalidProtocolException("no slot exist.");
        }

        int slot = slotStr.at(4) - '0';

        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        GameServerInfo* pGameServerInfo;
        if (zoneID > 10000 && zoneID < 30000) {
            pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo(1, pLoginPlayer->getServerGroupID(), WorldID);
        } else {
            ZoneInfo* pZoneInfo = g_pZoneInfoManager->getZoneInfo(zoneID);
            ZoneGroupInfo* pZoneGroupInfo = g_pZoneGroupInfoManager->getZoneGroupInfo(pZoneInfo->getZoneGroupID());
            // cout << "ConnctServerGroup: " << (int)pLoginPlayer->getServerGroupID() << ", ServerID: " <<
            // (int)pZoneGroupInfo->getServerID();
            cout << "WorldID " << (int)WorldID << ", ServerGroupID : " << (int)pLoginPlayer->getServerGroupID()
                 << ", ServerID : " << (int)pZoneGroupInfo->getServerID() << endl;
            pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo(pZoneGroupInfo->getServerID(),
                                                                          pLoginPlayer->getServerGroupID(), WorldID);
        }

        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        //		GameServerInfo* pGameServerInfo = g_pGameServerInfoManager->getGameServerInfo(pPlayer->getServerID());


        //----------------------------------------------------------------------

        //----------------------------------------------------------------------
        LGIncomingConnection lgIncomingConnection;
        lgIncomingConnection.setClientIP(pLoginPlayer->getSocket()->getHost());
        lgIncomingConnection.setPlayerID(pLoginPlayer->getID());
        lgIncomingConnection.setPCName(pPacket->getPCName());

        //--------------------------------------------------------------------------------
        //
        // *CAUTION*
        //


        //
        //--------------------------------------------------------------------------------
        pLoginPlayer->setPlayerStatus(LPS_AFTER_SENDING_LG_INCOMING_CONNECTION);

        // GameServerInfo.IP is advertised to the client. In this local Docker setup
        // loginserver and gameserver still talk to each other inside one container.
        const string internalGameServerIP = "127.0.0.1";

        // by tiancaiamao: when gameserver is behind docker, it may have a docker internal IP 172.20.0.1 and a outside
        // IP in database GameServerInfo table. The outside IP should be used.
        pLoginPlayer->setGameServerIP(pGameServerInfo->getIP());


        if (g_pConfig->getProperty("User") == "excel96")
            // g_pGameServerManager->sendPacket(pGameServerInfo->getIP() ,
            // g_pConfig->getPropertyInt("GameServerUDPPort"), &lgIncomingConnection);
            g_pGameServerManager->sendPacket(internalGameServerIP, pGameServerInfo->getUDPPort(),
                                             &lgIncomingConnection);
        else if (g_pConfig->getProperty("User") == "beowulf")
            g_pGameServerManager->sendPacket(internalGameServerIP, g_pConfig->getPropertyInt("GameServerUDPPort"),
                                             &lgIncomingConnection);
        //			g_pGameServerManager->sendPacket(pGameServerInfo->getIP() , 8886 , &lgIncomingConnection);
        else if (g_pConfig->getProperty("User") == "crazydog")
            g_pGameServerManager->sendPacket(internalGameServerIP, g_pConfig->getPropertyInt("GameServerUDPPort"),
                                             &lgIncomingConnection);
        //			g_pGameServerManager->sendPacket(pGameServerInfo->getIP() , 6665, &lgIncomingConnection);
        else if (g_pConfig->getProperty("User") == "elcastle") {
            cout << "gameserver ip: " << pGameServerInfo->getIP()
                 << ", port: " << g_pConfig->getPropertyInt("GameServerUDPPort") << endl;
            g_pGameServerManager->sendPacket(internalGameServerIP, g_pConfig->getPropertyInt("GameServerUDPPort"),
                                             &lgIncomingConnection);
            //			g_pGameServerManager->sendPacket(pGameServerInfo->getIP() , 8885 , &lgIncomingConnection);
        } else if (g_pConfig->getProperty("User") == "elca")
            g_pGameServerManager->sendPacket(internalGameServerIP, g_pConfig->getPropertyInt("GameServerUDPPort"),
                                             &lgIncomingConnection);
        //			g_pGameServerManager->sendPacket(pGameServerInfo->getIP() , 3335 , &lgIncomingConnection);


        PreparedStatement playerUpdateStmt( // (!)
            pConn1, "UPDATE Player Set CurrentWorldID = ?, CurrentServerGroupID = ?, LastSlot = ? WHERE PlayerID = ?");
        playerUpdateStmt.bindInt(1, (int)WorldID);
        playerUpdateStmt.bindInt(2, pLoginPlayer->getServerGroupID());
        playerUpdateStmt.bindInt(3, slot);
        playerUpdateStmt.bindString(4, pLoginPlayer->getID());
        playerUpdateStmt.execute();

        PreparedStatement slayerGroupStmt(pConn, "UPDATE Slayer Set ServerGroupID = ? WHERE Name=?");
        slayerGroupStmt.bindInt(1, pLoginPlayer->getServerGroupID());
        slayerGroupStmt.bindString(2, pPacket->getPCName());
        slayerGroupStmt.execute();

        PreparedStatement vampireGroupStmt(pConn, "UPDATE Vampire Set ServerGroupID = ? WHERE Name=?");
        vampireGroupStmt.bindInt(1, pLoginPlayer->getServerGroupID());
        vampireGroupStmt.bindString(2, pPacket->getPCName());
        vampireGroupStmt.execute();

        PreparedStatement oustersGroupStmt(pConn, "UPDATE Ousters Set ServerGroupID = ? WHERE Name=?");
        oustersGroupStmt.bindInt(1, pLoginPlayer->getServerGroupID());
        oustersGroupStmt.bindString(2, pPacket->getPCName());
        oustersGroupStmt.execute();

        // cout << "CLSelectPC SendPacket to Server IP : " << pGameServerInfo->getIP() << endl;
    } catch (SQLQueryException& sqe) {
        // cout << sqe.toString() << endl;

        throw DisconnectException(sqe.toString());
    } catch (NoSuchElementException& nsee) {
        StringStream msg;

        msg << "Critical Error : data intergrity broken at ZoneInfo - ZoneGroupInfo - GameServerInfo : "
            << nsee.toString();


        throw Error(msg.toString());
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
