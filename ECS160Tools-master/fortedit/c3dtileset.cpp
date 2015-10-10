#include "c3dtileset.h"

C3DTileset::C3DTileset()
    : DFrame(0)
{
    // Nothing...
}

/**
 * Creates a 3D tileset from a .DAT file pertaining to a 3D tileset
 * @param dat path to the .DAT file pertaining to a 3D tileset
 */
C3DTileset::C3DTileset(const QString& dat)
    : CTileset(dat),
      DFrame(0)
{
    // Initialize orientation map
    DOrientationMap.insert(CTileset::typeGrassEven, typeGrass);
    DOrientationMap.insert(CTileset::typeGrassOdd, typeGrass);
    DOrientationMap.insert(CTileset::typeShoreNEven, typeShoreN);
    DOrientationMap.insert(CTileset::typeShoreNOdd, typeShoreN);
    DOrientationMap.insert(CTileset::typeShoreNEEven, typeShoreNE);
    DOrientationMap.insert(CTileset::typeShoreNEOdd, typeShoreNE);
    DOrientationMap.insert(CTileset::typeShoreEEven, typeShoreE);
    DOrientationMap.insert(CTileset::typeShoreEOdd, typeShoreE);
    DOrientationMap.insert(CTileset::typeShoreSEEven, typeShoreSE);
    DOrientationMap.insert(CTileset::typeShoreSEOdd, typeShoreSE);
    DOrientationMap.insert(CTileset::typeShoreSEven, typeShoreS);
    DOrientationMap.insert(CTileset::typeShoreSOdd, typeShoreS);
    DOrientationMap.insert(CTileset::typeShoreSWEven, typeShoreSW);
    DOrientationMap.insert(CTileset::typeShoreSWOdd, typeShoreSW);
    DOrientationMap.insert(CTileset::typeShoreWEven, typeShoreW);
    DOrientationMap.insert(CTileset::typeShoreWOdd, typeShoreW);
    DOrientationMap.insert(CTileset::typeShoreNWEven, typeShoreNW);
    DOrientationMap.insert(CTileset::typeShoreNWOdd, typeShoreNW);
    DOrientationMap.insert(CTileset::typeShoreNE, typeShoreNEC);
    DOrientationMap.insert(CTileset::typeShoreSE, typeShoreSEC);
    DOrientationMap.insert(CTileset::typeShoreSW, typeShoreSWC);
    DOrientationMap.insert(CTileset::typeShoreNW, typeShoreNWC);
    DOrientationMap.insert(CTileset::typeShoreNESW, typeShoreNESW);
    DOrientationMap.insert(CTileset::typeShoreNWSE, typeShoreNWSE);
    DOrientationMap.insert(CTileset::typeWater, typeWater);
    DOrientationMap.insert(CTileset::typeHole, typeHole);
}

/**
 * Gets the requested tile from the tileset
 * @param type the type of tile to get
 * @return a QPixmap of the requested tile
 */
QPixmap& C3DTileset::GetTile(const CTileset::EType& type)
{
    if (type == CTileset::typeHole)
    {
        // Hole is not animated...
        return DTiles[DOrientationMap[type] * 36];
    }

    return DTiles[DOrientationMap[type] * 36 + DFrame];
}

/**
 * Ticks the current animation frame for the tileset.
 * Loops through frames 0, 1, and 2
 */
void C3DTileset::TickFrame()
{
    ++DFrame;

    if (DFrame > 3)
    {
        DFrame = 0;
    }
}
