#include "LuaSelectItem.h"

LuaSelectItem::LuaSelectItem(LuaState* pState) : LuaScript(pState) {
    m_ItemClass = (Item::ItemClass)0;
    m_ItemType = 0;
    m_OptionType = 0;
}

int LuaSelectItem::executeFile(const string& filename)

{
    __BEGIN_TRY

    int result = luaL_dofile(m_pState->getState(), filename.c_str());

    lua_getglobal(m_pState->getState(), "ItemClass");
    lua_getglobal(m_pState->getState(), "ItemType");
    lua_getglobal(m_pState->getState(), "OptionType");
    lua_getglobal(m_pState->getState(), "OptionType2");

    // Phase 9: range-checked enum casts. lua_toboundedenum throws
    // InvalidProtocolException if the stack slot is non-numeric or out
    // of range, instead of silently truncating to an in-range-looking
    // enumerator.
    //
    //   ItemClass     : enum, 0 .. ITEM_CLASS_MAX-1
    //   ItemType_t    : WORD (uint16), 0 .. 65535
    //   OptionType_t  : BYTE (uint8),  0 .. 255
    lua_State* L = m_pState->getState();
    m_ItemClass = lua_toboundedenum<Item::ItemClass>(L, 1, 0, Item::ITEM_CLASS_MAX - 1);
    m_ItemType = lua_toboundedenum<ItemType_t>(L, 2, 0, 0xFFFF);
    m_OptionType = lua_toboundedenum<OptionType_t>(L, 3, 0, 0xFF);
    m_OptionType2 = lua_toboundedenum<OptionType_t>(L, 4, 0, 0xFF);

    // cout << "ItemClass=" << ItemClass
    //	<< ", ItemType=" << ItemType
    //	<< ", OptionType=" << OptionType << endl;

    return result;

    __END_CATCH
}
