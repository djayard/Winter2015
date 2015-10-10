#ifndef CTILESET_H
#define CTILESET_H

#include <QString>
#include <QVector>
#include <QPixmap>
#include <QMap>

/**
 * Base class for tileset .dat files. Will load and parse given tileset .dat files
 */
class CTileset
{
public:
    CTileset();
    CTileset(const QString&);
    CTileset(const QString&, const unsigned&, const unsigned&);

    enum EType
    {
        typeGrassEven,
        typeGrassOdd,

        typeShoreNEven,
        typeShoreNOdd,

        typeShoreNEEven,
        typeShoreNEOdd,

        typeShoreEEven,
        typeShoreEOdd,

        typeShoreSEEven,
        typeShoreSEOdd,

        typeShoreSEven,
        typeShoreSOdd,

        typeShoreSWEven,
        typeShoreSWOdd,

        typeShoreWEven,
        typeShoreWOdd,

        typeShoreNWEven,
        typeShoreNWOdd,

        typeShoreNE,
        typeShoreSE,
        typeShoreSW,
        typeShoreNW,
        typeShoreNESW,
        typeShoreNWSE,

        typeWater,

        typeFloorEven,
        typeFloorBlue,
        typeFloorRed,
        typeFloorYellow,

        typeHole
    };

    virtual QPixmap& GetTile(const EType&);
    EType GetType(const int&);

protected:
    QVector<QPixmap> DTiles;
    unsigned DTileWidth;
    unsigned DTileHeight;

private:
    void LoadPixmaps(const QString&);
    QMap<int, EType> DOrientationMap;
};

#endif // CTILESET_H
