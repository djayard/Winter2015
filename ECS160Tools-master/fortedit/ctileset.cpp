#include <QFile>
#include <QTextStream>
#include <QString>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>

#include "ctileset.h"
#include "cparseexception.h"

CTileset::CTileset()
{
    // Nothing...
}

CTileset::CTileset(const QString& dat)
    : DTileWidth(12),
      DTileHeight(12)
{
    LoadPixmaps(dat);
}

CTileset::CTileset(const QString& dat, const unsigned& w, const unsigned& h)
    : DTileWidth(w),
      DTileHeight(h)
{
    LoadPixmaps(dat);
}

/**
 * Partitions the tileset into multiple QPixmaps for later retrieval
 * @param dat path to tileset .dat file
 */
void CTileset::LoadPixmaps(const QString& dat)
{
    QFile Dat(dat);
    if (!Dat.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw CParseException("Couldn't open dat file for reading");
    }

    QString Line;
    QTextStream DatIn(&Dat);

    // Path to tileset PNG
    Line = DatIn.readLine();
    if (Line.size() < 1)
    {
        throw CParseException("Tileset not specified in dat file!");
    }

    QImage tileset;
    tileset.load(Line);
    QPixmap tilesetPixmap = QPixmap::fromImage(tileset);

    // Number of tiles in tileset
    Line = DatIn.readLine();
    DTiles.resize(Line.toUInt());

    // TODO: What if the tiles are not in the order expected?
    for (int i = 0; i < DTiles.size(); ++i) {
        DTiles[i] = tilesetPixmap.copy(0, i * DTileHeight, DTileWidth, DTileHeight);
    }

    Dat.close();

    // Initialize orientation map
    DOrientationMap.insert(0, typeWater);
    DOrientationMap.insert(8, typeShoreNEven);
    DOrientationMap.insert(24, typeShoreNOdd);
    DOrientationMap.insert(12, typeShoreNEEven);
    DOrientationMap.insert(28, typeShoreNEOdd);
    DOrientationMap.insert(4, typeShoreEEven);
    DOrientationMap.insert(20, typeShoreEOdd);
    DOrientationMap.insert(6, typeShoreSEEven);
    DOrientationMap.insert(22, typeShoreSEOdd);
    DOrientationMap.insert(2, typeShoreSEven);
    DOrientationMap.insert(18, typeShoreSOdd);
    DOrientationMap.insert(3, typeShoreSWEven);
    DOrientationMap.insert(19, typeShoreSWOdd);
    DOrientationMap.insert(1, typeShoreWEven);
    DOrientationMap.insert(17, typeShoreWOdd);
    DOrientationMap.insert(9, typeShoreNWEven);
    DOrientationMap.insert(25, typeShoreNWOdd);
}

/**
 * Gets the given type's tile
 * @param type see {@link EType}
 * @return QPixmap for the tile type
 */
QPixmap& CTileset::GetTile(const CTileset::EType& type)
{
    return DTiles[type];
}

/**
 * Given a bitmap, returns the tile type
 * @param map bitmap of orientation flags
 * @return tile type, see {@link EType}
 */
CTileset::EType CTileset::GetType(const int& map)
{
    CTileset::EType type = DOrientationMap[map];

    if ((type == 0 && map == 0) || type != 0)
    {
        return type;
    }

    return CTileset::typeHole;
}
