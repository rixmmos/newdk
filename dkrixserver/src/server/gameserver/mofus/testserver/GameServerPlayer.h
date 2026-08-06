//////////////////////////////////////////////////////////////////////
//
// Filename    : GameServerPlayer.h
// Written by  : reiot@ewestsoft.com
// Description :
//
//////////////////////////////////////////////////////////////////////

#ifndef __GAME_SERVER_PLAYER_H__
#define __GAME_SERVER_PLAYER_H__

// include files
#include "Exception.h"
#include "Player.h"
#include "Types.h"

class MPacket;

//////////////////////////////////////////////////////////////////////
//
// class GameServerPlayer
//
//////////////////////////////////////////////////////////////////////

class GameServerPlayer : public Player {
public:
    // constructor
    GameServerPlayer() throw(Error);
    GameServerPlayer(Socket* pSocket) throw(Error);

    // destructor
    virtual ~GameServerPlayer() throw(Error);

    virtual void processInput() throw(IOException, Error);

    virtual void processOutput() throw(IOException, Error);

    // parse packet and execute handler for the packet
    virtual void processCommand() throw(IOException, Error);

    // disconnect
    
    
    
    virtual void disconnect(bool bDisconnected = DISCONNECTED) throw(InvalidProtocolException, Error);

    void sendPacket(MPacket* pPacket) throw(ProtocolException, Error);

    // get/set socket
    // Socket* getSocket () throw () { return m_pSocket; }
    void setSocket(Socket* pSocket) throw();

    // get/set player ID
    // string getID () const throw () { return m_ID; }
    // void setID (const string & id) throw () { m_ID = id; }

    // get debug string
    virtual string toString() const throw(Error);
};

#endif
