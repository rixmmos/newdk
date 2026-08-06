//////////////////////////////////////////////////////////////////////
//
// Filename    : GCSweeperBonusInfo.cpp
// Written By  :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCSweeperBonusInfo.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCSweeperBonusInfo::GCSweeperBonusInfo()

{}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCSweeperBonusInfo::~GCSweeperBonusInfo()

{
    __BEGIN_TRY

    
    clearSweeperBonusInfoList();

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSweeperBonusInfo::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE ListNum;

    iStream.read(ListNum);
    for (int i = 0; i < ListNum; i++) {
        SweeperBonusInfo* pSweeperBonusInfo = new SweeperBonusInfo();
        pSweeperBonusInfo->read(iStream);
        m_SweeperBonusInfoList.push_back(pSweeperBonusInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCSweeperBonusInfo::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE ListNum = m_SweeperBonusInfoList.size();
    oStream.write(ListNum);

    SweeperBonusInfoListConstItor itr = m_SweeperBonusInfoList.begin();
    for (; itr != m_SweeperBonusInfoList.end(); itr++) {
        (*itr)->write(oStream);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCSweeperBonusInfo::clearSweeperBonusInfoList()

{
    __BEGIN_TRY

    
    while (!m_SweeperBonusInfoList.empty()) {
        SweeperBonusInfo* pSweeperBonusInfo = m_SweeperBonusInfoList.front();
        m_SweeperBonusInfoList.pop_front();
        SAFE_DELETE(pSweeperBonusInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCSweeperBonusInfo::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCSweeperBonusInfoHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet size
//////////////////////////////////////////////////////////////////////
PacketSize_t GCSweeperBonusInfo::getPacketSize() const

{
    __BEGIN_TRY

    PacketSize_t PacketSize = szBYTE;

    SweeperBonusInfoListConstItor itr = m_SweeperBonusInfoList.begin();

    for (; itr != m_SweeperBonusInfoList.end(); itr++) {
        PacketSize += (*itr)->getSize();
    }

    return PacketSize;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCSweeperBonusInfo::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCSweeperBonusInfo(";

    SweeperBonusInfoListConstItor itr = m_SweeperBonusInfoList.begin();
    for (; itr != m_SweeperBonusInfoList.end(); itr++) {
        msg << (*itr)->toString();
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}
