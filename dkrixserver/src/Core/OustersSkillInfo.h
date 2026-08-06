//////////////////////////////////////////////////////////////////////
//
// Filename    : OustersSkillInfo.h
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __OUSTERS_SKILL_INFO_H__
#define __OUSTERS_SKILL_INFO_H__

// include files
#include "Exception.h"
#include "PCSkillInfo.h"
#include "Packet.h"
#include "SubOustersSkillInfo.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class OustersSkillInfo;
//

//
//////////////////////////////////////////////////////////////////////

class OustersSkillInfo : public PCSkillInfo {
public:
    // constructor
    OustersSkillInfo();

    // destructor
    ~OustersSkillInfo();

public:
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // get packet's body size
    
    PacketSize_t getSize();

    static uint getMaxSize() {
        return szBYTE + szBYTE + (SubOustersSkillInfo::getMaxSize() * 120);
    }

    // get packet's debug string
    string toString() const;

    // get / set New Skill
    bool isLearnNewSkill() const {
        return m_bLearnNewSkill;
    }
    void setLearnNewSkill(bool NewSkill) {
        m_bLearnNewSkill = NewSkill;
    }

    // get / set ListNumber
    BYTE getListNum() const {
        return m_ListNum;
    }
    void setListNum(BYTE ListNum) {
        m_ListNum = ListNum;
    }

    // add / delete / clear S List
    void addListElement(SubOustersSkillInfo* pSubOustersSkillInfo) {
        m_SubOustersSkillInfoList.push_back(pSubOustersSkillInfo);
    }

    // ClearList
    void clearList() {
        m_SubOustersSkillInfoList.clear();
        m_ListNum = 0;
    }

    // pop front Element in Status List
    SubOustersSkillInfo* popFrontListElement() {
        SubOustersSkillInfo* TempSubOustersSkillInfo = m_SubOustersSkillInfoList.front();
        m_SubOustersSkillInfoList.pop_front();
        return TempSubOustersSkillInfo;
    }

private:
    
    bool m_bLearnNewSkill;

    // SubOustersSkillInfo List Total Number
    BYTE m_ListNum;

    // SubOustersSkillInfo List
    list<SubOustersSkillInfo*> m_SubOustersSkillInfoList;
};

#endif
