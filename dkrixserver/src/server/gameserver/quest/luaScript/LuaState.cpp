//--------------------------------------------------------------------------------
// LuaState.cpp
//--------------------------------------------------------------------------------
#include "LuaState.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//--------------------------------------------------------------------------------
// constructor / destructor
//--------------------------------------------------------------------------------
LuaState::LuaState() : m_pState(NULL) {}

LuaState::~LuaState() {
    release();
}

//--------------------------------------------------------------------------------
// init
//--------------------------------------------------------------------------------
//
// Phase 9 (2026-08-07): Lua sandbox whitelist.
//
// Quest scripts run untrusted Lua loaded from disk (data/lua/*.lua).
// luaL_openlibs() used to register every standard library, including
// io (filesystem read/write), os (os.execute, os.remove, os.getenv),
// debug (VM reflection), and package (require/loadlib, dynamic C
// module loading). Any one of those lets a malicious or buggy quest
// file read config, exfiltrate credentials, shell out, or patch the
// running VM.
//
// Only the libraries quest scripts actually use are loaded:
//
//   base    (print, tostring, pairs, ipairs, type, unpack, etc.)
//   math    (math.random, math.floor, math.sin, math.pi, etc.)
//   string  (string.format, string.sub, string.find, string.gsub)
//
// Scope audit (2026-08-07): grepped every *.lua under the repo
// (dkrixserver/data/lua/**, quest/luaScript/test/**) for io./os./
// debug./require|package./table./string./math. -- none of the real
// quest scripts call io/os/debug/package, and none call table.* or
// math.*/string.* via dotted form either (they use bare `random`/
// `getn`, which are Lua 4.x/5.0 compat globals). Verified against
// the actual liblua5.1 shipped here (WSL, liblua5.1-0-dev) that
// LUA_COMPAT_GETN is #undef'd and no compat global for `random`
// exists in stock Lua 5.1 at all -- so `randomseed()` below and any
// script calling bare `random`/`getn` already fails with "attempt
// to call a nil value" under the old luaL_openlibs() too. Dropping
// `table` from the whitelist does not change that pre-existing
// behavior; io is unambiguously unused and safe to drop.
//
// Pattern mirrors luaL_openlibs()'s own implementation in Lua 5.1
// linit.c: push the luaopen_* function, push its library name, call
// with one arg. luaopen_base uses "" for its name by 5.1 convention.
//
void LuaState::init(int stackSize) {
    open(stackSize);

    // Whitelisted standard libs only -- io/os/debug/package/table
    // deliberately excluded to prevent filesystem / process /
    // reflection / require escape from untrusted quest scripts.
    static const luaL_Reg kAllowedLibs[] = {
        {"", luaopen_base},
        {LUA_STRLIBNAME, luaopen_string},
        {LUA_MATHLIBNAME, luaopen_math},
        {NULL, NULL}
    };
    for (const luaL_Reg* lib = kAllowedLibs; lib->func; ++lib) {
        lua_pushcfunction(m_pState, lib->func);
        lua_pushstring(m_pState, lib->name);
        lua_call(m_pState, 1, 0);
    }

    randomseed();
}


//--------------------------------------------------------------------------------
// release
//--------------------------------------------------------------------------------
void LuaState::release() {
    if (m_pState != NULL)
        lua_close(m_pState);
    m_pState = NULL;
}

//--------------------------------------------------------------------------------
// dofile
//--------------------------------------------------------------------------------
int LuaState::dofile(const string& filename) {
    __BEGIN_TRY

    return luaL_dofile(m_pState, filename.c_str());

    __END_CATCH
}

//--------------------------------------------------------------------------------
// randomseed
//--------------------------------------------------------------------------------
void LuaState::randomseed() {
    char str[80];
    srand((unsigned int)time(NULL));
    sprintf(str, "randomseed(%d)", rand() % 10000);
    luaL_dostring(m_pState, str);
}

//--------------------------------------------------------------------------------
// open
//--------------------------------------------------------------------------------
void LuaState::open(int stackSize) {
    release();
    m_pState = lua_open();
}

//--------------------------------------------------------------------------------
// close
//--------------------------------------------------------------------------------
void LuaState::close() {
    if (m_pState != NULL)
        lua_close(m_pState);
}

//--------------------------------------------------------------------------------
// baselibopen
//--------------------------------------------------------------------------------
void LuaState::baselibopen() {
    luaopen_base(m_pState);
}

//--------------------------------------------------------------------------------
// mathlibopen
//--------------------------------------------------------------------------------
void LuaState::mathlibopen() {
    luaopen_math(m_pState);
}

//--------------------------------------------------------------------------------
// strlibopen
//--------------------------------------------------------------------------------
void LuaState::strlibopen() {
    luaopen_string(m_pState);
}

//--------------------------------------------------------------------------------
// iolibopen
//--------------------------------------------------------------------------------
void LuaState::iolibopen() {
    luaopen_io(m_pState);
}

//--------------------------------------------------------------------------------
// getError to String
//--------------------------------------------------------------------------------
const string& LuaState::getErrorToString(int result) {
    switch (result) {
    case LUA_ERRRUN: {
        static string e = "error while running the chunk";
        return e;
    } break;
    case LUA_ERRSYNTAX: {
        static string e = "syntax error during pre-compilation";
        return e;
    } break;
    case LUA_ERRMEM: {
        static string e = "memory allocation error";
        return e;
    } break;
    case LUA_ERRERR: {
        static string e = "error while running _ERRORMESSAGE.";
        return e;
    } break;
    case LUA_ERRFILE: {
        static string e = "error opening the file.";
        return e;
    } break;
    }

    static string e = "unknown error";
    return e;
}

//--------------------------------------------------------------------------------
// log Error
//--------------------------------------------------------------------------------
void LuaState::logError(int result) {
    if (isError(result))
        filelog("luaError.log", "%s", getErrorToString(result).c_str());
}
