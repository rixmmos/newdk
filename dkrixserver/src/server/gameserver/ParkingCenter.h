//////////////////////////////////////////////////////////////////////////////
// Filename    : ParkingCenter.h
// Written By  : Reiot
// Description :
//////////////////////////////////////////////////////////////////////////////

#ifndef __PARKING_CENTER_H__
#define __PARKING_CENTER_H__

#include <list>

#include <unordered_map>

#include "Exception.h"
#include "Mutex.h"
#include "Types.h"
#include "Zone.h"
#include "item/Motorcycle.h"

//////////////////////////////////////////////////////////////////////////////
// class MotorcycleBox
//////////////////////////////////////////////////////////////////////////////

class MotorcycleBox {
public:
    MotorcycleBox(Motorcycle* pMotorcycle, Zone* pZone, ZoneCoord_t X, ZoneCoord_t Y);
    virtual ~MotorcycleBox();

public:
    Motorcycle* getMotorcycle() {
        return m_pMotorcycle;
    }
    void setMotorcycle(Motorcycle* pMotorcycle) {
        m_pMotorcycle = pMotorcycle;
    }

    Zone* getZone() const {
        return m_pZone;
    }
    void setZone(Zone* pZone) {
        m_pZone = pZone;
    }

    ZoneCoord_t getX() const {
        return m_X;
    }
    void setX(ZoneCoord_t X) {
        m_X = X;
    }

    ZoneCoord_t getY() const {
        return m_Y;
    }
    void setY(ZoneCoord_t Y) {
        m_Y = Y;
    }

    ItemID_t getItemID() const {
        return m_pMotorcycle->getItemID();
    }

    
    bool isTransport() const {
        return m_bTransport;
    }
    void setTransport(bool bTransport = true) {
        m_bTransport = bTransport;
    }

private:
    
    Motorcycle* m_pMotorcycle;

    
    Zone* m_pZone;
    ZoneCoord_t m_X;
    ZoneCoord_t m_Y;

    
    bool m_bTransport;
};

//////////////////////////////////////////////////////////////////////////////
// class ParkingCenter;
//////////////////////////////////////////////////////////////////////////////

class ParkingCenter {
public:
    ParkingCenter();
    virtual ~ParkingCenter();

public:
    void addMotorcycleBox(MotorcycleBox* pMotorcycleBox);

    
    void deleteMotorcycleBox(ItemID_t keyTargetID);

    
    bool hasMotorcycleBox(ItemID_t keyTargetID);

    
    MotorcycleBox* getMotorcycleBox(ItemID_t keyTargetID) const;

    
    void heartbeat();

private:
    
    unordered_map<ItemID_t, MotorcycleBox*> m_Motorcycles;
    list<MotorcycleBox*> m_RemoveMotorcycles;

    mutable Mutex m_Mutex;
    mutable Mutex m_MutexRemove;
};

// global variable declaration
extern ParkingCenter* g_pParkingCenter;

#endif
