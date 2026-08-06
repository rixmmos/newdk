//--------------------------------------------------------------------------------
//
// Filename    : UpdateManager.h
// Written By  : Reiot
//
//--------------------------------------------------------------------------------

#ifndef __UPDATE_MANAGER_H__
#define __UPDATE_MANAGER_H__

// include files
#include <list>

#include "Assert.h"
#include "Resource.h"
#include "ResourceManager.h"
#include "Update.h"

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
const uint maxUpdates = 1024;

//--------------------------------------------------------------------------------
//
// class UpdateManager
//

//

//
// load/save
// read(SocketInputStream&)/write(SocketOutputStream&)
// read(Socket*)/write(Socket*)
//

//
//--------------------------------------------------------------------------------

class UpdateManager {
public:
    // constructor
    UpdateManager();

    // destructor
    ~UpdateManager();


public:
    // load from update file
    void load(const string& filename);

    // save to update file
    void save(const string& filename) const;

    // read from socket input stream
    void read(SocketInputStream& iStream);

    // read from socket
    void read(Socket* pSocket);

    // write to socket output stream
    void write(SocketOutputStream& oStream) const;

    // write to socket
    void write(Socket* pSocket) const;

    // get size
    uint getSize() const;

    // get max size
    static uint getMaxSize() {
        return Update::getMaxSize() * maxUpdates;
    }


public:
    // list methods
    void push_back(Update* pUpdate) {
        Assert(pUpdate != NULL);
        m_Updates.push_back(pUpdate);
    }
    void pop_front() {
        Assert(!m_Updates.empty());
        m_Updates.pop_front();
    }
    Update* front() const {
        Assert(!m_Updates.empty());
        return m_Updates.front();
    }
    bool empty() const {
        return m_Updates.empty();
    }

    // get resource manager
    ResourceManager* getResourceManager();

    // get debug string
    string toString() const;


private:
    // list of Update
    list<Update*> m_Updates;

    ResourceManager* m_pResourceManager;
};

#endif
