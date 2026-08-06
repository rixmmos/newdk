//-------------------------------------------------------------------------------- //
// Filename    : GCRankBonusInfo.cpp
// Written By  : elca@ewestsoft.com
// Description :
//
//--------------------------------------------------------------------------------

// include files
#include "GCRankBonusInfo.h"

#include "Assert1.h"

//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
GCRankBonusInfo::GCRankBonusInfo()

{}

//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
GCRankBonusInfo::~GCRankBonusInfo()

{
    m_RankBonusInfoList.clear();
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCRankBonusInfo::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE ListNum;
    iStream.read(ListNum);

    for (WORD i = 0; i < ListNum; i++) {
        DWORD rankBonusType;
        iStream.read(rankBonusType);
        m_RankBonusInfoList.push_back(rankBonusType);
    }

    __END_CATCH
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCRankBonusInfo::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    //--------------------------------------------------
    // write pc type
    //--------------------------------------------------
    BYTE size = m_RankBonusInfoList.size();
    oStream.write(size);

    for (list<DWORD>::const_iterator itr = m_RankBonusInfoList.begin(); itr != m_RankBonusInfoList.end(); itr++) {
        oStream.write((*itr));
    }

    __END_CATCH
}


//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCRankBonusInfo::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCRankBonusInfoHandler::execute(this, pPlayer);

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCRankBonusInfo::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCRankBonusInfo(";
    for (list<DWORD>::const_iterator itr = m_RankBonusInfoList.begin(); itr != m_RankBonusInfoList.end(); itr++) {
        msg << (*itr) << ",";
    }
    msg << ")";

    return msg.toString();

    __END_CATCH
}
