#include "GuildUnion.h"

#include <stdio.h>

#include "DB.h"
#include "GCModifyInformation.h"
#include "GGCommand.h"
#include "GameServer.h"
#include "GameServerInfoManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "LoginServerManager.h"
#include "PCFinder.h"
#include "PacketUtil.h"
#include "Player.h"
#include "PreparedStatement.h"
#include "VariableManager.h"
GuildUnion::~GuildUnion() {
    // cout << "GuildUnion : DELETE!!!" << endl;
}

bool GuildUnion::hasGuild(GuildID_t gID) const {
    if (gID == m_MasterGuildID)
        return true;

    if (findGuildItr(gID) != m_Guilds.end())
        return true;

    return false;
}

bool GuildUnion::addGuild(GuildID_t gID) {
    if (hasGuild(gID))
        return false;

    m_Guilds.push_back(gID);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement insertStmt(pConn, "INSERT INTO GuildUnionMember (UnionID, OwnerGuildID) VALUES (?, ?)");
        insertStmt.bindUInt(1, m_UnionID);
        insertStmt.bindUInt(2, gID);
        insertStmt.execute();
    }
    END_DB(pStmt);

    return true;
}

bool GuildUnion::removeGuild(GuildID_t gID) {
    if (m_MasterGuildID == gID)
        return false;

    list<GuildID_t>::iterator itr = findGuildItr(gID);
    if (itr == m_Guilds.end())
        return false;

    m_Guilds.erase(itr);

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement deleteStmt(pConn, "DELETE FROM GuildUnionMember WHERE UnionID = ? and OwnerGuildID = ?");
        deleteStmt.bindUInt(1, m_UnionID);
        deleteStmt.bindUInt(2, gID);
        deleteStmt.execute();
        if (deleteStmt.getAffectedRowCount() < 1) {
            filelog("GuildUnion.log", "[%u:%u]    .", m_UnionID, gID);
        }
    }
    END_DB(pStmt);

    return true;
}

void GuildUnion::create() {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement insertUnionStmt(pConn, "INSERT INTO GuildUnionInfo (MasterGuildID) VALUES (?)");
        insertUnionStmt.bindUInt(1, m_MasterGuildID);
        insertUnionStmt.execute();

        m_UnionID = insertUnionStmt.getInsertID();

        list<GuildID_t>::iterator itr = m_Guilds.begin();

        for (; itr != m_Guilds.end(); ++itr) {
            PreparedStatement insertMemberStmt(pConn,
                                               "INSERT INTO GuildUnionMember (UnionID, OwnerGuildID) VALUES (?, ?)");
            insertMemberStmt.bindUInt(1, m_UnionID);
            insertMemberStmt.bindUInt(2, (*itr));
            insertMemberStmt.execute();
        }
    }
    END_DB(pStmt);

    __END_CATCH
}

void GuildUnion::destroy() {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement deleteInfoStmt(pConn, "DELETE FROM GuildUnionInfo WHERE UnionID = ?");
        deleteInfoStmt.bindUInt(1, m_UnionID);
        deleteInfoStmt.execute();

        PreparedStatement deleteMemberStmt(pConn, "DELETE FROM GuildUnionMember WHERE UnionID = ?");
        deleteMemberStmt.bindUInt(1, m_UnionID);
        deleteMemberStmt.execute();
    }
    END_DB(pStmt);

    __END_CATCH
}

GuildUnionManager::GuildUnionManager() {
    m_Mutex.setName("GuildUnionManager");
}

GuildUnionManager::~GuildUnionManager() {
    list<GuildUnion*>::iterator itr = m_GuildUnionList.begin();

    for (; itr != m_GuildUnionList.end(); ++itr) {
        SAFE_DELETE((*itr));
    }
}

void GuildUnionManager::addGuildUnion(GuildUnion* pUnion) {
    m_GuildUnionList.push_back(pUnion);

    m_UnionIDMap[pUnion->getUnionID()] = pUnion;
    m_GuildUnionMap[pUnion->getMasterGuildID()] = pUnion;

    list<GuildID_t>::iterator itr = pUnion->m_Guilds.begin();

    for (; itr != pUnion->m_Guilds.end(); ++itr) {
        m_GuildUnionMap[*itr] = pUnion;
    }
}

void GuildUnionManager::sendModifyUnionInfo(uint gID) {
    char Msg[80];
    sprintf(Msg, "*modifyunioninfo %d", gID);

    GGCommand ggCommand;
    ggCommand.setCommand(Msg);


    
    HashMapGameServerInfo** pGameServerInfos = g_pGameServerInfoManager->getGameServerInfos();


    static int myWorldID = g_pConfig->getPropertyInt("WorldID");
    static int myServerID = g_pConfig->getPropertyInt("ServerID");

    int maxWorldID = g_pGameServerInfoManager->getMaxWorldID();
    int maxServerGroupID = g_pGameServerInfoManager->getMaxServerGroupID();


    for (int worldID = 1; worldID < maxWorldID; worldID++) {
        for (int groupID = 0; groupID < maxServerGroupID; groupID++) {
            HashMapGameServerInfo& gameServerInfo = pGameServerInfos[worldID][groupID];

            if (!gameServerInfo.empty()) {
                HashMapGameServerInfo::const_iterator itr = gameServerInfo.begin();
                for (; itr != gameServerInfo.end(); itr++) {
                    GameServerInfo* pGameServerInfo = itr->second;

                    if (pGameServerInfo->getWorldID() == myWorldID) {
                        
                        if (pGameServerInfo->getGroupID() == myServerID) {
                        } else {
                            g_pLoginServerManager->sendPacket(pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(),
                                                              &ggCommand);
                        }
                    }
                }
            }
        }
    }
}

void GuildUnionManager::sendRefreshCommand() {
    GGCommand ggCommand;
    ggCommand.setCommand("*refreshguildunion");


    
    HashMapGameServerInfo** pGameServerInfos = g_pGameServerInfoManager->getGameServerInfos();


    static int myWorldID = g_pConfig->getPropertyInt("WorldID");
    static int myServerID = g_pConfig->getPropertyInt("ServerID");

    int maxWorldID = g_pGameServerInfoManager->getMaxWorldID();
    int maxServerGroupID = g_pGameServerInfoManager->getMaxServerGroupID();


    for (int worldID = 1; worldID < maxWorldID; worldID++) {
        for (int groupID = 0; groupID < maxServerGroupID; groupID++) {
            HashMapGameServerInfo& gameServerInfo = pGameServerInfos[worldID][groupID];

            if (!gameServerInfo.empty()) {
                HashMapGameServerInfo::const_iterator itr = gameServerInfo.begin();
                for (; itr != gameServerInfo.end(); itr++) {
                    GameServerInfo* pGameServerInfo = itr->second;

                    if (pGameServerInfo->getWorldID() == myWorldID) {
                        
                        if (pGameServerInfo->getGroupID() == myServerID) {
                        } else {
                            g_pLoginServerManager->sendPacket(pGameServerInfo->getIP(), pGameServerInfo->getUDPPort(),
                                                              &ggCommand);
                        }
                    }
                }
            }
        }
    }
}

bool GuildUnionManager::addGuild(uint uID, GuildID_t gID) {
    __BEGIN_TRY

    GuildUnion* pUnion = m_UnionIDMap[uID];
    if (pUnion == NULL)
        return false;

    if (pUnion->addGuild(gID)) {
        m_GuildUnionMap[gID] = pUnion;

        sendRefreshCommand();

        return true;
    } else {
        return false;
    }

    __END_CATCH
}

bool GuildUnionManager::removeMasterGuild(GuildID_t gID) {
    __BEGIN_TRY

    
    

    GuildUnion* pUnion = m_GuildUnionMap[gID];

    if (pUnion != NULL) {
        uint uID = pUnion->getUnionID();
        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement memberStmt(pConn, "SELECT OwnerGuildID FROM GuildUnionMember WHERE UnionID = ?");
            memberStmt.bindUInt(1, uID);
            Result* pResult = memberStmt.execute();


            if (pResult->getRowCount() == 0) {
                return false;
            }

            string unionMasterID = g_pGuildManager->getGuild(gID)->getMaster();

            while (pResult->next()) {
                if (pUnion->removeGuild(pResult->getInt(1))) {
                    m_GuildUnionMap[gID] = NULL;
                    if (pUnion->m_Guilds.empty()) {
                        list<GuildUnion*>::iterator itr =
                            find(m_GuildUnionList.begin(), m_GuildUnionList.end(), pUnion);
                        if (itr != m_GuildUnionList.end()) {
                            pUnion->destroy();
                            m_GuildUnionList.erase(itr);
                            m_GuildUnionMap.erase(pUnion->getMasterGuildID());
                            m_UnionIDMap.erase(pUnion->getUnionID());

                            SAFE_DELETE(pUnion);
                        } //
                    } // isEmpty
                    sendGCOtherModifyInfoGuildUnionByGuildID(pResult->getInt(1));
                } // if
            } // while
            

            Creature* pTargetCreature = NULL;
            __ENTER_CRITICAL_SECTION((*g_pPCFinder))

            pTargetCreature = g_pPCFinder->getCreature_LOCKED(unionMasterID);
            if (pTargetCreature != NULL) {
                GCModifyInformation gcModifyInformation2;
                makeGCModifyInfoGuildUnion(&gcModifyInformation2, pTargetCreature);
                pTargetCreature->getPlayer()->sendPacket(&gcModifyInformation2);
            }
            __LEAVE_CRITICAL_SECTION((*g_pPCFinder))

            
            sendGCOtherModifyInfoGuildUnionByGuildID(gID);

            sendRefreshCommand();
        }
        END_DB(pStmt);
    } else
    {
        Statement* pStmt = NULL;

        string unionMasterID = "";
        string guildMasterID = "";
        GuildID_t unionMasterGuildID = 0;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement memberStmt(pConn,
                                         "SELECT UnionID, OwnerGuildID FROM GuildUnionMember WHERE OwnerGuildID = ?");
            memberStmt.bindUInt(1, gID);
            Result* pResult = memberStmt.execute();


            if (pResult->getRowCount() == 0) {
                return false;
            }


            pResult->next();

            {
                // NOTE: the pre-migration code here allocated a second
                // Statement (pStmt2) but never actually executed a query
                // on it -- the second SELECT ran through the outer
                // statement's connection instead (pre-existing bug).
                // Preserved as-is: this second query still executes on
                // the same `pConn` rather than a distinct one, exactly
                // matching what actually ran before.
                PreparedStatement unionInfoStmt(pConn, "SELECT MasterGuildID FROM GuildUnionInfo WHERE UnionID = ?");
                unionInfoStmt.bindUInt(1, pResult->getInt(1));
                Result* pResult2 = unionInfoStmt.execute();


                if (pResult2->getRowCount() != 0) {
                    pResult->next();

                    unionMasterGuildID = pResult2->getInt(1);
                    unionMasterID = g_pGuildManager->getGuild(unionMasterGuildID)->getMaster();
                }
            }

            guildMasterID = g_pGuildManager->getGuild(gID)->getMaster();

            if (removeGuild(pResult->getInt(1), pResult->getInt(2))) {
                Creature* pTargetCreature = NULL;  
                Creature* pTargetCreature2 = NULL; 

                __ENTER_CRITICAL_SECTION((*g_pPCFinder))

                pTargetCreature = g_pPCFinder->getCreature_LOCKED(guildMasterID);
                if (pTargetCreature != NULL) {
                    GCModifyInformation gcModifyInformation2;
                    makeGCModifyInfoGuildUnion(&gcModifyInformation2, pTargetCreature);
                    pTargetCreature->getPlayer()->sendPacket(&gcModifyInformation2);
                }

                pTargetCreature2 = g_pPCFinder->getCreature_LOCKED(unionMasterID);
                if (pTargetCreature != NULL) {
                    GCModifyInformation gcModifyInformation2;
                    makeGCModifyInfoGuildUnion(&gcModifyInformation2, pTargetCreature2);
                    pTargetCreature2->getPlayer()->sendPacket(&gcModifyInformation2);
                }
                __LEAVE_CRITICAL_SECTION((*g_pPCFinder))


                
                sendGCOtherModifyInfoGuildUnionByGuildID(gID);
                
                sendGCOtherModifyInfoGuildUnionByGuildID(unionMasterGuildID);

                
                sendRefreshCommand();
            }
        }
        END_DB(pStmt);
    }

    __END_CATCH

    return true;
}

bool GuildUnionManager::removeGuild(uint uID, GuildID_t gID) {
    __BEGIN_TRY

    GuildUnion* pUnion = m_UnionIDMap[uID];
    if (pUnion == NULL)
        return false;

    if (pUnion->removeGuild(gID)) {
        m_GuildUnionMap[gID] = NULL;
        if (pUnion->m_Guilds.empty()) {
            list<GuildUnion*>::iterator itr = find(m_GuildUnionList.begin(), m_GuildUnionList.end(), pUnion);
            if (itr != m_GuildUnionList.end()) {
                pUnion->destroy();

                // m_GuildUnionMap[pUnion->getMasterGuildID()] = NULL;
                // m_UnionIDMap[pUnion->getUnionID()] = NULL;

                m_GuildUnionList.erase(itr);
                m_GuildUnionMap.erase(pUnion->getMasterGuildID());
                m_UnionIDMap.erase(pUnion->getUnionID());

                SAFE_DELETE(pUnion);
            }
        }

        sendRefreshCommand();
        return true;
    } else {
        return false;
    }

    __END_CATCH
}

void GuildUnionManager::reload() {
    __ENTER_CRITICAL_SECTION(m_Mutex)

    list<GuildUnion*>::iterator itr = m_GuildUnionList.begin();
    list<GuildUnion*>::iterator endItr = m_GuildUnionList.end();

    for (; itr != endItr; ++itr) {
        GuildUnion* pUnion = *itr;
        SAFE_DELETE(pUnion);
    }
    m_GuildUnionList.clear();

    m_GuildUnionMap.clear();
    m_UnionIDMap.clear();

    load();

    __LEAVE_CRITICAL_SECTION(m_Mutex)
}

void GuildUnionManager::load() {
    __BEGIN_TRY

    Statement* pStmt = NULL;
    Statement* pStmt2 = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement unionStmt(pConn, "SELECT UnionID, MasterGuildID FROM GuildUnionInfo");
        Result* pResult = unionStmt.execute();

        while (pResult->next()) {
            uint uID = pResult->getInt(1);
            GuildID_t gID = pResult->getInt(2);

            GuildUnion* pUnion = new GuildUnion(gID);
            pUnion->setUnionID(uID);

            BEGIN_DB {
                PreparedStatement memberStmt(pConn, "SELECT OwnerGuildID FROM GuildUnionMember WHERE UnionID = ?");
                memberStmt.bindUInt(1, uID);
                Result* pResult2 = memberStmt.execute();

                while (pResult2->next()) {
                    GuildID_t gID2 = pResult2->getInt(1);
                    pUnion->m_Guilds.push_back(gID2);
                    //					pUnion->addGuild( gID2 );
                }
            }
            END_DB(pStmt2)

            addGuildUnion(pUnion);
        }
    }
    END_DB(pStmt)

    __END_CATCH
}

uint GuildUnionOfferManager::offerJoin(GuildID_t gID, GuildID_t masterGID) {
    __BEGIN_TRY

    if (GuildUnionManager::Instance().getGuildUnion(gID) != NULL)
        return ALREADY_IN_UNION;
    GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(masterGID);

    Guild* pReqGuild = g_pGuildManager->getGuild(gID);
    Guild* pMasterGuild = g_pGuildManager->getGuild(masterGID);

    if (pReqGuild != NULL && pMasterGuild != NULL) {
        if (pReqGuild->getActiveMemberCount() > MAX_GUILDMEMBER_ACTIVE_COUNT ||
            pMasterGuild->getActiveMemberCount() > MAX_GUILDMEMBER_ACTIVE_COUNT) {
            return TOO_MANY_MEMBER;
        }
    }

    //
    if (pUnion == NULL) {
        pUnion = new GuildUnion(masterGID);
        pUnion->create();
        GuildUnionManager::Instance().addGuildUnion(pUnion);
    } else if (pUnion->getMasterGuildID() != masterGID) {
        return TARGET_IS_NOT_MASTER;
    }

    if (hasOffer(gID)) {
        return ALREADY_OFFER_SOMETHING;
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");

        PreparedStatement penaltyStmt(pConn,
                                      "SELECT COUNT(*) FROM GuildUnionOffer WHERE OfferType='ESCAPE' and "
                                      "OwnerGuildID=? and OfferTime >= now() - interval 10 day");
        penaltyStmt.bindUInt(1, gID);
        Result* pResult = penaltyStmt.execute();

        pResult->next();


        if (pResult->getInt(1) > 0) {
            return YOU_HAVE_PENALTY;
        }


        PreparedStatement slotStmt(pConn, "SELECT COUNT(*) FROM GuildUnionMember WHERE UnionID=?");
        slotStmt.bindUInt(1, pUnion->getUnionID());
        pResult = slotStmt.execute();
        pResult->next();

        if (pResult->getInt(1) >= g_pVariableManager->getVariable(GUILD_UNION_MAX)) {
            return NOT_ENOUGH_SLOT;
        }


        PreparedStatement expireStmt(
            pConn, "DELETE FROM GuildUnionOffer WHERE OwnerGuildID=? and OfferTime < now() - interval 10 day");
        expireStmt.bindUInt(1, gID);
        expireStmt.execute();

        PreparedStatement offerStmt(
            pConn, "INSERT INTO GuildUnionOffer (UnionID, OfferType, OwnerGuildID, OfferTime) VALUES (?, 'JOIN', ?, now())");
        offerStmt.bindUInt(1, pUnion->getUnionID());
        offerStmt.bindUInt(2, gID);
        offerStmt.execute();
    }
    END_DB(pStmt)

    return OK;

    __END_CATCH
}

uint GuildUnionOfferManager::offerQuit(GuildID_t gID) {
    __BEGIN_TRY

    GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(gID);

    if (pUnion == NULL) {
        return NOT_IN_UNION;
    } else if (pUnion->getMasterGuildID() == gID) {
        return MASTER_CANNOT_QUIT;
    }

    if (hasOffer(gID)) {
        return ALREADY_OFFER_SOMETHING;
    }

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement offerStmt(
            pConn, "INSERT INTO GuildUnionOffer (UnionID, OfferType, OwnerGuildID, OfferTime) VALUES (?, 'QUIT', ?, now())");
        offerStmt.bindUInt(1, pUnion->getUnionID());
        offerStmt.bindUInt(2, gID);
        offerStmt.execute();
    }
    END_DB(pStmt)

    return OK;

    __END_CATCH
}

bool GuildUnionOfferManager::makeOfferList(uint uID, GCUnionOfferList& offerList) {
    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement offerStmt(pConn,
                                    "SELECT OfferType+0, OwnerGuildID, DATE_FORMAT(Offertime,'%y%m%d') "
                                    "FROM GuildUnionOffer WHERE UnionID=?");
        offerStmt.bindUInt(1, uID);
        Result* pResult = offerStmt.execute();



        if (pResult->getRowCount() == 0) {
            return false;
        }

        while (pResult->next()) {
            SingleGuildUnionOffer* offer = new SingleGuildUnionOffer;

            offer->setGuildType(pResult->getInt(1));
            offer->setGuildID(pResult->getInt(2));

            DWORD dwDate = pResult->getInt(3);
            offer->setDate(dwDate * 100);

            // cout << "Date : " << (DWORD)offer->getDate() << endl;

            // cout << "TYPE : " << (int)pResult->getInt(1) << endl;

            PreparedStatement guildInfoStmt(pConn, "SELECT GuildName, Master FROM GuildInfo WHERE GuildID=?");
            guildInfoStmt.bindUInt(1, pResult->getInt(2));
            Result* pResult2 = guildInfoStmt.execute();

            if (pResult2->getRowCount() == 0) {
                delete offer;
                return false;
            }

            pResult2->next();

            offer->setGuildName(pResult2->getString(1));
            offer->setGuildMaster(pResult2->getString(2));

            offerList.addUnionOfferList(offer);

            /*cout << "GuildID : " << offer->getGuildID()
                << ", GuildType : " <<  (int)offer->getGuildType()
                << ", GuildName : " <<  offer->getGuildName()
                << ", GuildMaster : " <<  offer->getGuildMaster()
                << endl;
             */
        }
    }
    END_DB(pStmt)

    // cout << "make offerlist success!" << endl;
    return true;
}

uint GuildUnionOfferManager::acceptJoin(GuildID_t gID) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement offerStmt(pConn,
                                    "SELECT UnionID FROM GuildUnionOffer WHERE OfferType='JOIN' AND OwnerGuildID=?");
        offerStmt.bindUInt(1, gID);
        Result* pResult = offerStmt.execute();

        if (pResult->getRowCount() == 0)
            return NO_TARGET_UNION;

        if (pResult->next()) {
            clearOffer(gID);

            GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(gID);
            if (pUnion != NULL) {
                return ALREADY_IN_UNION;
            }

            uint uID = pResult->getInt(1);
            pUnion = GuildUnionManager::Instance().getGuildUnionByUnionID(uID);
            if (pUnion == NULL) {
                return NO_TARGET_UNION;
            }

            PreparedStatement slotStmt(pConn, "SELECT COUNT(*) FROM GuildUnionMember WHERE UnionID=?");
            slotStmt.bindUInt(1, uID);
            Result* pResult2 = slotStmt.execute();
            pResult2->next();

            if (pResult2->getInt(1) >= g_pVariableManager->getVariable(GUILD_UNION_MAX)) {
                return NOT_ENOUGH_SLOT;
            }


            GuildUnionManager::Instance().addGuild(uID, gID);
        }
    }
    END_DB(pStmt);

    return OK;

    __END_CATCH
}

uint GuildUnionOfferManager::acceptQuit(GuildID_t gID) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement offerStmt(pConn,
                                    "SELECT UnionID FROM GuildUnionOffer WHERE OfferType='QUIT' AND OwnerGuildID=?");
        offerStmt.bindUInt(1, gID);
        Result* pResult = offerStmt.execute();

        if (pResult->next()) {
            clearOffer(gID);

            GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(gID);
            if (pUnion == NULL) {
                return NOT_IN_UNION;
            }

            uint uID = pResult->getInt(1);
            if (uID != pUnion->getUnionID()) {
                return NOT_YOUR_UNION;
            }

            pUnion = GuildUnionManager::Instance().getGuildUnionByUnionID(uID);
            if (pUnion == NULL) {
                return NO_TARGET_UNION;
            }

            GuildUnionManager::Instance().removeGuild(uID, gID);
        }
    }
    END_DB(pStmt);

    return OK;

    __END_CATCH
}

uint GuildUnionOfferManager::denyJoin(GuildID_t gID) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement offerStmt(pConn,
                                    "SELECT UnionID FROM GuildUnionOffer WHERE OfferType='JOIN' AND OwnerGuildID=?");
        offerStmt.bindUInt(1, gID);
        Result* pResult = offerStmt.execute();

        if (pResult->next()) {
            clearOffer(gID);

            GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(gID);
            if (pUnion != NULL) {
                return ALREADY_IN_UNION;
            }

            uint uID = pResult->getInt(1);
            pUnion = GuildUnionManager::Instance().getGuildUnionByUnionID(uID);
            if (pUnion == NULL) {
                return NO_TARGET_UNION;
            }
        }
    }
    END_DB(pStmt);

    return OK;

    __END_CATCH
}

uint GuildUnionOfferManager::denyQuit(GuildID_t gID) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement offerStmt(pConn,
                                    "SELECT UnionID FROM GuildUnionOffer WHERE OfferType='QUIT' AND OwnerGuildID=?");
        offerStmt.bindUInt(1, gID);
        Result* pResult = offerStmt.execute();

        if (pResult->next()) {
            clearOffer(gID);

            GuildUnion* pUnion = GuildUnionManager::Instance().getGuildUnion(gID);
            if (pUnion == NULL) {
                return NOT_IN_UNION;
            }

            uint uID = pResult->getInt(1);
            if (uID != pUnion->getUnionID()) {
                return NOT_YOUR_UNION;
            }

            pUnion = GuildUnionManager::Instance().getGuildUnionByUnionID(uID);
            if (pUnion == NULL) {
                return NO_TARGET_UNION;
            }
        }
    }
    END_DB(pStmt);

    return OK;

    __END_CATCH
}

void GuildUnionOfferManager::clearOffer(GuildID_t gID) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement clearStmt(pConn, "DELETE FROM GuildUnionOffer WHERE OwnerGuildID=?");
        clearStmt.bindUInt(1, gID);
        clearStmt.execute();
    }
    END_DB(pStmt)

    __END_CATCH
}

bool GuildUnionOfferManager::hasOffer(GuildID_t gID) {
    __BEGIN_TRY

    Statement* pStmt = NULL;

    BEGIN_DB {
        Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
        PreparedStatement hasOfferStmt(pConn, "SELECT count(*) FROM GuildUnionOffer WHERE OwnerGuildID=?");
        hasOfferStmt.bindUInt(1, gID);
        Result* pResult = hasOfferStmt.execute();
        if (pResult->next()) {
            if (pResult->getInt(1) > 0) {
                return true;
            }
        }
    }
    END_DB(pStmt)

    return false;

    __END_CATCH
}
