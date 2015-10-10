#ifndef C3DTILESET_H
#define C3DTILESET_H

#include <QString>

#include "ctileset.h"

/**
 * Class representing a 3D terrain tileset. Loads and provides QPixmap for each tile
 */
class C3DTileset : public CTileset
{
public:
    C3DTileset();
    C3DTileset(const QString&);

    enum E3DType
    {
        typeGrass,
        typeShoreN,
        typeShoreNE,
        typeShoreE,
        typeShoreSE,
        typeShoreS,
        typeShoreSW,
        typeShoreW,
        typeShoreNW,
        typeShoreNEC,
        typeShoreSEC,
        typeShoreSWC,
        typeShoreNWC,
        typeShoreNESW,
        typeShoreNWSE,
        typeWater,
        typeHole
    };

    virtual QPixmap& GetTile(const CTileset::EType&);
    void TickFrame();

private:
    QMap<CTileset::EType, E3DType> DOrientationMap;
    unsigned DFrame;
};

#endif // C3DTILESET_H
