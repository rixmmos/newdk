//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginThrottle.h
// Description : Rate limit on failed authentication, keyed by source IP
//               and by account name.
//
//////////////////////////////////////////////////////////////////////

#ifndef __LOGIN_THROTTLE_H__
#define __LOGIN_THROTTLE_H__

// include files
#include <time.h>

#include "Mutex.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////
//
// class LoginThrottle
//
// LoginPlayer::m_FailureCount (18-AY) is a uint on the connection object.
// It is allocated when a TCP connection is accepted and destroyed with it,
// so it is per-connection -- not per-account and not per-IP -- and the stock
// client opens a fresh connection for every login attempt (Execute_UI_LOGIN
// calls InitSocket, which releases and reconnects). Anyone who reconnects
// per guess, which is exactly what the normal client does, is unaffected by
// it. All that counter actually bounds is pipelined guesses on one socket.
//
// This class is the state that survives the connection. It is owned by
// LoginPlayerManager, lives for the process, and is keyed on the two things
// a brute-force attempt cannot avoid revealing: where the packet came from
// and which account it is aimed at.
//
//
// WHY TWO TABLES
//
// They fail differently and neither one covers the other:
//
//   - Per-IP stops one host grinding many accounts (password spraying). It
//     is weak against a botnet and must be generous, because in this genre
//     a household, a LAN party, an internet cafe or an entire CGNAT-ed
//     mobile ISP can share one address.
//   - Per-account stops a botnet grinding one account. It can be strict,
//     because a single account has exactly one legitimate owner, but on its
//     own it does nothing about spraying across many accounts.
//
//
// WHY A FIXED TABLE AND NOT A map<string, int>
//
// Both keys are chosen by the attacker: the account name comes straight out
// of the packet, and the source address is whatever host they connect from.
// A container that grows one node per distinct key is therefore itself a
// remote memory-exhaustion primitive -- the control would be the
// vulnerability. So each table is a fixed array of nSlots entries with a
// fixed-width key, allocated once, and it never grows: total footprint is
// 2 * nSlots * sizeof(Entry), roughly 400 KB, whatever the inbound rate.
//
// Lookup is open addressing with linear probing over a window of
// nProbeLength slots. A key lives in exactly one of the nProbeLength slots
// following its hash, or nowhere.
//
//
// WHAT HAPPENS WHEN A PROBE WINDOW IS FULL
//
// It fails OPEN -- the oldest/weakest record in the window is evicted and
// the new key takes its slot. Failing closed would mean an attacker could
// deny login to everyone by filling the table, which is a far worse outcome
// than letting some guesses through, so that direction is not on the table.
//
// The eviction rule is what keeps failing open from being free: the entry
// evicted is the one with the FEWEST recorded failures, ties broken by the
// oldest window. A record that has actually tripped its limit is therefore
// the hardest thing in its window to displace. To evict a record with N
// failures an attacker must first raise the other nProbeLength - 1 slots in
// that window to at least N and then bring in one more key, which costs
// roughly 7 * N failed logins for a record that only has to absorb N more to
// trip again -- and every one of those failed logins is counted by the other
// table, so the price is paid twice over. The per-IP table
// meters the attempts needed to evict from the account table, and evicting
// from the per-IP table needs source addresses that hash into one specific
// window, which cannot be chosen without actually holding those addresses.
// Expired entries are always preferred over live ones, so under normal load
// nothing live is ever evicted.
//
//
// WINDOW SEMANTICS
//
// Bucketed, not sliding, and deliberately not extended by traffic. The
// first failure stamps windowStart; failures accumulate until
// now - windowStart reaches the window length, at which point the record
// resets to zero on its next use. A counter that never decays eventually
// locks out legitimate users, and a window that is pushed forward by every
// new failure lets an attacker hold an innocent CGNAT address locked out
// indefinitely. Here the worst case for a bystander is bounded by the
// window length and nothing an attacker does can extend it.
//
// The honest cost of a fixed bucket is that up to 2 * threshold guesses can
// straddle a window boundary. At these magnitudes that is irrelevant, and
// it buys an O(1) record with no per-attempt list.
//
//////////////////////////////////////////////////////////////////////

class LoginThrottle {
public:
    // constructor
    LoginThrottle();

    // destructor
    ~LoginThrottle();

public:
    // Is this source address / account name currently over its limit?
    // Const and side-effect free: callers check before doing any database
    // work, and a check must never be able to extend a lockout.
    bool isIPThrottled(const string& ip) const;
    bool isAccountThrottled(const string& id) const;

    // Record one failed authentication. Call these only where the
    // *credentials* were wrong -- see the note in CLLoginHandler.cpp about
    // which login-error paths do and do not qualify.
    void recordIPFailure(const string& ip);
    void recordAccountFailure(const string& id);

    // Drop this account's failure record. Called when a login succeeds.
    //
    // There is deliberately no clearIP(). Proving knowledge of a password
    // proves who owns the account, so clearing the account record is right.
    // It proves nothing about the source address -- an attacker who holds
    // one valid credential of their own (registering an account is free)
    // could otherwise spend the IP budget, log in once as themselves, and
    // reset it, which turns the per-IP limit into no limit at all. The IP
    // record therefore measures the rate of wrong guesses from that source
    // and is cleared only by time.
    void clearAccount(const string& id);

    // get debug string
    string toString() const;

private:
    // Slots per table. Power of two, so the hash reduces with a mask.
    // A healthy server sees a handful of distinct failing keys per window;
    // this is orders of magnitude of headroom, and it is fixed cost.
    static const int nSlots = 4096;

    // Slots examined for one key before an insert has to evict.
    static const int nProbeLength = 8;

    // Longest key stored, terminator included. CLLogin::read rejects an ID
    // longer than 30 bytes and getHost() returns a dotted-quad, so nothing
    // legitimate is ever truncated.
    static const int nMaxKeyLength = 32;

    struct Entry {
        char key[nMaxKeyLength]; // empty string == slot is unused
        time_t windowStart;
        uint failures;
    };

private:
    static bool isLive(const Entry& entry, time_t now, int windowSeconds);
    static const Entry* findEntry(const Entry* table, const char* key, time_t now, int windowSeconds);
    static Entry* claimEntry(Entry* table, const char* key, time_t now, int windowSeconds);

    bool isThrottled_NOLOCKED(const Entry* table, const string& key, int threshold, int windowSeconds) const;

    // Returns true if this failure is the one that crossed the threshold,
    // so the caller can log the trip exactly once. Every later rejection is
    // silent: the reject path is reachable by anyone who can open a socket,
    // and a log line per attempt would be a write amplifier.
    bool recordFailure_NOLOCKED(Entry* table, const string& key, int threshold, int windowSeconds);

private:
    Entry m_IPTable[nSlots];
    Entry m_AccountTable[nSlots];

    // Policy, loaded once from loginserver.conf. A threshold <= 0 disables
    // that table entirely.
    int m_IPThreshold;
    int m_IPWindowSeconds;
    int m_AccountThreshold;
    int m_AccountWindowSeconds;

    // Reached today only from CLLoginHandler::execute, which runs on the
    // ClientManager thread under LoginPlayerManager::m_Mutex. The login
    // server does run other threads (GameServerManager, CBillingPlayerManager)
    // and packets they handle already reach into LoginPlayerManager, so this
    // is not left as a single-threaded assumption. The mutex is a leaf --
    // nothing here calls back out while holding it -- and the bodies it
    // guards allocate nothing, so there is no path that unwinds past
    // __LEAVE_CRITICAL_SECTION.
    mutable Mutex m_Mutex;
};

// global variable declaration
extern LoginThrottle* g_pLoginThrottle;

#endif
