//////////////////////////////////////////////////////////////////////
//
// Filename    : GameServerManager.cpp
// Written By  : Reiot
// Description :
//
//////////////////////////////////////////////////////////////////////

#include "GameServerManager.h"

#include <unistd.h>

#include <set>

#include "DB.h"
#include "Datagram.h"
#include "DatagramPacket.h"
#include "GameServerInfoManager.h"
#include "LGKickCharacter.h"
#include "LogClient.h"
#include "Properties.h"
#include "SocketAPI.h"

//////////////////////////////////////////////////////////////////////
// Source-address gate for the login server's UDP port.
//
// Unlike the game server's UDP port, this one is not client-facing at all: it
// carries only inter-server datagrams from peer game servers. Those datagrams
// carry no authentication of any kind, and PACKET_GG_COMMAND -- the GM command
// packet -- has a factory registered on this server too
// (PacketFactoryManager.cpp, the `defined(__GAME_SERVER__) ||
// defined(__LOGIN_SERVER__)` block), so it can be constructed here from any
// source. Everything is therefore accepted only from an address this server
// would itself exchange inter-server datagrams with.
//
// The allowlist below names the client-facing packets rather than the peer-only
// ones, so any datagram packet added later is peer-gated by default. It is the
// same shape as the game server's gate in
// server/gameserver/LoginServerManager.cpp.
//////////////////////////////////////////////////////////////////////
namespace {

// Datagram packet IDs a game client may legitimately send to this port.
//
// Deliberately empty. Cross-checking Datagram::isDatagram() (Core/Datagram.cpp)
// against the factories registered under __LOGIN_SERVER__ leaves only GG_/GL_/
// LG_/GM_ packets, every one of which is sent by a game server. The one
// client-originated datagram in the protocol, PACKET_CG_PORT_CHECK, has its
// factory registered under __GAME_SERVER__ only and is addressed to the game
// server's UDP port; it cannot even be constructed here. The function is kept
// so a future client-facing datagram has one obvious place to be declared, and
// so anything not named here stays gated.
bool isClientDatagramPacket(PacketID_t packetID) {
    switch (packetID) {
    default:
        return false;
    }
}

// A source counts as a peer when it is the local host or the IP of any row
// GameServerInfoManager loaded from the GameServerInfo table -- exactly the
// game servers this login server serves. There is no configured peer address to
// add: unlike the game server, which knows "LoginServerIP", the login server
// never initiates an inter-server datagram; it only replies to the source of
// the datagram it just received (see GLIncomingConnectionHandler::execute).
//
// Loopback is always accepted. A single-host deployment -- including the Docker
// image, whose start-servers.sh runs all three servers in one container with
// LoginServerIP 127.0.0.1 -- delivers those datagrams with that source, and the
// kernel discards 127.0.0.0/8 source addresses seen on a non-loopback
// interface, so this cannot be reached from off-box.
bool isKnownPeerHost(const string& host) {
    if (host.compare(0, 4, "127.") == 0)
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
// with spoofed source addresses cannot grow it without bound; this runs only on
// the single GameServerManager thread, so no locking is needed.
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
GameServerManager::GameServerManager() : m_pDatagramSocket(NULL) {
    __BEGIN_TRY

    // create datagram server socket
    while (true) {
        try {
            m_pDatagramSocket = new DatagramSocket(g_pConfig->getPropertyInt("LoginServerUDPPort"));
            break;
        } catch (BindException& be) {
            SAFE_DELETE(m_pDatagramSocket);
            cout << be.toString() << endl;
            sleep(1);
        }
    }

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////////
GameServerManager::~GameServerManager() {
    __BEGIN_TRY

    if (m_pDatagramSocket != NULL) {
        delete m_pDatagramSocket;
        m_pDatagramSocket = NULL;
    }

    __END_CATCH_NO_RETHROW
}

//////////////////////////////////////////////////////////////////////
// stop thread
//////////////////////////////////////////////////////////////////////
void GameServerManager::stop() {
    __BEGIN_TRY

    throw UnsupportedError();

    __END_CATCH
}

//////////////////////////////////////////////////////////////////////
// main method
//////////////////////////////////////////////////////////////////////
void GameServerManager::run() {
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

        // How many datagrams one pass of the loop below may handle.
        //
        // This loop used to sleep 1ms per pass and handle exactly one datagram
        // per pass, capping the login server's UDP port at roughly 1000
        // datagrams a second (the game server's mirror of this loop slept
        // twice, for half that). The socket is blocking, so the sleep was not
        // guarding against a busy spin on an empty socket -- receive() already
        // parks the thread until a datagram arrives. It was a pure throughput
        // throttle, and the datagrams it could drop include the GL_/LG_ login
        // handoff, so saturating it stops anyone from logging in. Each pass now
        // drains what is queued instead.
        //
        // The bound keeps a flood from running the drain forever, so the pass
        // still returns to the top of the loop where the exception handlers and
        // any future periodic work live. 256 matches the game server's bound in
        // server/gameserver/LoginServerManager.cpp and is far above any
        // legitimate burst on this port, which carries only inter-server
        // traffic.
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
                        // cout << pDatagramPacket->toString() << endl;

                        /*
                        // UDP test
                        cout << "[DatagramPacket] " << pDatagram->getHost() << ":"
                                                    << pDatagram->getPort() << endl;

                        if (pDatagram->getPort()!=9997)
                        {
                            LGKickCharacter lg;
                            lg.setPCName("111");
                            lg.setID(111);

                            sendPacket(pDatagram->getHost(), pDatagram->getPort(), &lg);
                        }
                        */

                        pDatagram->read(pDatagramPacket);

                        if (pDatagramPacket != NULL) {
                            // Datagram::read() has already stamped the sender's
                            // address onto the packet. Fail closed: anything that is
                            // not client traffic must come from a known peer.
                            const PacketID_t packetID = pDatagramPacket->getPacketID();
                            const string sourceHost = pDatagramPacket->getHost();

                            if (!isClientDatagramPacket(packetID) && !isKnownPeerHost(sourceHost)) {
                                logRejectedDatagram(sourceHost, pDatagramPacket->getPort(), packetID);
                            } else {
                                pDatagramPacket->execute(NULL);
                            }


                            delete pDatagramPacket;
                            pDatagramPacket = NULL;
                        }


                        delete pDatagram;
                        pDatagram = NULL;
                    }

                    // Nothing more is queued, so end the pass here rather than
                    // block inside it. FIONREAD also reads 0 when the next
                    // queued datagram is zero-length; that one is consumed by
                    // the blocking receive at the top of the next pass, so no
                    // datagram is lost and the loop cannot spin.
                    if (SocketAPI::availablesocket_ex(m_pDatagramSocket->getSOCKET()) == 0)
                        break;
                }
            } catch (ProtocolException& pe) {
                cout << "GameServerManager::run Exception Check(ProtocolException)" << endl;
                cout << pe.toString() << endl;


                // throw Error( pe.toString() );
                delete pDatagramPacket;
                delete pDatagram;
            } catch (ConnectException& ce) {
                cout << "GameServerManager::run Exception Check(ConnectException)" << endl;
                cout << ce.toString() << endl;


                // throw Error( ce.toString() );
                delete pDatagramPacket;
                delete pDatagram;
            } catch (Throwable& t) {
                cout << "GameServerManager::run Exception Check(ConnectException)" << endl;
                cout << t.toString() << endl;
                delete pDatagramPacket;
                delete pDatagram;
            }
        }

        cout << "GameServerManager thread exiting... " << endl;
        //::exit(1);
    } catch (Throwable& t) {
        log(LOG_LOGINSERVER_ERROR, "", "", t.toString());
        cout << "GameServerManager thread exiting... : " << t.toString() << endl;
    }
}


//////////////////////////////////////////////////////////////////////
// send datagram to datagram-socket
//////////////////////////////////////////////////////////////////////
void GameServerManager::sendDatagram(Datagram* pDatagram) {
    __BEGIN_TRY

    try {
        m_pDatagramSocket->send(pDatagram);
    } catch (ConnectException& t) {
        cout << "GameServerManager::sendDatagram Exception Check!!" << endl;
        cout << t.toString() << endl;
        throw ConnectException("GameServerManager::sendDatagram  ");
    }

    __END_CATCH
}


//////////////////////////////////////////////////////////////////////
// send datagram-packet to datagram-socket
//////////////////////////////////////////////////////////////////////
void GameServerManager::sendPacket(string host, uint port, DatagramPacket* pPacket) {
    __BEGIN_TRY
    __BEGIN_DEBUG

    try {
        //	try
        //	{


        Datagram datagram;

        datagram.setHost(host);
        datagram.setPort(port);


        datagram.write(pPacket);


        m_pDatagramSocket->send(&datagram);
        //	}
        //	catch ( ConnectException & t )
        //	{
        //		cout << "GameServerManager::sendDatagram Exception Check!!" << endl;
        //		cout << t.toString() << endl;

        //	}

    } catch (Throwable& t) {
        cout << "====================================================================" << endl;
        cout << t.toString() << endl;
        cout << "====================================================================" << endl;
    }

    __END_DEBUG
    __END_CATCH
}


// global variable definition
GameServerManager* g_pGameServerManager = NULL;
