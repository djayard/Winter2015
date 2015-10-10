#ifndef CWALLTILESET_H
#define CWALLTILESET_H

#include <QString>
#include <QMap>

#include "ctileset.h"

/**
 * Specialized tileset class for wall chunks
 */
class CWallTileset : public CTileset
{
public:
    CWallTileset();
    CWallTileset(const QString&);
    CWallTileset(const QString&, const unsigned&, const unsigned&);

    const static int NORTH = 1 << 3;
    const static int SOUTH = 1 << 2;
    const static int EAST  = 1 << 1;
    const static int WEST  = 1;

    enum EWallType
    {
        typeNSEW,
        typeSEW,
        typeNSW,
        typeSW,
        typeNEW,
        typeEW,
        typeNW,
        typeW,
        typeNSE,
        typeSE,
        typeNS,
        typeS,
        typeNE,
        typeE,
        typeN,
        typeBare
    };

    QPixmap& GetTile(const CTileset::EType&);
    QPixmap& GetWallTile(const EWallType&, const unsigned&);
    EWallType GetType(const int&);

private:
    void initOrientationMap();
    QMap<int, EWallType> DOrientationMap;
};

#endif // CWALLTILESET_H
