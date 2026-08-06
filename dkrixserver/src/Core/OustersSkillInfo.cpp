//////////////////////////////////////////////////////////////////////
//
// Filename    : OustersSkillInfo.cpp
// Written By  :
// Description :
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// include files
//////////////////////////////////////////////////////////////////////
#include "OustersSkillInfo.h"

#include "Assert.h"
#include "SocketInputStream.h"
#include "SocketOutputStream.h"

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
OustersSkillInfo::OustersSkillInfo() {
    __BEGIN_TRY
    m_bLearnNewSkill = false;
    m_ListNum = 0;
    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
OustersSkillInfo::~OustersSkillInfo() {
    __BEGIN_TRY

    
    while (!m_SubOustersSkillInfoList.empty()) {
        SubOustersSkillInfo* pSubOustersSkillInfo = m_SubOustersSkillInfoList.front();
        SAFE_DELETE(pSubOustersSkillInfo);
        m_SubOustersSkillInfoList.pop_front();
    }

    __END_CATCH_NO_RETHROW
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void OustersSkillInfo::read(SocketInputStream& iStream) {
    __BEGIN_TRY

    
    iStream.read(m_bLearnNewSkill);
    iStream.read(m_ListNum);

    for (int i = 0; i < m_ListNum; i++) {
        SubOustersSkillInfo* pSubOustersSkillInfo = new SubOustersSkillInfo();
        pSubOustersSkillInfo->read(iStream);
        m_SubOustersSkillInfoList.push_back(pSubOustersSkillInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void OustersSkillInfo::write(SocketOutputStream& oStream) const {
    __BEGIN_TRY

    
    oStream.write(m_bLearnNewSkill);
    oStream.write(m_ListNum);

    for (list<SubOustersSkillInfo*>::const_iterator itr = m_SubOustersSkillInfoList.begin();
         itr != m_SubOustersSkillInfoList.end(); itr++) {
        Assert(*itr != NULL);
        (*itr)->write(oStream);
    }

    __END_CATCH
}

//--------------------------------------------------------------------
// getSize
//--------------------------------------------------------------------
PacketSize_t OustersSkillInfo::getSize() {
    PacketSize_t PacketSize = szBYTE + szBYTE;

    for (list<SubOustersSkillInfo*>::const_iterator itr = m_SubOustersSkillInfoList.begin();
         itr != m_SubOustersSkillInfoList.end(); itr++) {
        PacketSize += (*itr)->getSize();
    }

    return PacketSize;
}


//////////////////////////////////////////////////////////////////////
//
// get packet's debug string
//
//////////////////////////////////////////////////////////////////////
string OustersSkillInfo::toString() const {
    __BEGIN_TRY

    StringStream msg;

    msg << "OustersSkillInfo( ListNum:" << (int)m_ListNum << " ListSet( ";

    for (list<SubOustersSkillInfo*>::const_iterator itr = m_SubOustersSkillInfoList.begin();
         itr != m_SubOustersSkillInfoList.end(); itr++) {
        Assert(*itr != NULL);
        msg << (*itr)->toString() << ",";
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}
