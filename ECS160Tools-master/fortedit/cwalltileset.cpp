#include "cwalltileset.h"
#include "cscene.h"

CWallTileset::CWallTileset()
{
    initOrientationMap();
}

CWallTileset::CWallTileset(const QString& dat)
    : CTileset(dat)
{
    initOrientationMap();
}

CWallTileset::CWallTileset(const QString& dat, const unsigned& w, const unsigned& h)
    : CTileset(dat, w, h)
{
    initOrientationMap();
}

/**
 * Unused
 */
QPixmap& CWallTileset::GetTile(const CTileset::EType& type)
{
    (void) type;
    return DTiles[0];
}

/**
 * Gets the tile for the given wall chunk
 * @param type type of wall chunk
 * @param side the territory the wall lives under
 * @return QPixmap for the wall chunk
 */
QPixmap& CWallTileset::GetWallTile(const EWallType& type, const unsigned& side)
{
    if (side == STile::sideNONE)
    {
        return DTiles[type];
    }

    return DTiles[type + (side + 3) * 16];
}

/**
 * Given a bitmap of orientation flags, returns the corresponding wall chunk type
 * @param bitmap bitmap of orientation flags
 * @return wall chunk type, see {@link EWallType}
 */
CWallTileset::EWallType CWallTileset::GetType(const int& bitmap)
{
    return DOrientationMap[bitmap & 15];
}

/**
 * Initializes the bitmap -> {@link EWallType} map
 */
void CWallTileset::initOrientationMap()
{
    // N S E W
    DOrientationMap.insert(0 , typeBare);
    DOrientationMap.insert(1 , typeW);
    DOrientationMap.insert(2 , typeE);
    DOrientationMap.insert(3 , typeEW);
    DOrientationMap.insert(4 , typeS);
    DOrientationMap.insert(5 , typeSW);
    DOrientationMap.insert(6 , typeSE);
    DOrientationMap.insert(7 , typeSEW);
    DOrientationMap.insert(8 , typeN);
    DOrientationMap.insert(9 , typeNW);
    DOrientationMap.insert(10, typeNE);
    DOrientationMap.insert(11, typeNEW);
    DOrientationMap.insert(12, typeNS);
    DOrientationMap.insert(13, typeNSW);
    DOrientationMap.insert(14, typeNSE);
    DOrientationMap.insert(15, typeNSEW);
}
