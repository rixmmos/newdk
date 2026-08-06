//--------------------------------------------------------------------------------
//
// Filename   : Assert.cpp
// Written By : Reiot
//
//--------------------------------------------------------------------------------

// include files
#include "Assert1.h"

#include <time.h>

#include "Exception.h"
#include "Types.h"

//--------------------------------------------------------------------------------
//
// __assert__
//

//
//--------------------------------------------------------------------------------
void __assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false) {
    StringStream msg;

    msg << "\nAssertion Failed : " << file << " : " << line;

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
void __protocol_assert__(const char* file, uint line, const char* func, const char* expr) noexcept(false) {
    StringStream msg;

    msg << "\nProtocol Assertion Failed : " << file << " : " << line;

    if (func)
        msg << " : " << func;

    time_t currentTime = time(0);

    msg << expr << " at " << ctime(&currentTime);

    ofstream ofile("protocol_assertion_failed.log", ios::app);
    ofile << msg.toString() << endl;
    ofile.close();

    throw InvalidProtocolException(msg.toString());
}
