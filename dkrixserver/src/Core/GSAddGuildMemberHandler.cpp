//----------------------------------------------------------------------
//
// Filename    : GSAddGuildMemberHandler.cpp
// Written By  :
// Description :
//
//----------------------------------------------------------------------

// include files
#include "Assert.h"
#include "GSAddGuildMember.h"

#ifdef __SHARED_SERVER__

#include "DB.h"
#include "GameServerManager.h"
#include "Guild.h"
#include "GuildManager.h"
#include "PreparedStatement.h"
#include "SGAddGuildMemberOK.h"
#include "SGModifyGuildOK.h"
#include "StringPool.h"

#endif

//----------------------------------------------------------------------
//
// GSAddGuildHandler::execute()
//
//----------------------------------------------------------------------
void GSAddGuildMemberHandler::execute(GSAddGuildMember* pPacket, Player* pPlayer) {
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __SHARED_SERVER__

        Assert(pPacket != NULL);

    GuildMember* pGuildMember = new GuildMember();
    pGuildMember->setGuildID(pPacket->getGuildID());
    pGuildMember->setName(pPacket->getName());
    pGuildMember->setRank(pPacket->getGuildMemberRank());

    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_WAIT) {
        pGuildMember->setRequestDateTime(VSDateTime::currentDateTime());
    }

    // DB 에 Guild Member 를 저장한다.
    pGuildMember->create();

    // Guild Member Intro 를 DB에 저장한다.
    pGuildMember->saveIntro(pPacket->getGuildMemberIntro());

    // 길드에 추가한다.
    Guild* pGuild = g_pGuildManager->getGuild(pPacket->getGuildID());
    pGuild->addMember(pGuildMember);

    // 게임 서버로 보낼 패킷을 만든다.
    SGAddGuildMemberOK sgAddGuildMemberOK;
    sgAddGuildMemberOK.setGuildID(pGuildMember->getGuildID());
    sgAddGuildMemberOK.setName(pGuildMember->getName());
    sgAddGuildMemberOK.setGuildMemberRank(pGuildMember->getRank());
    sgAddGuildMemberOK.setServerGroupID(pPacket->getServerGroupID());

    // 게임 서버로 패킷을 보낸다.
    g_pGameServerManager->broadcast(&sgAddGuildMemberOK);

    // 등록 대기 길드이고 길드원이 5명 이상이 되면 정식 길드가 된다.
    if (pGuild->getState() == Guild::GUILD_STATE_WAIT && pGuild->getActiveMemberCount() > 4) {
        HashMapGuildMember& Members = pGuild->getMembers();
        HashMapGuildMemberItor itr = Members.begin();
        for (; itr != Members.end(); itr++) {
            pGuildMember = itr->second;

            ///////////////////////////////////////////////////////////////////////////
            // DB에 Slayer, Vampire, Ousters 테이블의 GuildID를 바꾼다.
            // DB에 Message를 추가한다.
            ///////////////////////////////////////////////////////////////////////////
            Statement* pStmt = NULL;
            BEGIN_DB {
                Connection* pConnection = g_pDatabaseManager->getConnection("DARKEDEN");
                pStmt = pConnection->createStatement();

                PreparedStatement updateGuildMemberStmt(
                    pConnection, "UPDATE GuildMember SET RequestDateTime = now() WHERE Name = ?");
                updateGuildMemberStmt.bindString(1, pGuildMember->getName());
                updateGuildMemberStmt.execute();

                if (pGuild->getRace() == Guild::GUILD_RACE_SLAYER) {
                    PreparedStatement updateRaceStmt(pConnection, "UPDATE Slayer SET GuildID = ? WHERE Name = ?");
                    updateRaceStmt.bindInt(1, pGuild->getID());
                    updateRaceStmt.bindString(2, pGuildMember->getName());
                    updateRaceStmt.execute();

                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
                    {
                        PreparedStatement insertMessageStmt(
                            pConnection, "INSERT INTO Messages (Receiver, Message) VALUES (?, ?)");
                        insertMessageStmt.bindString(1, pGuildMember->getName());
                        insertMessageStmt.bindString(2, g_pStringPool->c_str(STRID_TEAM_REGISTRATION_ACCEPT));
                        insertMessageStmt.execute();
                    }
                    else {
                        PreparedStatement insertMessageStmt(
                            pConnection, "INSERT INTO Messages (Receiver, Message) VALUES (?, ?)");
                        insertMessageStmt.bindString(1, pGuildMember->getName());
                        insertMessageStmt.bindString(2, g_pStringPool->c_str(STRID_TEAM_REGISTRATION_ACCEPT_2));
                        insertMessageStmt.execute();
                    }
                } else if (pGuild->getRace() == Guild::GUILD_RACE_VAMPIRE) {
                    PreparedStatement updateRaceStmt(pConnection, "UPDATE Vampire SET GuildID = ? WHERE Name = ?");
                    updateRaceStmt.bindInt(1, pGuild->getID());
                    updateRaceStmt.bindString(2, pGuildMember->getName());
                    updateRaceStmt.execute();

                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
                    {
                        PreparedStatement insertMessageStmt(
                            pConnection, "INSERT INTO Messages (Receiver, Message) VALUES (?, ?)");
                        insertMessageStmt.bindString(1, pGuildMember->getName());
                        insertMessageStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT));
                        insertMessageStmt.execute();
                    }
                    else {
                        PreparedStatement insertMessageStmt(
                            pConnection, "INSERT INTO Messages (Receiver, Message) VALUES (?, ?)");
                        insertMessageStmt.bindString(1, pGuildMember->getName());
                        insertMessageStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT_2));
                        insertMessageStmt.execute();
                    }
                } else if (pGuild->getRace() == Guild::GUILD_RACE_OUSTERS) {
                    PreparedStatement updateRaceStmt(pConnection, "UPDATE Ousters SET GuildID = ? WHERE Name = ?");
                    updateRaceStmt.bindInt(1, pGuild->getID());
                    updateRaceStmt.bindString(2, pGuildMember->getName());
                    updateRaceStmt.execute();

                    if (pGuildMember->getRank() == GuildMember::GUILDMEMBER_RANK_MASTER)
                    {
                        PreparedStatement insertMessageStmt(
                            pConnection, "INSERT INTO Messages (Receiver, Message) VALUES (?, ?)");
                        insertMessageStmt.bindString(1, pGuildMember->getName());
                        insertMessageStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT));
                        insertMessageStmt.execute();
                    }
                    else {
                        PreparedStatement insertMessageStmt(
                            pConnection, "INSERT INTO Messages (Receiver, Message) VALUES (?, ?)");
                        insertMessageStmt.bindString(1, pGuildMember->getName());
                        insertMessageStmt.bindString(2, g_pStringPool->c_str(STRID_CLAN_REGISTRATION_ACCEPT_2));
                        insertMessageStmt.execute();
                    }
                }


                SAFE_DELETE(pStmt);
            }
            END_DB(pStmt)
        }

        pGuild->setState(Guild::GUILD_STATE_ACTIVE);
        pGuild->save();

        SGModifyGuildOK sgModifyGuildOK;
        sgModifyGuildOK.setGuildID(pGuild->getID());
        sgModifyGuildOK.setGuildState(pGuild->getState());

        g_pGameServerManager->broadcast(&sgModifyGuildOK);
    }

#endif

    __END_DEBUG_EX __END_CATCH
}
