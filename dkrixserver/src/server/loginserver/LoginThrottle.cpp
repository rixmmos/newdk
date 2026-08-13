//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginThrottle.cpp
// Description : Rate limit on failed authentication, keyed by source IP
//               and by account name. See LoginThrottle.h for the design.
//
//////////////////////////////////////////////////////////////////////

// include files
#include "LoginThrottle.h"

#include <string.h>

#include "Properties.h"
#include "StringStream.h"
#include "Utility.h"

//--------------------------------------------------------------------------------
//
// THE NUMBERS, AND WHY THESE NUMBERS
//
// The failure modes that matter here are false positives, not false
// negatives. A limit that is an order of magnitude too loose still turns an
// unbounded guess rate into a bounded one; a limit that is slightly too
// tight locks real players out of the game. Both defaults are therefore set
// against measured legitimate behaviour with room to spare, and only
// wrong-credential failures are counted at all (CLLoginHandler.cpp explains
// which login errors qualify) -- so none of the correlated mass-failure
// modes, the ones that hit many players at once after a restart or a
// billing hiccup, feed either counter.
//
// Per account: 10 failures / 600 s.
//
//   A player mistyping a password gets it right or gives up in one to three
//   attempts; caps lock or a password changed on the website by somebody
//   else in the household is the bad case, and that is still under ten in a
//   sitting. There is no automatic client-side retry to worry about --
//   INVALID_ID_PASSWORD puts the client in MODE_LOGIN_WRONG and waits for
//   the user to click again -- so ten failures inside ten minutes means ten
//   deliberate human attempts. 10/600 s is one guess per minute sustained
//   against a given account, from any number of hosts, which is where a
//   distributed attack against one account actually stops. Worst case for a
//   player who does burn the budget is a wait bounded by ten minutes.
//
// Per IP: 30 failures / 300 s.
//
//   This is the one that has to tolerate shared addresses, and in this
//   genre they are the norm rather than the exception: a household with
//   several players, a LAN party, a PC-bang with dozens of machines, or a
//   carrier-grade NAT putting an entire mobile ISP behind one address.
//   Thirty wrong passwords inside five minutes from one address means
//   thirty separate people each typing a password wrong in the same five
//   minutes, which does not happen on a shared connection during normal
//   play now that only credential failures count.
//
//   The window is deliberately the short one and the threshold the loose
//   one: the cost of a false positive is a bystander waiting, so bound that
//   wait at five minutes and pay for it with a higher count. Sustained,
//   this is six wrong guesses a minute from any single address, down from
//   unbounded.
//
// Both are tunable in loginserver.conf, and a threshold of 0 or less
// disables that table. The keys are optional -- getPropertyInt throws when
// absent, so the load is wrapped and falls back to the default, which means
// conf files that predate the keys (including the untracked live one) keep
// working unchanged.
//
//--------------------------------------------------------------------------------
static const int defaultIPThreshold = 30;
static const int defaultIPWindowSeconds = 300;
static const int defaultAccountThreshold = 10;
static const int defaultAccountWindowSeconds = 600;

namespace {

//--------------------------------------------------------------------------------
// Read an optional integer from loginserver.conf.
//--------------------------------------------------------------------------------
int loadConfigInt(const char* key, int defaultValue) {
    if (g_pConfig != NULL) {
        try {
            return g_pConfig->getPropertyInt(key);
        } catch (Throwable&) {
            // Key absent from this conf file. Fall through to the default.
        }
    }

    return defaultValue;
}

//--------------------------------------------------------------------------------
// FNV-1a.
//
// Not a keyed hash, and it does not need to be. The account table's keys are
// attacker-chosen, so an attacker can compute names that land in whichever
// probe window they like; a seeded hash would only raise the price of aiming
// at a window. What makes aiming insufficient is the eviction rule in
// claimEntry() below, which is where the cost actually lives.
//--------------------------------------------------------------------------------
uint hashKey(const char* key) {
    uint hash = 2166136261u;

    for (const char* p = key; *p != '\0'; p++) {
        hash ^= (uint)(unsigned char)(*p);
        hash *= 16777619u;
    }

    return hash;
}

//--------------------------------------------------------------------------------
// Copy a key into a fixed buffer, truncating rather than overflowing.
//
// Truncation is unreachable in practice: CLLogin::read rejects an ID longer
// than 30 bytes before the handler ever sees it, and Socket::getHost()
// returns a dotted quad. The bound is here because the buffer size, not the
// caller, is what guarantees the table's footprint.
//
// The stored key is NUL-terminated, so an ID containing an embedded NUL --
// the packet reader copies raw bytes and does not forbid one -- is keyed on
// its prefix. That can only merge two keys into one record, never split one,
// so it makes the limit stricter rather than looser and cannot be used to
// escape it. No legitimate ID can collide with a truncated one, because the
// ones in the database are text.
//--------------------------------------------------------------------------------
void copyKey(char* dst, size_t dstSize, const string& src) {
    size_t length = src.size();

    if (length > dstSize - 1)
        length = dstSize - 1;

    if (length > 0)
        memcpy(dst, src.data(), length);

    dst[length] = '\0';
}

} // namespace


//--------------------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------------------
LoginThrottle::LoginThrottle() {
    __BEGIN_TRY

    m_Mutex.setName("LoginThrottle");

    for (int i = 0; i < nSlots; i++) {
        m_IPTable[i].key[0] = '\0';
        m_IPTable[i].windowStart = 0;
        m_IPTable[i].failures = 0;

        m_AccountTable[i].key[0] = '\0';
        m_AccountTable[i].windowStart = 0;
        m_AccountTable[i].failures = 0;
    }

    m_IPThreshold = loadConfigInt("LoginFailureIPThreshold", defaultIPThreshold);
    m_IPWindowSeconds = loadConfigInt("LoginFailureIPWindowSeconds", defaultIPWindowSeconds);
    m_AccountThreshold = loadConfigInt("LoginFailureAccountThreshold", defaultAccountThreshold);
    m_AccountWindowSeconds = loadConfigInt("LoginFailureAccountWindowSeconds", defaultAccountWindowSeconds);

    // A zero or negative window would make every record permanently expired
    // or permanently live depending on which comparison ran first. Treat it
    // as a typo and use the default; disabling is what a threshold <= 0 is
    // for, and that stays available.
    if (m_IPWindowSeconds <= 0)
        m_IPWindowSeconds = defaultIPWindowSeconds;

    if (m_AccountWindowSeconds <= 0)
        m_AccountWindowSeconds = defaultAccountWindowSeconds;

    __END_CATCH
}


//--------------------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------------------
LoginThrottle::~LoginThrottle() {
    __BEGIN_TRY

    // Both tables are members. Nothing was allocated, so nothing is freed --
    // that is the point of the fixed layout.

    __END_CATCH_NO_RETHROW
}


//--------------------------------------------------------------------------------
// Is this entry still inside its counting window?
//
// An unused slot (empty key) is never live. An expired slot is not live
// either, which is what makes failures age out and what makes the slot
// reusable without a sweep: the table cannot grow, so there is nothing for a
// periodic sweep to reclaim that the next probe would not reclaim anyway.
//--------------------------------------------------------------------------------
bool LoginThrottle::isLive(const Entry& entry, time_t now, int windowSeconds) {
    if (entry.key[0] == '\0')
        return false;

    // A clock that steps backwards must not freeze a record forever.
    if (now < entry.windowStart)
        return false;

    return (long)(now - entry.windowStart) < (long)windowSeconds;
}


//--------------------------------------------------------------------------------
// Find the live record for a key, or NULL. Never mutates the table.
//--------------------------------------------------------------------------------
const LoginThrottle::Entry* LoginThrottle::findEntry(const Entry* table, const char* key, time_t now,
                                                     int windowSeconds) {
    const uint base = hashKey(key);

    for (int i = 0; i < nProbeLength; i++) {
        const Entry& entry = table[(base + (uint)i) & (uint)(nSlots - 1)];

        if (entry.key[0] != '\0' && strcmp(entry.key, key) == 0)
            return isLive(entry, now, windowSeconds) ? &entry : NULL;
    }

    return NULL;
}


//--------------------------------------------------------------------------------
// Find or create the record for a key. Never returns NULL.
//
// Preference order within the probe window:
//
//   1. The key's own slot, if it already has one. An expired one is reset in
//      place, so a key occupies at most one slot in its window and cannot
//      accumulate duplicates.
//   2. The first unused or expired slot.
//   3. Otherwise evict -- fewest failures first, oldest window as the tie
//      break. See the header for why this is the rule that makes failing
//      open expensive rather than free.
//--------------------------------------------------------------------------------
LoginThrottle::Entry* LoginThrottle::claimEntry(Entry* table, const char* key, time_t now, int windowSeconds) {
    const uint base = hashKey(key);

    Entry* reusable = NULL;
    Entry* weakest = NULL;

    for (int i = 0; i < nProbeLength; i++) {
        Entry& entry = table[(base + (uint)i) & (uint)(nSlots - 1)];

        const bool live = isLive(entry, now, windowSeconds);

        if (entry.key[0] != '\0' && strcmp(entry.key, key) == 0) {
            if (!live) {
                entry.windowStart = now;
                entry.failures = 0;
            }

            return &entry;
        }

        if (!live) {
            if (reusable == NULL)
                reusable = &entry;

            continue;
        }

        if (weakest == NULL || entry.failures < weakest->failures ||
            (entry.failures == weakest->failures && entry.windowStart < weakest->windowStart))
            weakest = &entry;
    }

    // Every slot in the window is either reusable or live, so at least one of
    // the two candidates was set.
    Entry* slot = (reusable != NULL) ? reusable : weakest;

    // key came from copyKey(), so it already fits with its terminator.
    memcpy(slot->key, key, strlen(key) + 1);
    slot->windowStart = now;
    slot->failures = 0;

    return slot;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
bool LoginThrottle::isThrottled_NOLOCKED(const Entry* table, const string& key, int threshold,
                                         int windowSeconds) const {
    if (threshold <= 0)
        return false;

    char buffer[nMaxKeyLength];
    copyKey(buffer, nMaxKeyLength, key);

    const Entry* entry = findEntry(table, buffer, time(NULL), windowSeconds);

    return entry != NULL && entry->failures >= (uint)threshold;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
bool LoginThrottle::recordFailure_NOLOCKED(Entry* table, const string& key, int threshold, int windowSeconds) {
    if (threshold <= 0)
        return false;

    char buffer[nMaxKeyLength];
    copyKey(buffer, nMaxKeyLength, key);

    Entry* entry = claimEntry(table, buffer, time(NULL), windowSeconds);

    // Saturate rather than wrap. A wrapped counter would read as zero
    // failures, which is both a free pass and the cheapest thing in the
    // window to evict.
    if (entry->failures < 0xFFFFFFFFu)
        entry->failures++;

    return entry->failures == (uint)threshold;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
bool LoginThrottle::isIPThrottled(const string& ip) const {
    __BEGIN_TRY

    bool result = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    result = isThrottled_NOLOCKED(m_IPTable, ip, m_IPThreshold, m_IPWindowSeconds);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return result;

    __END_CATCH
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
bool LoginThrottle::isAccountThrottled(const string& id) const {
    __BEGIN_TRY

    bool result = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    result = isThrottled_NOLOCKED(m_AccountTable, id, m_AccountThreshold, m_AccountWindowSeconds);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return result;

    __END_CATCH
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LoginThrottle::recordIPFailure(const string& ip) {
    __BEGIN_TRY

    bool tripped = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    tripped = recordFailure_NOLOCKED(m_IPTable, ip, m_IPThreshold, m_IPWindowSeconds);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    // Logged outside the lock, and only on the crossing.
    if (tripped)
        filelog("loginthrottle.txt", "ip %s reached %d failed logins in %d seconds", ip.c_str(), m_IPThreshold,
                m_IPWindowSeconds);

    __END_CATCH
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LoginThrottle::recordAccountFailure(const string& id) {
    __BEGIN_TRY

    bool tripped = false;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    tripped = recordFailure_NOLOCKED(m_AccountTable, id, m_AccountThreshold, m_AccountWindowSeconds);

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    if (tripped)
        filelog("loginthrottle.txt", "account %s reached %d failed logins in %d seconds", id.c_str(),
                m_AccountThreshold, m_AccountWindowSeconds);

    __END_CATCH
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void LoginThrottle::clearAccount(const string& id) {
    __BEGIN_TRY

    if (m_AccountThreshold <= 0)
        return;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    char buffer[nMaxKeyLength];
    copyKey(buffer, nMaxKeyLength, id);

    const uint base = hashKey(buffer);

    for (int i = 0; i < nProbeLength; i++) {
        Entry& entry = m_AccountTable[(base + (uint)i) & (uint)(nSlots - 1)];

        if (entry.key[0] != '\0' && strcmp(entry.key, buffer) == 0) {
            // Release the slot outright rather than zeroing the count. The
            // capacity is worth more than the record.
            entry.key[0] = '\0';
            entry.windowStart = 0;
            entry.failures = 0;
            break;
        }
    }

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    __END_CATCH
}


//--------------------------------------------------------------------------------
// get debug string
//--------------------------------------------------------------------------------
string LoginThrottle::toString() const {
    __BEGIN_TRY

    StringStream msg;

    __ENTER_CRITICAL_SECTION(m_Mutex)

    const time_t now = time(NULL);

    int liveIPs = 0;
    int liveAccounts = 0;

    for (int i = 0; i < nSlots; i++) {
        if (isLive(m_IPTable[i], now, m_IPWindowSeconds))
            liveIPs++;

        if (isLive(m_AccountTable[i], now, m_AccountWindowSeconds))
            liveAccounts++;
    }

    msg << "LoginThrottle(ip:" << liveIPs << "/" << (int)nSlots << " limit " << m_IPThreshold << "/"
        << m_IPWindowSeconds << "s, account:" << liveAccounts << "/" << (int)nSlots << " limit " << m_AccountThreshold
        << "/" << m_AccountWindowSeconds << "s)";

    __LEAVE_CRITICAL_SECTION(m_Mutex)

    return msg.toString();

    __END_CATCH
}


// global variable definition
LoginThrottle* g_pLoginThrottle = NULL;
