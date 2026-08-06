//////////////////////////////////////////////////////////////////////
//
// Filename    : GCUnionOfferList.cpp
// Written By  :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "GCUnionOfferList.h"

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GCUnionOfferList::~GCUnionOfferList()

{
    __BEGIN_TRY
    while (!m_UnionOfferList.empty()) {
        SingleGuildUnionOffer* pUnionOffer = m_UnionOfferList.front();

        SAFE_DELETE(pUnionOffer);

        m_UnionOfferList.pop_front();
    }

    m_UnionOfferList.clear();

    __END_CATCH_NO_RETHROW
}

PacketSize_t GCUnionOfferList::getPacketSize() const {
    PacketSize_t ret = szBYTE;

    list<SingleGuildUnionOffer*>::const_iterator itr = m_UnionOfferList.begin();

    for (; itr != m_UnionOfferList.end(); ++itr) {
        ret += (*itr)->getSize();
    }

    return ret;
}

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCUnionOfferList::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    BYTE szNum;
    iStream.read(szNum);
    for (int i = 0; i < szNum; ++i) {
        SingleGuildUnionOffer* pInfo = new SingleGuildUnionOffer;
        pInfo->read(iStream);
        m_UnionOfferList.push_back(pInfo);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
void GCUnionOfferList::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    BYTE szNum = m_UnionOfferList.size();
    oStream.write(szNum);

    list<SingleGuildUnionOffer*>::const_iterator itr = m_UnionOfferList.begin();

    for (; itr != m_UnionOfferList.end(); ++itr) {
        (*itr)->write(oStream);
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////
void GCUnionOfferList::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCUnionOfferListHandler::execute(this, pPlayer);

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////
string GCUnionOfferList::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCUnionOfferList("
        << ")";

    return msg.toString();

    __END_CATCH
}
