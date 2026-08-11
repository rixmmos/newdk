//--------------------------------------------------------------------------------
//
// Filename   : CheckedCast.cpp
//
//--------------------------------------------------------------------------------

// include files
#include "CheckedCast.h"

#include "StringStream.h"
#include "Types.h"

//--------------------------------------------------------------------------------
//
// __invalid_cast__
//
// [[noreturn]] is repeated on the definition so the compiler verifies the body
// really cannot fall off the end (-Winvalid-noreturn); the header carries the
// declaration every caller sees.
//
//--------------------------------------------------------------------------------
[[noreturn]] void __invalid_cast__(const std::type_info& targetType,
                                   const std::type_info* pSourceType) noexcept(false) {
    StringStream msg;

    msg << "checkedCast to " << targetType.name() << " failed : ";

    if (pSourceType == NULL)
        msg << "source pointer was NULL";
    else
        msg << "source object is a " << pSourceType->name();

    throw InvalidCastError(msg.toString());
}
