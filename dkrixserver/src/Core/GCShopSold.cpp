//--------------------------------------------------------------------------------
//
// Filename    : GCShopSold.cpp





//
//--------------------------------------------------------------------------------

// include files
#include "GCShopSold.h"

#include "Assert1.h"

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopSold::GCShopSold()

{
    __BEGIN_TRY

    __END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopSold::~GCShopSold()

{
    __BEGIN_TRY

    __END_CATCH_NO_RETHROW;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopSold::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_ObjectID);
    iStream.read(m_Version);
    iStream.read(m_RackType);
    iStream.read(m_RackIndex);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopSold::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_ObjectID);
    oStream.write(m_Version);
    oStream.write(m_RackType);
    oStream.write(m_RackIndex);

    __END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopSold::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCShopSoldHandler::execute(this, pPlayer);

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCShopSold::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCShopSold("
        << "ObjectID:" << (int)m_ObjectID << ",ShopVersion: " << (int)m_Version << ",ShopRackType: " << (int)m_RackType
        << ",ShopRackIndex: " << (int)m_RackIndex << ")";
    return msg.toString();

    __END_CATCH
}
