//--------------------------------------------------------------------------------
//
// Filename    : GCShopMarketCondition.cpp


//
//--------------------------------------------------------------------------------

// include files
#include "GCShopMarketCondition.h"

#include "Assert1.h"


//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------

GCShopMarketCondition::GCShopMarketCondition()

{
    __BEGIN_TRY

    __END_CATCH;
}

//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
GCShopMarketCondition::~GCShopMarketCondition()

{
    __BEGIN_TRY

    __END_CATCH_NO_RETHROW;
}

//--------------------------------------------------------------------

//--------------------------------------------------------------------
void GCShopMarketCondition::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    // read object id
    iStream.read(m_ObjectID);

    iStream.read(m_MarketCondBuy);

    iStream.read(m_MarketCondSell);

    __END_CATCH
}


//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void GCShopMarketCondition::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    // write object id
    oStream.write(m_ObjectID);

    oStream.write(m_MarketCondBuy);

    oStream.write(m_MarketCondSell);

    __END_CATCH
}

//--------------------------------------------------------------------------------
// execute packet's handler
//--------------------------------------------------------------------------------
void GCShopMarketCondition::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCShopMarketConditionHandler::execute(this, pPlayer);

    __END_CATCH
}

//--------------------------------------------------------------------------------
// get packet's debug string
//--------------------------------------------------------------------------------
string GCShopMarketCondition::toString() const

{
    __BEGIN_TRY

    StringStream msg;

    msg << "GCShopMarketCondition("
        << "ObjectID: " << (int)m_ObjectID << ", "
        << "MarketCondBuy: " << (int)m_MarketCondBuy << ", "
        << "MarketCondsell: " << (int)m_MarketCondSell;
    msg << ")";

    return msg.toString();

    __END_CATCH
}
