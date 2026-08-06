//////////////////////////////////////////////////////////////////////////////
// Filename    : Object.h
// Written By  : Elca

//////////////////////////////////////////////////////////////////////////////

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Assert.h"
#include "Exception.h"
#include "Types.h"

//////////////////////////////////////////////////////////////////////////////
// Object Priority
//



//



//////////////////////////////////////////////////////////////////////////////
enum ObjectPriority {
    OBJECT_PRIORITY_WALKING_CREATURE,
    OBJECT_PRIORITY_FLYING_CREATURE,
    OBJECT_PRIORITY_BURROWING_CREATURE,
    OBJECT_PRIORITY_EFFECT,
    OBJECT_PRIORITY_ITEM,
    OBJECT_PRIORITY_PORTAL,
    OBJECT_PRIORITY_OBSTACLE,
    OBJECT_PRIORITY_NONE 
};

class Packet;

//////////////////////////////////////////////////////////////////////////////
// class Object

//////////////////////////////////////////////////////////////////////////////

class Object {
public:
    // Object Class
    
    
    enum ObjectClass {
        OBJECT_CLASS_CREATURE,
        OBJECT_CLASS_ITEM,
        OBJECT_CLASS_OBSTACLE,
        OBJECT_CLASS_EFFECT,
        OBJECT_CLASS_PORTAL
    };

public:
    Object(ObjectID_t objectID = 0) : m_ObjectID(objectID) {}
    virtual ~Object() {}

public:
    // get/set object id
    //
    
    
    
    
    
    
    ObjectID_t getObjectID() const {
        Assert(m_ObjectID != 0);
        return m_ObjectID;
    };
    void setObjectID(ObjectID_t objectID) {
        Assert(objectID != 0);
        m_ObjectID = objectID;
    }

    // get object class(virtual)
    
    
    //
    // *CAUTION*
    
    
    
    virtual ObjectClass getObjectClass() const = 0;

    // get object priority(virtual)
    virtual ObjectPriority getObjectPriority() const = 0;

    // get debug string
    virtual string toString() const = 0;

    virtual Packet* getAddPacket() const {
        return NULL;
    }

protected:
    ObjectID_t m_ObjectID; // Object ID
};

//////////////////////////////////////////////////////////////////////////////
// function object
//////////////////////////////////////////////////////////////////////////////
class isSameObjectID {
public:
    isSameObjectID(ObjectID_t objectID) : m_ObjectID(objectID) {}

    bool operator()(Object* pObject) {
        return pObject->getObjectID() == m_ObjectID;
    }

private:
    ObjectID_t m_ObjectID;
};

#endif
