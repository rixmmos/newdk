 

#ifndef __MZONE_TILEPROVIDER_H__
#define __MZONE_TILEPROVIDER_H__

#include "Client/MZone.h"
#include "Client/TileRenderer.h"  // For ITileDataProvider interface

 
class MZoneTileProvider : public ITileDataProvider {
public:
    MZoneTileProvider();
    ~MZoneTileProvider();

    
    bool LoadFromFile(const char* filename);

    
    void Release();

    // ITileDataProvider interface implementation
    virtual int GetSpriteID(int sectorX, int sectorY) override
    {
        if (!IsInBounds(sectorX, sectorY))
            return -1;  // Out of bounds = empty

        const MSector& sector = m_pZone->GetSector(sectorX, sectorY);
        TYPE_SPRITEID spriteID = sector.GetSpriteID();

        // spriteID=0 or SPRITEID_NULL means no tile
        if (spriteID == 0 || spriteID == SPRITEID_NULL)
            return -1;

        return spriteID;
    }

    virtual int GetWidth() const override
    {
        if (m_pZone == nullptr)
            return 0;
        return m_pZone->GetWidth();
    }

    virtual int GetHeight() const override
    {
        if (m_pZone == nullptr)
            return 0;
        return m_pZone->GetHeight();
    }

    
    MZone* GetZone() { return m_pZone; }
    const MZone* GetZone() const { return m_pZone; }

private:
    bool IsInBounds(int x, int y) const
    {
        if (m_pZone == nullptr)
            return false;
        return (x >= 0 && y >= 0 && x < m_pZone->GetWidth() && y < m_pZone->GetHeight());
    }

private:
    MZone* m_pZone;
    
    std::ifstream* m_pFile;
};

#endif // __MZONE_TILEPROVIDER_H__
