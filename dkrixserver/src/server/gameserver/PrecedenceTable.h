//////////////////////////////////////////////////////////////////////////////
// Filename    : PrecedenceTable.h
// Written by  : excel96
// Description :




//////////////////////////////////////////////////////////////////////////////

#ifndef __PRECEDENCETABLE_H__
#define __PRECEDENCETABLE_H__

#include <unordered_map>

#include "Timeval.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// Forward declaration
//////////////////////////////////////////////////////////////////////////////
class Creature;

//////////////////////////////////////////////////////////////////////////////
// class PrecedenceElement;



//////////////////////////////////////////////////////////////////////////////

class PrecedenceElement {
public:
    PrecedenceElement();

public:
    string getName(void) const {
        return m_Name;
    }
    void setName(const string& name) {
        m_Name = name;
    }

    int getPartyID(void) const {
        return m_PartyID;
    }
    void setPartyID(int PartyID) {
        m_PartyID = PartyID;
    }

    int getDamage(void) const {
        return m_Damage;
    }
    void setDamage(int Damage) {
        m_Damage = Damage;
    }

    Timeval getDeadline(void) const {
        return m_Deadline;
    }
    void setDeadline(const Timeval& deadline) {
        m_Deadline = deadline;
    }
    void setNextTime(void);

    string toString(void) const;

public:
    string m_Name;
    int m_PartyID;
    int m_Damage;
    Timeval m_Deadline;
};

//////////////////////////////////////////////////////////////////////////////
// class PrecedenceTable;




//////////////////////////////////////////////////////////////////////////////

class PrecedenceTable {
public:
    PrecedenceTable();
    ~PrecedenceTable();

public:
    //	void addPrecedence(Creature* pCreature, int damage);
    void addPrecedence(const string& Name, int PartyID, int damage);
    void heartbeat(const Timeval& currentTime);
    void compute(void);

public:
    bool canLoot(Creature* pCreature) const;
    bool canDrainBlood(Creature* pCreature) const;
    bool canGainRankExp(Creature* pCreature) const;

    string getHostName(void) const {
        return m_HostName;
    }
    int getHostPartyID(void) const {
        return m_HostPartyID;
    }

    bool getComputeFlag(void) const {
        return m_bComputeFlag;
    }
    void setComputeFlag(bool bFlag) {
        m_bComputeFlag = bFlag;
    }

    string getQuestHostName() const {
        return m_QuestHostName;
    }
    void setQuestHostName(const string& name) {
        m_QuestHostName = name;
    }

    double getDamagePercent(const string& Name, int PartyID) const;

    string toString(void) const;

protected:
    unordered_map<string, PrecedenceElement*> m_CreatureMap;
    unordered_map<int, PrecedenceElement*> m_PartyMap;

    string m_FirstAttackerName; 
    int m_FirstAttackerPartyID; 

    string m_HostName; 
    int m_HostPartyID; 

    string m_QuestHostName; 

    bool m_bComputeFlag; 

    Damage_t m_TotalDamage; 
};

#endif
