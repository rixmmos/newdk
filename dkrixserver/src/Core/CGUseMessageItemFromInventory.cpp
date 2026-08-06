//////////////////////////////////////////////////////////////////////////////
// Filename    : CGUseMessageItemFromInventory.cpp
// Written By  : excel96
// Description :


//////////////////////////////////////////////////////////////////////////////

#include "CGUseMessageItemFromInventory.h"

#include "Assert1.h"


void CGUseMessageItemFromInventory::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    CGUseItemFromInventory::read(iStream);

    BYTE szMessage;

    // message
    iStream.read(szMessage);

    if (szMessage == 0)
        throw InvalidProtocolException("szMessage == 0");

    if (szMessage > 128)
        throw InvalidProtocolException("too large message length");

    iStream.read(m_Message, szMessage);


    __END_CATCH
}

void CGUseMessageItemFromInventory::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    CGUseItemFromInventory::write(oStream);

    // message
    BYTE szMessage = m_Message.size();

    if (szMessage == 0)
        throw InvalidProtocolException("szMessage == 0");

    if (szMessage > 128)
        throw InvalidProtocolException("too large message length");

    oStream.write(szMessage);

    oStream.write(m_Message);


    __END_CATCH
}

void CGUseMessageItemFromInventory::execute(Player* pPlayer)

{
    __BEGIN_TRY

    CGUseMessageItemFromInventoryHandler::execute(this, pPlayer);

    __END_CATCH
}

string CGUseMessageItemFromInventory::toString() const {
    __BEGIN_TRY

    StringStream msg;
    msg << "CGUseMessageItemFromInventory("
        << "ObjectID:" << (int)getObjectID() << ",InvenX:" << (int)getX() << ",InvenY:" << (int)getY()
        << ",msg:" << m_Message.c_str() << ")";
    return msg.toString();

    __END_CATCH
}
