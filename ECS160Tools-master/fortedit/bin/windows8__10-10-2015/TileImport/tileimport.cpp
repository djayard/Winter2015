#include "tileimport.h"

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QPixmap>


#include <QCoreApplication>
#include <QDir>
#include <QPainter>

#define DEFAULT_TILESET "/data/2DTerrain.dat"
#define NEWLINE "/n"

/*
 * class to read in tilesets
 */

TileImport::TileImport() {

}

/*
 * imports tile set from dat
 */
bool TileImport::importFile() {
    bool failed = false;

    /*
     * determines tileset to read in
     * if member inTilesetPath is not null, read from there
     * otherwise read in from default tileset
     */
    QString filePath = !(this->inTilesetPath.isEmpty()) ? this->inTilesetPath : QDir::currentPath().append(DEFAULT_TILESET);

    QFile file(filePath);
    if(!file.open(QIODevice::ReadWrite)) {
        return failed;
    }

    QTextStream in(&file);

    //the first line of the dat file is the png file path
    QString imageFile = in.readLine();
    if(imageFile.isEmpty() || imageFile.isNull()) {
        return failed;
    }

    //the second line is the number of tiles in png file
    QString numTiles = in.readLine();
    if(numTiles.isEmpty() || numTiles.isNull()) {
        return failed;
    }

    //call to get the tiles
    if(!importTiles(imageFile, numTiles.toInt())) {
        return failed;
    }

    //the rest are the names of the tiles
    QList<QString> tmpTileNames;
    while(!in.atEnd()) {
        tmpTileNames.append(in.readLine());
    }

    setTileNames(tmpTileNames);
    tmpTileNames.clear();
    //remove "pngs/" from string imageFile
    this->fileName = imageFile.remove(0,5);

    return true;
}

/*
 * Imports tiles from png
 * @param filename name of png file
 * @param numTiles number of tiles stored in the png
 */
bool TileImport::importTiles(QString filename, int numTiles) {
    //read in the entire png
    QPixmap* pixmap = new QPixmap(QDir::currentPath().append("/" + filename));
    if(pixmap->isNull()) {
        return false;
    }

    //break apart png into tile squares
    QList<QPixmap> tiles;
    int height = pixmap->height() / numTiles;
    int width = pixmap->width();
    int x = 0;
    int y = 0;
    for(int i = 0; i < numTiles; i++) {
        QPixmap* tile = new QPixmap(pixmap->copy(x, y, width, height));
        tiles.append(*tile);
        y += height;
        delete tile;
    }

    setTiles(tiles);
    delete pixmap;

    return true;
}

/*
 * sets tile names in member QList
 */
void TileImport::setTileNames(const QList<QString> tmpTileNames) {
    this->tileNames.clear();
    this->tileNames = tmpTileNames;
}

/*
 * sets tile pixmaps in member QList
 */
void TileImport::setTiles(const QList<QPixmap> tiles) {
    this->tiles.clear();
    this->tiles = tiles;
}

/*
 * saves dat file (calls function to save png)
 */
bool TileImport::saveFile() {
    bool failed = false;

    QString filepath = this->outTilesetPath;
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly)) {
        return failed;
    }

    QTextStream out(&file);

    //create the png filename for first line of .dat file
    QString filename = this->outTilesetPath.remove(0, QDir::currentPath().length() + 5);

    //create filepath for the png
    QString pngFilePath = QDir::currentPath().remove(QDir::currentPath().length(), 3).append("/pngs" + filename.replace(filename.length() - 3, 3, "png"));

    //output the png filename [first line]
    out << filename << NEWLINE;

    //output number of tiles [second line]
    out << this->tiles.size() << NEWLINE;

    //output the tile names
    for(int i = 0; i < this->tileNames.size(); i++) {
        out << this->tileNames[i] << NEWLINE;
    }

    saveTiles(pngFilePath, this->tiles.size());

    return true;
}

/*
 * saves png
 */
bool TileImport::saveTiles(QString filepath, int numTiles) {
    QFile file(filepath);
    file.open(QIODevice::WriteOnly);

    //rebuild png from tile squares
    int tileHeight = this->tiles[0].height();
    int height = tileHeight * numTiles;
    int width = this->tiles[0].width();
    int x = 0;
    int y = 0;
    QPixmap* tileset = new QPixmap(width, height);

    QPainter painter(tileset);
    for(int i = 0; i < numTiles; i++) {
        painter.drawPixmap(x, y, this->tiles.at(i));
        y += tileHeight;
    }
    painter.end();

    //save the full tileset png
    tileset->save(&file);

    delete tileset;

    return true;
}
