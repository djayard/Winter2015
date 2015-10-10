#ifndef C3DWALLTILESET_H
#define C3DWALLTILESET_H

#include "cwalltileset.h"

/**
 * Class representing a 3D wall tileset. Loads and provides QPixmap for each tile
 */
class C3DWallTileset : public CWallTileset
{
public:
    C3DWallTileset();
    C3DWallTileset(const QString&);

    const static int NORTH_EAST = 1 << 7;
    const static int NORTH_WEST = 1 << 6;
    const static int SOUTH_EAST = 1 << 5;
    const static int SOUTH_WEST = 1 << 4;

    enum E3DWallType
    {
        typeNE_NW_SE_SW_NSEW,
        typeNW_SE_SW_NSEW,
        typeNE_NW_SW_NSEW,
        typeNW_SW_NSEW,
        typeNE_NW_SE_NSEW,
        typeNW_SE_NSEW,
        typeNE_NW_NSEW,
        typeNW_NSEW,
        typeNE_SE_SW_NSEW,
        typeSE_SW_NSEW,
        typeNE_SW_NSEW,
        typeSW_NSEW,
        typeNE_SE_NSEW,
        typeSE_NSEW,
        typeNE_NSEW,
        typeNSEW,

        typeSE_SW_SEW,
        typeSW_SEW,
        typeSE_SEW,
        typeSEW,

        typeNW_SW_NSW,
        typeNW_NSW,
        typeSW_NSW,
        typeNSW,

        typeSW_SW,
        typeSW,

        typeNE_NW_NEW,
        typeNW_NEW,
        typeNE_NEW,
        typeNEW,

        typeEW,

        typeNW_NW,
        typeNW,

        typeW,

        typeNE_SE_NSE, // 8
        typeSE_NSE,
        typeNE_NSE,
        typeNSE,

        typeSE_SE,
        typeSE,

        typeNS,
        typeS,

        typeNE_NE,
        typeNE,

        typeE,
        typeN,
        typeBare
    };

    QPixmap& GetWallTile(const E3DWallType&, const unsigned&);
    E3DWallType GetType(const int&);

private:
    QMap<int, E3DWallType> DOrientationMap;
};

#endif // C3DWALLTILESET_H
