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
// Phase 9B (2026-04-19): Lua sandbox whitelist.
//
// Quest scripts run untrusted Lua loaded from disk. The old path
// called luaL_openlibs(), which registers EVERY standard library
// including:
//
//   io       - file I/O (io.open, io.read, io.write, io.popen)
//   os       - os.execute, os.remove, os.rename, os.exit, os.getenv
//   debug    - debug.sethook, debug.getupvalue, sandbox escape
//   package  - require/loadlib, dynamic loading of C modules
//   (coroutine is bundled with base in 5.1; unexposed by choice)
//
// Any of those is enough for a malicious quest file to read
// config, exfiltrate creds, shell out, or patch the VM at runtime.
//
// Only the four libraries actually used by quest scripts are
// loaded here:
//
//   base     (print, tostring, pairs, ipairs, type, unpack, etc.)
//   table    (table.insert, table.remove, table.concat)
//   string   (string.format, string.sub, string.find, string.gsub)
//   math     (math.random, math.floor, math.sin, math.pi, etc.)
//
// Pattern mirrors luaL_openlibs' own implementation from Lua 5.1
// linit.c: push the luaopen_* function, push its lib name, call
// with one arg (the name). `luaopen_base` uses "" by convention.
//--------------------------------------------------------------------------------
void LuaState::init(int stackSize) {
    open(stackSize);

    // Whitelisted standard libs only — io/os/debug/package deliberately
    // excluded to prevent filesystem / process / reflection escape.
    static const luaL_Reg kAllowedLibs[] = {
        {"",              luaopen_base},
        {LUA_TABLIBNAME,  luaopen_table},
        {LUA_STRLIBNAME,  luaopen_string},
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
