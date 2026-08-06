//////////////////////////////////////////////////////////////////////////////
// Filename    : MonsterSummonInfo.h

// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __MONSTER_SUMMON_INFO_H__
#define __MONSTER_SUMMON_INFO_H__

#include <list>
#include <vector>

#include "MonsterInfoTypes.h"
#include "Types.h"

struct MonsterCollection;


struct SUMMON_INFO {
    enum ClanType {
        CLAN_TYPE_DEFAULT,      // default = 1
        CLAN_TYPE_RANDOM_EACH,  
        CLAN_TYPE_RANDOM_GROUP, 
        CLAN_TYPE_GROUP,        
    };

    SUMMON_INFO() {
        scanEnemy = false;
        canScanEnemy = false;
        clanType = CLAN_TYPE_DEFAULT;
        clanID = 0;
        hasItem = true;
        regenType = REGENTYPE_MAX;
        initHPPercent = 0;
    }

    bool canScanEnemy; 
    ClanType clanType;
    int clanID;
    bool hasItem; 
    RegenType regenType;
    bool scanEnemy; 
    int initHPPercent;
};


struct SUMMON_INFO2 : public SUMMON_INFO {
    ZoneCoord_t X;
    ZoneCoord_t Y;
    MonsterCollection* pMonsters;
};


struct MonsterCollectionInfo {
    SpriteType_t SpriteType;
    MonsterType_t MonsterType;
    int Num;

    void parseString(const string& infoString);

    string toString() const;
};


struct MonsterCollection {
    list<MonsterCollectionInfo> Infos;

    void parseString(const string& collectionString);

    string toString() const;
};


struct MonsterSummonStep {
    vector<MonsterCollection> Collections;

    const MonsterCollection* getRandomMonsterCollection() const;

    void parseString(const string& summonStepsString);

    string toString() const;
};


struct MonsterSummonInfo {
    vector<MonsterSummonStep> Steps;

    const MonsterCollection* getRandomMonsterCollection(int step) const;
    bool hasNextMonsterCollection(int step) const;

    void parseString(const string& summonInfoString);

    string toString() const;
};

#endif
