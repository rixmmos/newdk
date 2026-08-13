//////////////////////////////////////////////////////////////////////////////
// Filename    : CGModifyNicknameHandler.cc
// Written By  : elca@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "CGModifyNickname.h"

#ifdef __GAME_SERVER__
#include "CreatureUtil.h"
#include "DB.h"
#include "GCModifyNickname.h"
#include "GCNicknameVerify.h"
#include "GCUseOK.h"
#include "GamePlayer.h"
#include "Inventory.h"
#include "NicknameBook.h"
#include "NicknameInfo.h"
#include "PetInfo.h"
#include "PlayerCreature.h"
#include "PreparedStatement.h"
#include "Zone.h"
#include "item/EventGiftBox.h"
#include "item/PetItem.h"
#endif

// Backslash-escapes quotes for embedding into a raw SQL literal. The whole tree
// now binds these values as parameters instead, so this has no callers left --
// only two stale extern declarations in PetItem.cpp and NicknameBook.cpp. It is
// kept, and corrected, rather than deleted: the previous body checked its bound
// *after* both writes, so a 100-character input wrote ret[100] and then the
// terminator at ret[101], two bytes past a 100-byte stack array, with the
// overflowing byte taken straight from the input.
string getDBString(const string& str) {
    string ret;
    ret.reserve(str.size());

    for (string::const_iterator itr = str.begin(); itr != str.end(); ++itr) {
        char c = *itr;
        if (c == '\\' || c == '\'')
            ret += '\\';
        ret += c;
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void CGModifyNicknameHandler::execute(CGModifyNickname* pPacket, Player* pPlayer)

{
    __BEGIN_TRY __BEGIN_DEBUG_EX

#ifdef __GAME_SERVER__

        Assert(pPacket != NULL);
    Assert(pPlayer != NULL);

    GamePlayer* pGamePlayer = dynamic_cast<GamePlayer*>(pPlayer);
    Assert(pGamePlayer != NULL);

    PlayerCreature* pPC = dynamic_cast<PlayerCreature*>(pGamePlayer->getCreature());
    Assert(pPC != NULL);

    Inventory* pInventory = pPC->getInventory();
    Assert(pInventory != NULL);

    GCNicknameVerify gcNV;

    ObjectID_t itemOID = pPacket->getItemObjectID();
    EventGiftBox* pItem = NULL;

    if (itemOID != 0) {
        pItem = dynamic_cast<EventGiftBox*>(pInventory->getItemWithObjectID(itemOID));

        if (pItem == NULL) {
            filelog("Nickname.log", "[%s:%s]   EventGiftBox .", pPlayer->getID().c_str(), pPC->getName().c_str());

            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }
    } else {
        PetInfo* pPetInfo = pPC->getPetInfo();
        if (pPetInfo == NULL || pPetInfo->getPetType() != PET_PACKAGE) {
            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }

        pPetInfo->setNickname(pPacket->getNickname());
        pPetInfo->getPetItem()->savePetInfo();
        sendPetInfo(pGamePlayer, true);

        gcNV.setCode(NICKNAME_MODIFY_OK);
        pGamePlayer->sendPacket(&gcNV);
        return;
    }

    switch (pItem->getItemType()) {
    case 23: {
        PetInfo* pPetInfo = pPC->getPetInfo();
        if (pPetInfo == NULL) {
            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }

        if (pPetInfo->getPetType() != PET_PACKAGE && pItem == NULL) {
            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }

        pPetInfo->setNickname(pPacket->getNickname());
        pPetInfo->getPetItem()->savePetInfo();
        sendPetInfo(pGamePlayer, true);

        gcNV.setCode(NICKNAME_MODIFY_OK);
        pGamePlayer->sendPacket(&gcNV);

        if (pPetInfo->getPetType() != PET_PACKAGE) {
            pInventory->deleteItem(itemOID);
            pItem->destroy();
            SAFE_DELETE(pItem);

            GCUseOK gcOK;
            pGamePlayer->sendPacket(&gcOK);
        }

        return;
    }


    case 22:
    case 25: {
        NicknameInfo* pNickname = pPC->getNicknameBook()->getNicknameInfo(0);

        if (pNickname == NULL || pNickname->getNicknameType() != NicknameInfo::NICK_CUSTOM ||
            pPacket->getNickname() == "") {
            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }

        if (pItem == NULL) {
            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }

        //			pPC->removeFlag(Effect::EFFECT_CLASS_CAN_MODIFY_NICKNAME_0);
        pNickname->setNickname(pPacket->getNickname());

        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement updateNicknameStmt(pConn, "UPDATE NicknameBook SET Nickname=? WHERE OwnerID=? AND nID=?");
            updateNicknameStmt.bindString(1, pNickname->getNickname());
            updateNicknameStmt.bindString(2, pPC->getName());
            updateNicknameStmt.bindUInt(3, pNickname->getNicknameID());
            updateNicknameStmt.execute();
        }
        END_DB(pStmt)

        gcNV.setCode(NICKNAME_MODIFY_OK);
        pGamePlayer->sendPacket(&gcNV);

        if (pItem->getItemType() == 22) {
            pInventory->deleteItem(itemOID);
            pItem->destroy();
            SAFE_DELETE(pItem);

            GCUseOK gcOK;
            pGamePlayer->sendPacket(&gcOK);
        }

        if (pPC->getNickname() == pNickname) {
            GCModifyNickname gcMN;
            gcMN.setObjectID(pPC->getObjectID());
            gcMN.setNicknameInfo(pNickname);
            pPC->getZone()->broadcastPacket(pPC->getX(), pPC->getY(), &gcMN);
        }

        break;
    }


    case 24: {
        if (pItem == NULL) {
            gcNV.setCode(NICKNAME_MODIFY_FAIL_NO_ITEM);
            pGamePlayer->sendPacket(&gcNV);
            return;
        }

        NicknameInfo* pNickname = new NicknameInfo;
        pNickname->setNicknameID(pPC->getNicknameBook()->popNicknameID());
        pNickname->setNicknameType(NicknameInfo::NICK_CUSTOM);
        pNickname->setNickname(pPacket->getNickname());

        pPC->getNicknameBook()->setNicknameInfo(pNickname->getNicknameID(), pNickname);

        Statement* pStmt = NULL;

        BEGIN_DB {
            Connection* pConn = g_pDatabaseManager->getConnection("DARKEDEN");
            PreparedStatement insertNicknameStmt(pConn,
                                                 "INSERT INTO NicknameBook (nID, OwnerID, NickType, Nickname, Time) "
                                                 "VALUES (?, ?, ?, ?, now())");
            insertNicknameStmt.bindUInt(1, pNickname->getNicknameID());
            insertNicknameStmt.bindString(2, pPC->getName());
            insertNicknameStmt.bindUInt(3, pNickname->getNicknameType());
            insertNicknameStmt.bindString(4, pNickname->getNickname());
            insertNicknameStmt.execute();
        }
        END_DB(pStmt)

        gcNV.setCode(NICKNAME_MODIFY_OK);
        pGamePlayer->sendPacket(&gcNV);

        Packet* pNickList = pPC->getNicknameBook()->getNicknameBookListPacket();
        pGamePlayer->sendPacket(pNickList);
        SAFE_DELETE(pNickList);

        pInventory->deleteItem(itemOID);
        pItem->destroy();
        SAFE_DELETE(pItem);

        GCUseOK gcOK;
        pGamePlayer->sendPacket(&gcOK);

        break;
    }
    }

#endif // __GAME_SERVER__

    __END_DEBUG_EX __END_CATCH
}
