//--------------------------------------------------------------------------------
//
// Filename   : CheckedCast.h
//
// A dynamic_cast that cannot silently yield NULL.
//
// The tree performs its entire type dispatch with unchecked downcasts: about
// 3,700 `dynamic_cast<T*>(...)` sites, the majority of which dereference the
// result on the very next line. When such a cast fails the code walks off a NULL
// pointer, which is undefined behaviour and, at -O2, the source of most of the
// -Wnull-dereference inventory reported by the warning-scan CI leg (the deref
// usually lands inside a trivial inline getter, so GCC points at the callee's
// header instead of the guilty handler).
//
// checkedCast<T*>(p) is a drop-in replacement for those sites: same cast, same
// success path, but a NULL result raises InvalidCastError instead of being
// handed on. It deliberately mirrors dynamic_cast's spelling - the target is
// written as a pointer type - so converting a site is a pure token rename.
//
// Sites that legitimately test the result for NULL must keep using
// dynamic_cast. dynamic_cast *is* the non-throwing variant; introducing a
// synonym for it would only add churn.
//
//--------------------------------------------------------------------------------

#ifndef __CHECKED_CAST_H__
#define __CHECKED_CAST_H__

#include <typeinfo>

#include <type_traits>

#include "Exception.h"
#include "Types.h"

//--------------------------------------------------------------------------------
//
// class InvalidCastError
//
// Raised when checkedCast() cannot produce the requested type.
//
// It derives from AssertionError, and that choice is load-bearing:
//
//  * A failed downcast is a broken server-side type invariant, not malformed
//    client input. The object graph is server-owned, so no sequence of bytes on
//    the wire can make `dynamic_cast<PlayerCreature*>(pCreature)` fail. Throwing
//    from the ProtocolException family would therefore mislabel it - and worse,
//    GamePlayer::sendPacket() silently swallows InvalidProtocolException.
//
//  * __END_DEBUG_EX (Exception.h) rethrows Error and ProtocolException but
//    *swallows* every other Exception into packet_exception.txt and lets the
//    handler return normally. 456 files under Core/ close their handlers with
//    that macro, so anything derived from Exception - NoSuchElementException,
//    InvalidArgumentException, RuntimeException - would turn a failed cast into
//    a packet that quietly appears to have succeeded. Deriving from Error is
//    what keeps the throw visible.
//
//  * Being an AssertionError specifically, rather than a bare Error, means the
//    converted sites behave exactly as the `Assert(pFoo != NULL)` lines that
//    already sit next to many of them: same base class, so every existing
//    `catch (Error&)` / `catch (Throwable&)` / `catch (...)` site is unchanged.
//    In the gameserver that path is GamePlayer::processInput(), which logs the
//    player, host, MAC and packet dump to GamePlayerError.txt and then raises
//    DisconnectException - i.e. one connection is dropped, the server lives.
//
//--------------------------------------------------------------------------------
class InvalidCastError : public AssertionError {
public:
    InvalidCastError() : AssertionError() {}
    InvalidCastError(const string& msg) : AssertionError(msg) {}
    string getName() const {
        return "InvalidCastError";
    }
};

//--------------------------------------------------------------------------------
//
// __invalid_cast__
//
// Cold path of checkedCast(), kept out of line (CheckedCast.cpp) so the inline
// fast path is nothing but a dynamic_cast and a NULL test. Marked [[noreturn]]
// for the same reason __assert__ is: it ends in an unconditional throw, and
// without the attribute every caller reads as a live fall-through.
//
// pSourceType is NULL when the source pointer itself was NULL; otherwise it is
// the *dynamic* type of the source object, which is the informative half of the
// diagnostic. Names come from typeid() and are compiler-mangled.
//
//--------------------------------------------------------------------------------
[[noreturn]] void __invalid_cast__(const std::type_info& targetType, const std::type_info* pSourceType) noexcept(false);

//--------------------------------------------------------------------------------
//
// checkedCast<Target*>(p)
//
// dynamic_cast<Target*>(p), except that a NULL result throws InvalidCastError.
// Never returns NULL.
//
//--------------------------------------------------------------------------------
template <typename TargetPtr, typename Source> inline TargetPtr checkedCast(Source* pSource) {
    static_assert(std::is_pointer<TargetPtr>::value,
                  "checkedCast requires a pointer target type, e.g. checkedCast<Foo*>(p)");

    TargetPtr pTarget = dynamic_cast<TargetPtr>(pSource);

    if (pTarget == NULL)
        __invalid_cast__(typeid(TargetPtr),
                         pSource == NULL ? static_cast<const std::type_info*>(NULL) : &typeid(*pSource));

    return pTarget;
}

#endif
