 

#ifndef __ZONELOADER_H__
#define __ZONELOADER_H__

#include <string>
#include <cstdint>
#include <vector>
#include <map>


typedef struct Zone Zone;
typedef struct ZoneHeader ZoneHeader;
typedef struct Sector Sector;
typedef struct ImageObject ImageObject;

#ifdef __cplusplus
extern "C" {
#endif


void zone_free(Zone* zone);

#ifdef __cplusplus
}
#endif


struct ZoneInfo {
    std::string version;
    uint16_t zoneID;
    uint16_t zoneGroupID;
    std::string zoneName;
    uint8_t zoneType;
    uint8_t zoneLevel;
    std::string description;
};


struct SectorData {
    uint16_t spriteID;
    uint8_t  property;
    uint8_t  light;

    SectorData() : spriteID(0xFFFF), property(0), light(0) {}

    
    void InitFrom(const Sector* s);
};


struct ImageObjectData {
    uint8_t type;                  // ImageObjectType
    uint32_t id;                   // Instance Object ID
    uint16_t sectorX;              // Sector X
    uint16_t sectorY;              // Sector Y
    uint32_t imageObjectID;        // Image class ID
    uint16_t spriteID;             // Sprite ID
    int32_t pixelX;                // Pixel X position
    int32_t pixelY;                // Pixel Y position
    uint16_t viewpoint;            // Viewpoint
    uint8_t isAnimation;           // Animation flag
    uint8_t transFlags;            // Transparency flags

    // Position list - sectors this object occupies
    std::vector<std::pair<uint16_t, uint16_t>> positions;

    ImageObjectData() : type(0), id(0), sectorX(0), sectorY(0),
                       imageObjectID(0), spriteID(0), pixelX(0), pixelY(0),
                       viewpoint(0), isAnimation(0), transFlags(0) {}

    
    void InitFrom(const ImageObject* obj);
};

 
class ZoneLoader {
public:
    ZoneLoader();
    ~ZoneLoader();

    
    ZoneLoader(const ZoneLoader&) = delete;
    ZoneLoader& operator=(const ZoneLoader&) = delete;

     
    bool LoadFromFile(const char* filename);

     
    const ZoneInfo& GetInfo() const { return m_info; }

     
    int GetWidth() const;
    int GetHeight() const;

     
    const SectorData* GetSector(int x, int y) const;

     
    const std::vector<SectorData>& GetAllSectors() const { return m_sectors; }

     
    int GetImageObjectCount() const { return m_imageObjects.size(); }

     
    const ImageObjectData* GetImageObject(int index) const;

     
    const std::vector<ImageObjectData>& GetAllImageObjects() const { return m_imageObjects; }

     
    int GetSpriteID(int sectorX, int sectorY) const;

     
    void Release();

private:
     
    void ConvertZoneInfo(const ZoneHeader* header);

private:
    class Impl;  
    Impl* m_impl;

    ZoneInfo m_info;
    std::vector<SectorData> m_sectors;
    std::vector<ImageObjectData> m_imageObjects;
};

 
class ZoneLoaderDataProvider {
public:
    explicit ZoneLoaderDataProvider(ZoneLoader* loader) : m_loader(loader) {}

    int GetSpriteID(int sectorX, int sectorY) const {
        return m_loader ? m_loader->GetSpriteID(sectorX, sectorY) : -1;
    }

    int GetWidth() const { return m_loader ? m_loader->GetWidth() : 0; }
    int GetHeight() const { return m_loader ? m_loader->GetHeight() : 0; }

private:
    ZoneLoader* m_loader;
};

#endif // __ZONELOADER_H__
