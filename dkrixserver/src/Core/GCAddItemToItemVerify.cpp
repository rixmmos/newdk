//////////////////////////////////////////////////////////////////////////////
// Filename    : GCAddItemToItemVerify.cpp
// Written By  : excel96
// Description :
//////////////////////////////////////////////////////////////////////////////

#include "GCAddItemToItemVerify.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCAddItemToItemVerify::read(SocketInputStream& iStream)

{
    __BEGIN_TRY

    iStream.read(m_Code);

    switch (m_Code) {
    
    case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE:
    case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
        iStream.read(m_Parameter);
        break;
    case ADD_ITEM_TO_ITEM_VERIFY_THREE_ENCHANT_OK:
        iStream.read(m_Parameter);
        iStream.read(m_Parameter2);
        break;
    
    default:
        break;
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
void GCAddItemToItemVerify::write(SocketOutputStream& oStream) const

{
    __BEGIN_TRY

    oStream.write(m_Code);

    switch (m_Code) {
    
    case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE:
    case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
        oStream.write(m_Parameter);
        break;
    case ADD_ITEM_TO_ITEM_VERIFY_THREE_ENCHANT_OK:
        oStream.write(m_Parameter);
        oStream.write(m_Parameter2);
        break;
    
    default:
        break;
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// execute packet's handler
//////////////////////////////////////////////////////////////////////////////
void GCAddItemToItemVerify::execute(Player* pPlayer)

{
    __BEGIN_TRY

    GCAddItemToItemVerifyHandler::execute(this, pPlayer);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////

PacketSize_t GCAddItemToItemVerify::getPacketSize() const

{
    __BEGIN_TRY

    PacketSize_t size = szBYTE;

    switch (m_Code) {
    
    case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_FAIL_DECREASE:
    case ADD_ITEM_TO_ITEM_VERIFY_ENCHANT_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_MIXING_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_DETACHING_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_REVIVAL_OK:
    case ADD_ITEM_TO_ITEM_VERIFY_UP_GRADE_OK:
        size += szuint;
        break;

    
    default:
        break;
    }

    return size;

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////////////
// get packet's debug string
//////////////////////////////////////////////////////////////////////////////
string GCAddItemToItemVerify::toString() const

{
    __BEGIN_TRY

    StringStream msg;
    msg << "GCAddItemToItemVerify("
        << "Code : " << (int)m_Code << "Parameter : " << (int)m_Parameter << ")";
    return msg.toString();

    __END_CATCH
}
