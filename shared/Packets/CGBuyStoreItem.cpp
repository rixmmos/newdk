////////////////////////////////////////////////////////////////////////////////
// Filename    : CGBuyStoreItem.cpp

// Description :



////////////////////////////////////////////////////////////////////////////////

#include "CGBuyStoreItem.h"

void CGBuyStoreItem::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_OwnerObjectID);
    iStream.read(m_ItemObjectID);
    iStream.read(m_Index);

    __END_CATCH
}

void CGBuyStoreItem::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_OwnerObjectID);
    oStream.write(m_ItemObjectID);
    oStream.write(m_Index);

    __END_CATCH
}

void CGBuyStoreItem::execute(Player* pPlayer)

{
    __BEGIN_TRY

    CGBuyStoreItemHandler::execute(this, pPlayer);

    __END_CATCH
}

string CGBuyStoreItem::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "CGBuyStoreItem("
        << ")";
    return msg.toString();

    __END_CATCH
}
