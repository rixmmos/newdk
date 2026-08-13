//////////////////////////////////////////////////////////////////////
//
// Filename    : LoginServerManager.cpp
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

// include files
#include "LoginServerManager.h"

#include <unistd.h>

#include <set>

#include "Assert.h"
#include "DB.h"
#include "Datagram.h"
#include "DatagramPacket.h"
#include "GameServerInfoManager.h"
#include "LogClient.h"
#include "Properties.h"
#include "SocketAPI.h"
#include "ThreadManager.h"
#include "ThreadPool.h"
#include "TimeChecker.h"
#include "Timeval.h"

//////////////////////////////////////////////////////////////////////
// Source-address gate for the gameserver's UDP port.
//
// This port is client-facing by design -- real clients send
// PACKET_CG_PORT_CHECK to it -- but the same socket also carries inter-server
// datagrams (GG_* from peer gameservers, LG_* from the loginserver). Those
// carry no authentication of any kind, and PACKET_GG_COMMAND in particular is
// dispatched straight into the GM command handlers. So everything that is not
// client traffic is accepted only from an address this server would itself
// send inter-server datagrams to.
//
// The allowlist below names the client-facing packets rather than the
// peer-only ones, so any datagram packet added later is peer-gated by default.
//////////////////////////////////////////////////////////////////////
namespace {

// Datagram packet IDs a game client may legitimately send to this port.
bool isClientDatagramPacket(PacketID_t packetID) {
    switch (packetID) {
    case Packet::PACKET_CG_PORT_CHECK:
        return true;
    default:
        return false;
    }
}

// A source counts as a peer when it is the local host, the configured login
// server, or the IP of any row GameServerInfoManager loaded from the
// GameServerInfo table -- exactly the addresses this server sends inter-server
// datagrams to (see CGSayHandler::opworld and LoginServerManager::sendPacket).
//
// Loopback is always accepted. A single-host deployment runs login, shared and
// game servers on 127.0.0.1 and their datagrams arrive with that source, and
// the kernel discards 127.0.0.0/8 source addresses seen on a non-loopback
// interface, so this cannot be reached from off-box.
bool isKnownPeerHost(const string& host) {
    if (host.compare(0, 4, "127.") == 0)
        return true;

    if (g_pConfig != NULL && g_pConfig->hasKey("LoginServerIP") && g_pConfig->getProperty("LoginServerIP") == host)
        return true;

    if (g_pGameServerInfoManager == NULL)
        return false;

    HashMapGameServerInfo** pGameServerInfos = g_pGameServerInfoManager->getGameServerInfos();

    if (pGameServerInfos == NULL)
        return false;

    // WorldID indices start at 1; index 0 is never allocated by
    // GameServerInfoManager::load().
    const int maxWorldID = g_pGameServerInfoManager->getMaxWorldID();
    const int maxServerGroupID = g_pGameServerInfoManager->getMaxServerGroupID();

    for (int worldID = 1; worldID < maxWorldID; worldID++) {
        for (int groupID = 0; groupID < maxServerGroupID; groupID++) {
            const HashMapGameServerInfo& infos = pGameServerInfos[worldID][groupID];

            for (HashMapGameServerInfo::const_iterator itr = infos.begin(); itr != infos.end(); itr++) {
                if (itr->second != NULL && itr->second->getIP() == host)
                    return true;
            }
        }
    }

    return false;
}

// Log a dropped datagram once per source host. The set is capped so a flood
// with spoofed source addresses cannot grow it without bound; this runs only
// on the single LoginServerManager thread, so no locking is needed.
void logRejectedDatagram(const string& host, uint port, PacketID_t packetID) {
    const size_t MAX_LOGGED_HOSTS = 64;

    static set<string> loggedHosts;
    static bool capReported = false;

    if (loggedHosts.size() >= MAX_LOGGED_HOSTS) {
        if (!capReported) {
            capReported = true;
            filelog("datagram.txt", "peer-only datagram: further drops suppressed after %u distinct sources",
                    (uint)MAX_LOGGED_HOSTS);
        }
        return;
    }

    if (!loggedHosts.insert(host).second)
        return;

    filelog("datagram.txt", "dropped peer-only datagram id:%u from %s:%u (not a known peer)", (uint)packetID,
            host.c_str(), port);
}

} // namespace

//////////////////////////////////////////////////////////////////////
// constructor
//////////////////////////////////////////////////////////////////////
LoginServerManager::LoginServerManager() : m_pDatagramSocket(NULL) {
    __BEGIN_TRY

    m_Mutex.setName("LoginServerManager");

    // create datagram server socket
    while (true) {
        try {
            m_pDatagramSocket = new DatagramSocket(g_pConfig->getPropertyInt("GameServerUDPPort"));
            break;
        } catch (BindException& be) {
            SAFE_DELETE(m_pDatagramSocket);
            cout << "LoginServerManager(" << g_pConfig->getPropertyInt("GameServerUDPPort") << ") : " << be.toString()
                 << endl;
            sleep(1);
        }
    }

    //	m_pDatagramSocket = new DatagramSocket(g_pConfig->getPropertyInt("GameServerUDPPort"));

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
LoginServerManager::~LoginServerManager() {
    __BEGIN_TRY

    SAFE_DELETE(m_pDatagramSocket);

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////
// stop thread
//////////////////////////////////////////////////////////////////////
void LoginServerManager::stop() {
    __BEGIN_TRY

    throw UnsupportedError(__PRETTY_FUNCTION__);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////
void LoginServerManager::run() {
    try {
        string host = g_pConfig->getProperty("DB_HOST");
        string db = g_pConfig->getProperty("DB_DB");
        string user = g_pConfig->getProperty("DB_USER");
        string password = g_pConfig->getProperty("DB_PASSWORD");
        uint port = 0;
        if (g_pConfig->hasKey("DB_PORT"))
            port = g_pConfig->getPropertyInt("DB_PORT");

        Connection* pConnection = new Connection(host, db, user, password, port);
        g_pDatabaseManager->addConnection((int)(long)Thread::self(), pConnection);
        cout << "************************************************************************" << endl;
        cout << "************************************************************************" << endl;
        cout << "************************************************************************" << endl;
        cout << "************************************************************************" << endl;
        cout << "OPEN LOGIN DB" << endl;
        cout << "************************************************************************" << endl;
        cout << "************************************************************************" << endl;
        cout << "************************************************************************" << endl;

        Timeval dummyQueryTime;
        getCurrentTime(dummyQueryTime);

        // How many datagrams one pass of the loop below may handle.
        //
        // This loop used to sleep 1ms twice per pass and handle exactly one
        // datagram per pass, which capped the gameserver's UDP port at roughly
        // 500 datagrams a second. The socket is blocking, so those sleeps were
        // not guarding against a busy spin on an empty socket -- receive()
        // already parks the thread until a datagram arrives. They were a pure
        // throughput throttle, and since this port is reachable from the
        // internet, a trickle of junk traffic was enough to push the GL_/LG_
        // login handoff out of the kernel's receive queue and stop anyone from
        // logging in. Each pass now drains what is queued instead.
        //
        // The bound exists so a flood cannot keep the drain running forever and
        // starve the periodic work at the bottom of the pass. 256 is far above
        // any legitimate burst -- inter-server traffic plus client port checks
        // are a handful of datagrams per second -- while still bounding one
        // pass to a few milliseconds of work.
        const int MAX_DATAGRAMS_PER_PASS = 256;

        while (true) {
            Datagram* pDatagram = NULL;
            DatagramPacket* pDatagramPacket = NULL;

            try {
                for (int drained = 0; drained < MAX_DATAGRAMS_PER_PASS; drained++) {
                    pDatagram = NULL;
                    pDatagramPacket = NULL;

                    // Blocking receive. On an empty socket this parks the
                    // thread, which is this thread's idle state; NULL means the
                    // datagram that arrived carried no payload.
                    pDatagram = m_pDatagramSocket->receive();

                    if (pDatagram != NULL) {
                        // cout << "[Datagram] " << pDatagram->getHost() << ":" << pDatagram->getPort() << endl;
                        pDatagram->read(pDatagramPacket);

                        if (pDatagramPacket != NULL) {
                            // cout << "[DatagramPacket] " << pDatagram->getHost() << ":"
                            //      << pDatagram->getPort() << endl;

                            // Datagram::read() has already stamped the sender's
                            // address onto the packet. Fail closed: anything that
                            // is not client traffic must come from a known peer.
                            const PacketID_t packetID = pDatagramPacket->getPacketID();
                            const string sourceHost = pDatagramPacket->getHost();

                            if (!isClientDatagramPacket(packetID) && !isKnownPeerHost(sourceHost)) {
                                logRejectedDatagram(sourceHost, pDatagramPacket->getPort(), packetID);
                            } else {
                                __ENTER_CRITICAL_SECTION(m_Mutex)

                                pDatagramPacket->execute(NULL);

                                __LEAVE_CRITICAL_SECTION(m_Mutex)
                            }

                            SAFE_DELETE(pDatagramPacket);
                        }


                        SAFE_DELETE(pDatagram);
                    }

                    // Nothing more is queued, so end the pass here rather than
                    // block inside it -- the periodic work below has to keep
                    // running. FIONREAD also reads 0 when the next queued
                    // datagram is zero-length; that one is consumed by the
                    // blocking receive at the top of the next pass, so no
                    // datagram is lost and the loop cannot spin.
                    if (SocketAPI::availablesocket_ex(m_pDatagramSocket->getSOCKET()) == 0)
                        break;
                }
            } catch (ProtocolException& pe) {
                cerr << "----------------------------------------------------------------------" << endl;
                cerr << "GameServerManager::run Exception Check(ProtocolException)" << endl;
                cerr << pe.toString() << endl;
                cerr << "----------------------------------------------------------------------" << endl;
                SAFE_DELETE(pDatagramPacket);
                SAFE_DELETE(pDatagram);


                // throw Error(pe.toString());

                filelog("LOGINSERVERMANAGER.log", "LoginServerManager::run() 1 : %s", pe.toString().c_str());
            } catch (ConnectException& ce) {
                cerr << "----------------------------------------------------------------------" << endl;
                cerr << "GameServerManager::run Exception Check(ConnectException)" << endl;
                cerr << ce.toString() << endl;
                cerr << "----------------------------------------------------------------------" << endl;
                SAFE_DELETE(pDatagramPacket);
                SAFE_DELETE(pDatagram);


                // throw Error(ce.toString());

                filelog("LOGINSERVERMANAGER.log", "LoginServerManager::run() 2 : %s", ce.toString().c_str());
            } catch (Throwable& t) {
                cerr << "----------------------------------------------------------------------" << endl;
                cerr << "GameServerManager::run Exception Check(Throwable)" << endl;
                cerr << t.toString() << endl;
                cerr << "----------------------------------------------------------------------" << endl;
                SAFE_DELETE(pDatagramPacket);
                SAFE_DELETE(pDatagram);

                filelog("LOGINSERVERMANAGER.log", "LoginServerManager::run() 3 : %s", t.toString().c_str());
            }

            Timeval currentTime;
            getCurrentTime(currentTime);

            if (dummyQueryTime < currentTime) {
                g_pDatabaseManager->executeDummyQuery(pConnection);


                dummyQueryTime.tv_sec += (60 + rand() % 30) * 60;
            }


            g_pTimeChecker->heartbeat();
        }
    } catch (Throwable& t) {
        filelog("LOGINSERVERMANAGER.log", "LoginServerManager::run() 4 : %s", t.toString().c_str());

        cerr << t.toString() << endl;
    }
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
void LoginServerManager::sendDatagram(Datagram* pDatagram) {
    __BEGIN_TRY

    m_pDatagramSocket->send(pDatagram);

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// send datagram packet to login server
//////////////////////////////////////////////////////////////////////
void LoginServerManager::sendPacket(const string& host, uint port, DatagramPacket* pPacket) {
    __BEGIN_TRY
    __BEGIN_DEBUG

    try {
        Datagram datagram;

        datagram.setHost(host);
        datagram.setPort(port);


        datagram.write(pPacket);


        m_pDatagramSocket->send(&datagram);
    } catch (Throwable& t) {
        // cerr << "====================================================================" << endl;
        // cerr << t.toString() << endl;
        // cerr << "====================================================================" << endl;
    }

    __END_DEBUG
    __END_CATCH
}

// global variable definition
LoginServerManager* g_pLoginServerManager = NULL;
