//--------------------------------------------------------------------------------
//
// Filename    : GCShopSellOK.cpp






//
//--------------------------------------------------------------------------------

// include files
#include "GCShopSellOK.h"

#include "Assert1.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopSellOK::GCShopSellOK()

{
    __BEGIN_TRY

    __END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopSellOK::~GCShopSellOK()

{
    __BEGIN_TRY


    __END_CATCH_NO_RETHROW;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopSellOK::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_ObjectID);
    iStream.read(m_Version);
    iStream.read(m_ItemObjectID);
    iStream.read(m_Price);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopSellOK::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_ObjectID);
    oStream.write(m_Version);
    oStream.write(m_ItemObjectID);
    oStream.write(m_Price);

    __END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopSellOK::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCShopSellOKHandler::execute(this, pPlayer);

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCShopSellOK::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCShopSellOK("
        << "ObjectID:" << m_ObjectID << ",Version:" << (int)m_Version << ",ItemObejctID:" << m_ItemObjectID
        << ",Price:" << m_Price << ")";
    return msg.toString();

    __END_CATCH
}
