//////////////////////////////////////////////////////////////////////
//
// Filename    : GCHolyLandBonusInfo.cpp
// Written By  :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCHolyLandBonusInfo.h"


//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCHolyLandBonusInfo::GCHolyLandBonusInfo()

{}

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
GCHolyLandBonusInfo::~GCHolyLandBonusInfo()

{
    __BEGIN_TRY

    
    clearBloodBibleBonusInfoList();

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCHolyLandBonusInfo::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE ListNum;

    iStream.read(ListNum);
    for (int i = 0; i < ListNum; i++) {
        BloodBibleBonusInfo* pBloodBibleBonusInfo = new BloodBibleBonusInfo();
        pBloodBibleBonusInfo->read(iStream);
        m_BloodBibleBonusInfoList.push_back(pBloodBibleBonusInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCHolyLandBonusInfo::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE ListNum = m_BloodBibleBonusInfoList.size();
    oStream.write(ListNum);

    BloodBibleBonusInfoListConstItor itr = m_BloodBibleBonusInfoList.begin();
    for (; itr != m_BloodBibleBonusInfoList.end(); itr++) {
        (*itr)->write(oStream);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCHolyLandBonusInfo::clearBloodBibleBonusInfoList()

{
    __BEGIN_TRY

    
    while (!m_BloodBibleBonusInfoList.empty()) {
        BloodBibleBonusInfo* pBloodBibleBonusInfo = m_BloodBibleBonusInfoList.front();
        m_BloodBibleBonusInfoList.pop_front();
        SAFE_DELETE(pBloodBibleBonusInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCHolyLandBonusInfo::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCHolyLandBonusInfoHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet size
//////////////////////////////////////////////////////////////////////
PacketSize_t GCHolyLandBonusInfo::getPacketSize() const

{
    __BEGIN_TRY

    PacketSize_t PacketSize = szBYTE;

    BloodBibleBonusInfoListConstItor itr = m_BloodBibleBonusInfoList.begin();

    for (; itr != m_BloodBibleBonusInfoList.end(); itr++) {
        PacketSize += (*itr)->getSize();
    }

    return PacketSize;

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCHolyLandBonusInfo::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCHolyLandBonusInfo(";

    BloodBibleBonusInfoListConstItor itr = m_BloodBibleBonusInfoList.begin();
    for (; itr != m_BloodBibleBonusInfoList.end(); itr++) {
        msg << (*itr)->toString();
    }

    msg << ")";

    return msg.toString();

    __END_CATCH
}
