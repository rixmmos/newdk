//--------------------------------------------------------------------------------
//
// Filename   : Assert.cpp
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Assert.h"

#include <time.h>

#include "Exception.h"
#include "Types.h"

//--------------------------------------------------------------------------------
//
// __assert__
//

//
//--------------------------------------------------------------------------------
// [[noreturn]] is repeated on the definition so the compiler verifies the body
// really cannot fall off the end (-Winvalid-noreturn); the header carries the
// declaration every caller sees.
[[noreturn]] void __assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false) {
    StringStream msg;

    msg << "\n"
        << "Assertion Failed : " << file << " : " << line;

    if (func)
        msg << " : " << func;

    time_t currentTime = time(0);

    msg << expr << " at " << ctime(&currentTime);

    ofstream ofile("assertion_failed.log", ios::app);
    ofile << msg.toString() << endl;
    ofile.close();

    throw AssertionError(msg.toString());
}

//--------------------------------------------------------------------------------
//


//
//--------------------------------------------------------------------------------
[[noreturn]] void __protocol_assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false) {
    StringStream msg;

    msg << "\n"
        << "Protocol Assertion Failed : " << file << " : " << line;

    if (func)
        msg << " : " << func;

    time_t currentTime = time(0);

    msg << expr << " at " << ctime(&currentTime);

    ofstream ofile("protocol_assertion_failed.log", ios::app);
    ofile << msg.toString() << endl;
    ofile.close();

    throw InvalidProtocolException(msg.toString());
}
