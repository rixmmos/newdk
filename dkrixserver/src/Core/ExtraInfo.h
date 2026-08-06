//////////////////////////////////////////////////////////////////////
//
// Filename    : ExtraInfo.h
// Written By  : elca@ewestsoft.com

//
//////////////////////////////////////////////////////////////////////

#ifndef __EXTRA_INFO_H__
#define __EXTRA_INFO_H__

// include files
#include "Exception.h"
#include "ExtraSlotInfo.h"
#include "Packet.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class ExtraInfo;
//

//
//////////////////////////////////////////////////////////////////////

class ExtraInfo {
public:
    // constructor
    ExtraInfo();

    // destructor
    ~ExtraInfo() noexcept;

public:
    
    
    void read(SocketInputStream& iStream);

    
    void write(SocketOutputStream& oStream) const;

    // get packet's body size
    
    PacketSize_t getSize();

    static uint getMaxSize() {
        return szBYTE + (ExtraSlotInfo::getMaxSize() * 1);
    }

    // get packet's debug string
    string toString() const;

    // get / set ListNumber
    BYTE getListNum() const {
        return m_ListNum;
    }
    void setListNum(BYTE ListNum) {
        m_ListNum = ListNum;
    }

    // add / delete / clear S List
    void addListElement(ExtraSlotInfo* pExtraSlotInfo) {
        m_ExtraSlotInfoList.push_back(pExtraSlotInfo);
    }

    // ClearList
    void clearList() {
        m_ExtraSlotInfoList.clear();
        m_ListNum = 0;
    }

    // pop front Element in Status List
    ExtraSlotInfo* popFrontListElement() {
        ExtraSlotInfo* TempExtraSlotInfo = m_ExtraSlotInfoList.front();
        m_ExtraSlotInfoList.pop_front();
        return TempExtraSlotInfo;
    }

private:
    // ExtraSlotInfo List Total Number
    BYTE m_ListNum;

    // ExtraSlotInfo List
    list<ExtraSlotInfo*> m_ExtraSlotInfoList;
};

#endif
