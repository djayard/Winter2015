#ifndef TILECONTROLLER_H
#define TILECONTROLLER_H

#include "tileimport.h"
#include "animator.h"
#include <QList>
#include <QString>
#include <QPixmap>
#include <QMouseEvent>
#include <QPoint>

/*
 * controller class for tiles
 */
class TileController
{
    public:
        TileController();
        ~TileController();

        QList<QString> getTileNames() const { return import->getTileNames(); }
        QList<QString> getAnimNames() const { return animator->getAnimNames(); }
        QList<int> getAnimNum() const { return animator->getAnimNum(); }
        QPixmap displayTile(int tileIndex);

        void setInTilesetPath(QString tilesetPath);
        void setOutTilesetPath(QString tilesetPath);
        void reloadTileset();
        void saveTileset();
        void saveDisplayTile(QPixmap tile, int tileIndex);
        void setAnimNum(QString animName);

        void selectColor();
        void pressed(QPoint point);
        void unpressed(QPoint point);
        QPixmap paint(QPoint point);
        QPixmap getPaintedTile();

    private:
        TileImport* import;
        Animator* animator;
};

#endif // TILECONTROLLER_H
