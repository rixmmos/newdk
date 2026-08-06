//////////////////////////////////////////////////////////////////////////////
// Filename    : Obstacle.h
// Written by  : reiot@ewestsoft.com
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include "Object.h"

//////////////////////////////////////////////////////////////////////////////
// Obstacle Type
//


//////////////////////////////////////////////////////////////////////////////
typedef BYTE ObstacleType_t;
const uint szObstacleType = sizeof(ObstacleType_t);

//////////////////////////////////////////////////////////////////////////////
// class Obstacle;
//



//////////////////////////////////////////////////////////////////////////////

class Obstacle : public Object {
public:
    enum ObstacleClass { OBSTACLE_CLASS_DOOR, OBSTACLE_CLASS_SWITCH, OBSTACLE_CLASS_TRAP };

public:
    Obstacle(ObjectID_t objectID) : Object(objectID) {}
    virtual ~Obstacle() {}

    // methods from Object
public:
    virtual ObjectClass getObjectClass() const {
        return OBJECT_CLASS_OBSTACLE;
    }
    virtual ObjectPriority getObjectPriority() const {
        return OBJECT_PRIORITY_OBSTACLE;
    }
    virtual string toString() const = 0;

    // own methods
public:
    virtual ObstacleClass getObstacleClass() const = 0;
    virtual ObstacleType_t getObstacleType() const = 0;
};

#endif
