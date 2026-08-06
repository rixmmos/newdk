//////////////////////////////////////////////////////////////////////////////
// Filename    : PrecedenceTable.cpp
// Written by  : excel96
// Description :




//////////////////////////////////////////////////////////////////////////////

#include "PrecedenceTable.h"

#include "Creature.h"
#include "StringStream.h"

//////////////////////////////////////////////////////////////////////////////
// class PrecedenceElement member methods
//////////////////////////////////////////////////////////////////////////////

PrecedenceElement::PrecedenceElement() {
    m_Name = "";
    m_PartyID = -1;
    m_Damage = 0;

    getCurrentTime(m_Deadline);
}

void PrecedenceElement::setNextTime(void) {
    getCurrentTime(m_Deadline);
    m_Deadline.tv_sec += 30;
}

string PrecedenceElement::toString(void) const {
    StringStream msg;
    msg << "PrecedenceElement("
        << "Name:" << m_Name << ",PartyID:" << m_PartyID << ",Damage:" << m_Damage << ")";
    return msg.toString();
}


//////////////////////////////////////////////////////////////////////////////
// class PrecedenceTable member methods
//////////////////////////////////////////////////////////////////////////////

PrecedenceTable::PrecedenceTable() {
    m_FirstAttackerName = "";
    m_FirstAttackerPartyID = 0;
    m_HostName = "";
    m_HostPartyID = 0;
    m_bComputeFlag = false;
}

PrecedenceTable::~PrecedenceTable() {
    
    unordered_map<string, PrecedenceElement*>::iterator itr = m_CreatureMap.begin();
    for (; itr != m_CreatureMap.end(); itr++) {
        SAFE_DELETE(itr->second);
    }
    m_CreatureMap.clear();

    
    unordered_map<int, PrecedenceElement*>::iterator itr2 = m_PartyMap.begin();
    for (; itr2 != m_PartyMap.end(); itr2++) {
        SAFE_DELETE(itr2->second);
    }
    m_PartyMap.clear();
}
 

void PrecedenceTable::addPrecedence(const string& Name, int PartyID, int damage) {
    
    if (m_CreatureMap.empty()) {
        
        m_FirstAttackerName = Name;

        
        PrecedenceElement* pElement = new PrecedenceElement;
        pElement->setName(Name);
        pElement->setPartyID(-1);
        pElement->setDamage(damage);
        pElement->setNextTime();
        m_CreatureMap[Name] = pElement;
    } else {
        unordered_map<string, PrecedenceElement*>::iterator itr = m_CreatureMap.find(Name);
        if (itr == m_CreatureMap.end()) {
            
            PrecedenceElement* pElement = new PrecedenceElement;
            pElement->setName(Name);
            pElement->setPartyID(-1);
            pElement->setDamage(damage);
            pElement->setNextTime();
            m_CreatureMap[Name] = pElement;
        } else {
            
            PrecedenceElement* pElement = itr->second;
            pElement->setDamage(pElement->getDamage() + damage);
            pElement->setNextTime();
        }
    }

    
    
    
    //	int PartyID = PartyID;
    if (m_PartyMap.empty()) {
        PrecedenceElement* pElement = new PrecedenceElement;
        pElement->setPartyID(PartyID);
        pElement->setDamage(damage);
        pElement->setNextTime();
        m_PartyMap[PartyID] = pElement;
    } else {
        unordered_map<int, PrecedenceElement*>::iterator itr = m_PartyMap.find(PartyID);
        if (itr == m_PartyMap.end()) {
            PrecedenceElement* pElement = new PrecedenceElement;
            pElement->setPartyID(PartyID);
            pElement->setDamage(damage);
            pElement->setNextTime();
            m_PartyMap[PartyID] = pElement;
        } else {
            PrecedenceElement* pElement = itr->second;
            pElement->setDamage(pElement->getDamage() + damage);
            pElement->setNextTime();
        }
    }

    
    m_bComputeFlag = false;
}

void PrecedenceTable::heartbeat(const Timeval& currentTime) {
    unordered_map<string, PrecedenceElement*>::iterator c_before = m_CreatureMap.end();
    unordered_map<string, PrecedenceElement*>::iterator c_current = m_CreatureMap.begin();

    while (c_current != m_CreatureMap.end()) {
        PrecedenceElement* pElement = c_current->second;
        Assert(pElement != NULL);

        if (pElement->getDeadline() < currentTime) {
            if (c_before == m_CreatureMap.end()) {
                m_CreatureMap.erase(c_current);
                c_current = m_CreatureMap.begin();
            } else {
                m_CreatureMap.erase(c_current);
                c_current = c_before;
                c_current++;
            }
        } else {
            c_before = c_current++;
        }
    }

    unordered_map<string, PrecedenceElement*>::iterator p_before = m_CreatureMap.end();
    unordered_map<string, PrecedenceElement*>::iterator p_current = m_CreatureMap.begin();

    while (p_current != m_CreatureMap.end()) {
        PrecedenceElement* pElement = p_current->second;
        Assert(pElement != NULL);

        if (pElement->getDeadline() < currentTime) {
            if (p_before == m_CreatureMap.end()) {
                m_CreatureMap.erase(p_current);
                p_current = m_CreatureMap.begin();
            } else {
                m_CreatureMap.erase(p_current);
                p_current = p_before;
                p_current++;
            }
        } else {
            p_before = p_current++;
        }
    }
}

void PrecedenceTable::compute(void) {
    
    if (m_bComputeFlag)
        return;

    int MaxDamage = 0;
    string MaxDamageName = "";
    string SecondDamageName = "";
    int MaxDamagePartyID = 0;
    int SecondDamagePartyID = 0;

    Damage_t TotalDamage = 0;

    
    unordered_map<string, PrecedenceElement*>::const_iterator itr = m_CreatureMap.begin();
    for (; itr != m_CreatureMap.end(); itr++) {
        PrecedenceElement* pElement = itr->second;
        Assert(pElement != NULL);

        if (MaxDamage < pElement->getDamage()) {
            if (MaxDamageName == "") {
                MaxDamageName = pElement->getName();
                MaxDamage = pElement->getDamage();
            } else {
                SecondDamageName = MaxDamageName;
                MaxDamageName = pElement->getName();
                MaxDamage = pElement->getDamage();
            }
        }

        TotalDamage += pElement->getDamage();
    }

    m_TotalDamage = TotalDamage;

    if (MaxDamageName != "") {
        
        
        if (MaxDamageName == m_FirstAttackerName) {
            m_HostName = MaxDamageName;
        }
        
        
        
        
        else {
            if (SecondDamageName != "" && SecondDamageName == m_FirstAttackerName) {
                m_HostName = SecondDamageName;
            } else {
                m_HostName = MaxDamageName;
            }
        }
    } else {
        
        
        m_HostName = m_FirstAttackerName;
    }

    
    
    
    
    
    
    
    MaxDamage = 0;
    MaxDamagePartyID = -1;
    SecondDamagePartyID = -1;

    unordered_map<int, PrecedenceElement*>::const_iterator itr2 = m_PartyMap.begin();
    for (; itr2 != m_PartyMap.end(); itr2++) {
        PrecedenceElement* pElement = itr2->second;
        Assert(pElement != NULL);

        if (MaxDamage < pElement->getDamage()) {
            if (MaxDamagePartyID == -1) {
                MaxDamagePartyID = pElement->getPartyID();
                MaxDamage = pElement->getDamage();
            } else {
                SecondDamagePartyID = MaxDamagePartyID;
                MaxDamagePartyID = pElement->getPartyID();
                MaxDamage = pElement->getDamage();
            }
        }
    }

    if (MaxDamagePartyID != -1) {
        
        
        if (MaxDamagePartyID == m_FirstAttackerPartyID) {
            m_HostPartyID = MaxDamagePartyID;
        }
        
        
        
        
        else {
            if (SecondDamagePartyID != -1 && SecondDamagePartyID == m_FirstAttackerPartyID) {
                m_HostPartyID = SecondDamagePartyID;
            } else {
                m_HostPartyID = MaxDamagePartyID;
            }
        }
    } else {
        
        
        m_HostPartyID = m_FirstAttackerPartyID;
    }

    m_bComputeFlag = true;
}

bool PrecedenceTable::canLoot(Creature* pCreature) const {
    
    if (m_HostName == pCreature->getName())
        return true;

    
    int PartyID = pCreature->getPartyID();
    if (PartyID != 0 && m_HostPartyID == PartyID)
        return true;

    return false;
}

bool PrecedenceTable::canDrainBlood(Creature* pCreature) const {
    
    if (m_HostName == pCreature->getName())
        return true;

    
    int PartyID = pCreature->getPartyID();
    if (PartyID != 0 && m_HostPartyID == PartyID)
        return true;

    return false;
}

bool PrecedenceTable::canGainRankExp(Creature* pCreature) const {
    
    unordered_map<string, PrecedenceElement*>::const_iterator itr = m_CreatureMap.find(pCreature->getName());
    if (itr == m_CreatureMap.end())
        return false;

    return (m_TotalDamage >> 2) < itr->second->getDamage();
}

double PrecedenceTable::getDamagePercent(const string& Name, int PartyID) const {
    if (m_TotalDamage == 0)
        return 0.0;

    double ownDamage = 0.0;
    unordered_map<string, PrecedenceElement*>::const_iterator itr = m_CreatureMap.find(Name);
    if (itr != m_CreatureMap.end()) {
        ownDamage = (double)(itr->second->getDamage());
    }

    double partyDamage = 0.0;
    if (PartyID != 0) {
        unordered_map<int, PrecedenceElement*>::const_iterator itr = m_PartyMap.find(PartyID);
        if (itr != m_PartyMap.end()) {
            partyDamage = (double)(itr->second->getDamage());
        }
    }

    double maxDamage = (ownDamage > partyDamage ? ownDamage : partyDamage);

    return maxDamage / (double)m_TotalDamage;
}

string PrecedenceTable::toString(void) const {
    StringStream msg;
    msg << "PrecedenceTable("
        << "FirstAttackerName:" << m_FirstAttackerName << ",FirstAttackerPartyID:" << m_FirstAttackerPartyID
        << ",HostName:" << m_HostName << ",HostPartyID:" << m_HostPartyID << ",ComputeFlag:" << m_bComputeFlag;

    msg << "\n,CreatureMap:\n";

    unordered_map<string, PrecedenceElement*>::const_iterator itr1 = m_CreatureMap.begin();
    for (; itr1 != m_CreatureMap.end(); itr1++) {
        msg << itr1->second->toString() << ",";
    }

    msg << "\n,PartyMap:\n";

    unordered_map<int, PrecedenceElement*>::const_iterator itr2 = m_PartyMap.begin();
    for (; itr2 != m_PartyMap.end(); itr2++) {
        msg << itr2->second->toString() << ",";
    }

    msg << ")";

    return msg.toString();
}
