//////////////////////////////////////////////////////////////////////////////
// Filename    : PacketUtil.h
// Written by  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#ifndef __PACKETUTIL_H__
#define __PACKETUTIL_H__

#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// forward declaration
//////////////////////////////////////////////////////////////////////////////

class Creature;
class Slayer;
class Vampire;
class Ousters;
class Monster;
class NPC;
class Item;
class SlayerCorpse;
class VampireCorpse;
class OustersCorpse;
class MonsterCorpse;
class SLAYER_RECORD;
class VAMPIRE_RECORD;
class OUSTERS_RECORD;
class GamePlayer;
class PlayerCreature;

class GCUpdateInfo;
class GCAddSlayer;
class GCAddVampire;
class GCAddOusters;
class GCAddMonster;
class GCAddNPC;
class GCAddNewItemToZone;
class GCDropItemToZone;
class GCAddSlayerCorpse;
class GCAddVampireCorpse;
class GCAddMonsterCorpse;
class GCAddOustersCorpse;
class GCOtherModifyInfo;
class GCCreateItem;
class GCWarScheduleList;
class GCMiniGameScores;
class GCPetStashList;
class GCModifyInformation;

// class GCItemNameInfoList;

//////////////////////////////////////////////////////////////////////////////
// function headers
//////////////////////////////////////////////////////////////////////////////


void makeGCModifyInfoGuildUnion(GCModifyInformation* pModifyInformation, Creature* pCreature);
void makeGCOtherModifyInfoGuildUnion(GCOtherModifyInfo* pModifyInformation, Creature* pCreature);
void sendGCOtherModifyInfoGuildUnion(Creature* pTargetCreature);
void sendGCOtherModifyInfoGuildUnionByGuildID(uint gID);



void makeGCUpdateInfo(GCUpdateInfo* pUpdateInfo, Creature* pCreature);


void makeGCAddSlayer(GCAddSlayer* pAddSlayer, Slayer* pSlayer);


void makeGCAddVampire(GCAddVampire* pAddVampire, Vampire* pVampire);


void makeGCAddOusters(GCAddOusters* pAddOusters, Ousters* pOusters);


void makeGCAddMonster(GCAddMonster* pAddMonster, Monster* pMonster);


void makeGCAddNPC(GCAddNPC* pAddNPC, NPC* pNPC);


void makeGCAddNewItemToZone(GCAddNewItemToZone* pAddItem, Item* pItem, int X, int Y);


void makeGCDropItemToZone(GCDropItemToZone* pAddItem, Item* pItem, int X, int Y);


void makeGCAddSlayerCorpse(GCAddSlayerCorpse* pAddSlayerCorpse, SlayerCorpse* pSlayerCorpse);


void makeGCAddVampireCorpse(GCAddVampireCorpse* pAddVampireCorpse, VampireCorpse* pVampireCorpse);


void makeGCAddMonsterCorpse(GCAddMonsterCorpse* pAddMonsterCorpse, MonsterCorpse* pMonsterCorpse, int X, int Y);


void makeGCAddOustersCorpse(GCAddOustersCorpse* pAddOustersCorpse, OustersCorpse* pOustersCorpse);


void makeGCOtherModifyInfo(GCOtherModifyInfo* pInfo, Slayer* pSlayer, const SLAYER_RECORD* prev);
void makeGCOtherModifyInfo(GCOtherModifyInfo* pInfo, Vampire* pVampire, const VAMPIRE_RECORD* prev);
void makeGCOtherModifyInfo(GCOtherModifyInfo* pInfo, Ousters* pOusters, const OUSTERS_RECORD* prev);


void sendPayInfo(GamePlayer* pGamePlayer);


void makeGCCreateItem(GCCreateItem* pGCCreateItem, Item* pItem, CoordInven_t x, CoordInven_t y);


void sendEffectLevelUp(Creature* pCreature);


void sendSystemMessage(GamePlayer* pGamePlayer, const string& msg);

bool makeGCWarScheduleList(GCWarScheduleList* pGCWarScheduleList, ZoneID_t zoneID);


// void makeGCItemNameInfoList(GCItemNameInfoList* pInfo, PlayerCreature* pPC) ;

void sendGCMiniGameScores(PlayerCreature* pPC, BYTE gameType, BYTE Level);

void makeGCPetStashList(GCPetStashList* pPacket, PlayerCreature* pPC);

#endif
