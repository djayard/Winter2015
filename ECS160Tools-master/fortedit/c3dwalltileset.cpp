#include <QDebug>
#include "c3dwalltileset.h"
#include "cscene.h"
#include "Binary.h"

C3DWallTileset::C3DWallTileset()
{
    // Empty
}

/**
 * Creates a 3D tileset from a .DAT file pertaining to a 3D tileset
 * @param dat path to the .DAT file pertaining to a 3D tileset
 */
C3DWallTileset::C3DWallTileset(const QString& dat)
    : CWallTileset(dat, 12, 24)
{
    // This is ugly... if only the tiles were in proper bit order in the first place ;_;
    // NE NW SE SW N S E W
    DOrientationMap.insert(binary(00000000), typeBare);
    DOrientationMap.insert(binary(00000001), typeW);
    DOrientationMap.insert(binary(00000010), typeE);
    DOrientationMap.insert(binary(00000011), typeEW);
    DOrientationMap.insert(binary(00000100), typeS);
    DOrientationMap.insert(binary(00010101), typeSW_SW);
    DOrientationMap.insert(binary(00000101), typeSW);
    DOrientationMap.insert(binary(00100110), typeSE_SE);
    DOrientationMap.insert(binary(00000110), typeSE);
    DOrientationMap.insert(binary(00110111), typeSE_SW_SEW);
    DOrientationMap.insert(binary(00100111), typeSE_SEW);
    DOrientationMap.insert(binary(00010111), typeSW_SEW);
    DOrientationMap.insert(binary(00000111), typeSEW);
    DOrientationMap.insert(binary(00001000), typeN);
    DOrientationMap.insert(binary(01001001), typeNW_NW);
    DOrientationMap.insert(binary(00001001), typeNW);
    DOrientationMap.insert(binary(10001010), typeNE_NE);
    DOrientationMap.insert(binary(00001010), typeNE);
    DOrientationMap.insert(binary(11001011), typeNE_NW_NEW);
    DOrientationMap.insert(binary(01001011), typeNW_NEW);
    DOrientationMap.insert(binary(10001011), typeNE_NEW);
    DOrientationMap.insert(binary(00001011), typeNEW);
    DOrientationMap.insert(binary(00001100), typeNS);
    DOrientationMap.insert(binary(01011101), typeNW_SW_NSW);
    DOrientationMap.insert(binary(01001101), typeNW_NSW);
    DOrientationMap.insert(binary(00011101), typeSW_NSW);
    DOrientationMap.insert(binary(00001101), typeNSW);
    DOrientationMap.insert(binary(10101110), typeNE_SE_NSE);
    DOrientationMap.insert(binary(00101110), typeSE_NSE);
    DOrientationMap.insert(binary(10001110), typeNE_NSE);
    DOrientationMap.insert(binary(00001110), typeNSE);
    DOrientationMap.insert(binary(11111111), typeNE_NW_SE_SW_NSEW);
    DOrientationMap.insert(binary(01111111), typeNW_SE_SW_NSEW);
    DOrientationMap.insert(binary(11011111), typeNE_NW_SW_NSEW);
    DOrientationMap.insert(binary(01011111), typeNW_SW_NSEW);
    DOrientationMap.insert(binary(11101111), typeNE_NW_SE_NSEW);
    DOrientationMap.insert(binary(01101111), typeNW_SE_NSEW);
    DOrientationMap.insert(binary(11001111), typeNE_NW_NSEW);
    DOrientationMap.insert(binary(01001111), typeNW_NSEW);
    DOrientationMap.insert(binary(10111111), typeNE_SE_SW_NSEW);
    DOrientationMap.insert(binary(00111111), typeSE_SW_NSEW);
    DOrientationMap.insert(binary(10011111), typeNE_SW_NSEW);
    DOrientationMap.insert(binary(00011111), typeSW_NSEW);
    DOrientationMap.insert(binary(10101111), typeNE_SE_NSEW);
    DOrientationMap.insert(binary(00101111), typeSE_NSEW);
    DOrientationMap.insert(binary(10001111), typeNE_NSEW);
    DOrientationMap.insert(binary(00001111), typeNSEW);
}

/**
 * Gets the requested wall type QPixmap from the tileset
 *
 * @param type type of wall, see {@link E3DWallType}
 * @param side the player side for the wall type
 * @return a QPixmap for the wall tile
 */
QPixmap &C3DWallTileset::GetWallTile(const E3DWallType& type, const unsigned& side)
{
    if (side == STile::sideNONE)
    {
        return DTiles[type];
    }

    return DTiles[type + side * 47];
}

/**
 * Given a bitmap of orientation flags, returns a 3D wall type
 *
 * @param bitmap map of orientation flags, see {@link E3DWallType}
 * @return {@link E3DWallType}
 */
C3DWallTileset::E3DWallType C3DWallTileset::GetType(const int& bitmap)
{
    // We only pay attention to the 8 neighbors for certain orientations
    CWallTileset::EWallType Wall2DType = CWallTileset::GetType(bitmap);
    if (Wall2DType == 0)
    {
        return DOrientationMap[bitmap];
    }
    else if (Wall2DType == 1)
    {
        return DOrientationMap[bitmap & binary(00110111)];
    }
    else if (Wall2DType == 2)
    {
        return DOrientationMap[bitmap & binary(01011101)];
    }
    else if (Wall2DType == 3)
    {
        return DOrientationMap[bitmap & binary(00010101)];
    }
    else if (Wall2DType == 4)
    {
        return DOrientationMap[bitmap & binary(11001011)];
    }
    else if (Wall2DType == 6)
    {
        return DOrientationMap[bitmap & binary(01001001)];
    }
    else if (Wall2DType == 8)
    {
        return DOrientationMap[bitmap & binary(10101110)];
    }
    else if (Wall2DType == 9)
    {
        return DOrientationMap[bitmap & binary(00100110)];
    }
    else if (Wall2DType == 12)
    {
        return DOrientationMap[bitmap & binary(10001010)];
    }
    else if (Wall2DType == 5
            || Wall2DType == 7
            || Wall2DType == 10
            || Wall2DType == 11
            || Wall2DType == 13
            || Wall2DType == 14
            || Wall2DType == 15)
    {
        return DOrientationMap[bitmap & 15];
    }
}

