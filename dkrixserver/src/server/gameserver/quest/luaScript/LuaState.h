//--------------------------------------------------------------------------------
// by sigi. 2002.12.12
//--------------------------------------------------------------------------------

#ifndef __LUA_STATE_H__
#define __LUA_STATE_H__

#include <sstream>

#include "Exception.h"
#include "Types.h"

extern "C" {
#if defined(__APPLE__)
#include <luajit-2.1/lauxlib.h>
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#else
#include <lua5.1/lauxlib.h>
#include <lua5.1/lua.h>
#include <lua5.1/lualib.h>
#endif
}


//--------------------------------------------------------------------------------
// Phase 9 (2026-08-07): Range-checked Lua-stack -> enum cast.
//
// Quest scripts hand enum values to the server as plain Lua numbers.
// The old pattern was:
//
//   m_ItemClass = (Item::ItemClass)lua_tonumber(L, 1);
//
// which silently accepts:
//   - nil / non-numeric stack slots (lua_tonumber returns 0 -- a
//     valid enumerator for most enums, so the bug is invisible)
//   - out-of-range integers that reinterpret-cast to a valid-looking
//     enumerator and crash downstream code holding an enum-indexed
//     dispatch table
//   - negative numbers that wrap when the enum's underlying type is
//     unsigned
//
// lua_toboundedenum<T>() rejects all three with an
// InvalidProtocolException carrying the slot index, observed value,
// and legal range, so a misbehaving script fails loudly instead of
// producing an invalid enum.
//
// Usage:
//   m_ItemClass = lua_toboundedenum<Item::ItemClass>(
//                     L, 1, 0, Item::ITEM_CLASS_MAX - 1);
//--------------------------------------------------------------------------------
template <typename T> inline T lua_toboundedenum(lua_State* L, int idx, long minValue, long maxValue) {
    if (!lua_isnumber(L, idx)) {
        std::ostringstream msg;
        msg << "lua_toboundedenum: stack slot " << idx << " is not a number (Lua type=" << lua_type(L, idx) << ")";
        throw InvalidProtocolException(msg.str());
    }
    long v = (long)lua_tonumber(L, idx);
    if (v < minValue || v > maxValue) {
        std::ostringstream msg;
        msg << "lua_toboundedenum: value " << v << " on stack slot " << idx << " is outside [" << minValue << ", "
            << maxValue << "]";
        throw InvalidProtocolException(msg.str());
    }
    return (T)v;
}


class LuaState {
public:
    static const int defaultStateSize = 100;

public:
    LuaState();
    virtual ~LuaState();


    virtual void init(int stackSize = defaultStateSize);
    virtual void release();

    lua_State* getState() const {
        return m_pState;
    }
    int dofile(const string& filename);


    static bool isError(int result) {
        return result != 0;
    }
    static const string& getErrorToString(int result);
    static void logError(int result);

    void randomseed();

protected:
    void open(int stackSize = defaultStateSize);
    void baselibopen();
    void mathlibopen();
    void strlibopen();
    void iolibopen();
    void close();


protected:
    lua_State* m_pState;
};

#endif
