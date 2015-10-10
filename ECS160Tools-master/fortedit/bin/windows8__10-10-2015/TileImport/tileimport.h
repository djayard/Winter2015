#ifndef TILEIMPORT_H
#define TILEIMPORT_H

#include <QString>
#include <QPixmap>

/*
 * class to import tilesets
 */
class TileImport {
    public:
        TileImport();
        bool importFile();
        bool importTiles(QString filename, int numTiles);

        bool saveFile();
        bool saveTiles(QString filename, int numTiles);

        QPixmap getTileAt(int index) { return this->tiles.at(index); }
        QList<QPixmap> getTiles() const { return this->tiles; }
        QList<QString> getTileNames() const { return this->tileNames; }
        QString getFileName() const { return this->fileName; }

        void setInTilesetPath(QString tilesetPath) { this->inTilesetPath = tilesetPath; }
        void setOutTilesetPath(QString tilesetPath) {this->outTilesetPath = tilesetPath; }

        void saveTile(QPixmap tile, int tileIndex) { this->tiles[tileIndex].swap(tile); }

    protected:
        QList<QPixmap> tiles;
        QList<QString> tileNames;
        QString fileName;

    private:
        QString inTilesetPath;
        QString outTilesetPath;
        void setTiles(const QList<QPixmap> tiles);
        void setTileNames(const QList<QString> tmpTileNames);
};

#endif // TILEIMPORT_H
