#include "tilecontroller.h"

#include "tileimport.h"

#include <QPixmap>
#include <QSize>
#include <QMouseEvent>

/*
 * controller class for tiles
 */
TileController::TileController() {
    import = new TileImport();
    animator = new Animator();
    import->importFile();
    animator->setAnimNames(import->getFileName());
}

TileController::~TileController() {
    delete import;
    delete animator;
}

/*
 * gets tile and scales it for display
 * @param index of tile in tile list
 */
QPixmap TileController::displayTile(int tileIndex) {
    //get specific tile
    QPixmap tile = import->getTileAt(tileIndex);

    //expand the tile and return
    return tile.scaled(QSize(400, 400), Qt::KeepAspectRatio);
}

/*
 * sets path of tile set to read in
 * @param tileset to read in
 */
void TileController::setInTilesetPath(QString tilesetPath) {
    import->setInTilesetPath(tilesetPath);
}

/*
 * sets path of tileset to save
 * @param tileset path to save
 */
void TileController::setOutTilesetPath(QString tilesetPath) {
    import->setOutTilesetPath(tilesetPath);
}

/*
 * reloads tileset
 */
void TileController::reloadTileset() {
    import->importFile();
    animator->setAnimNames(import->getFileName());
}

/*
 * saves tileset
 */
void TileController::saveTileset() {
    import->saveFile();
}

/*
 * saves the tile in dispaly with painted tiles
 * sclaes back down tile to normal size of pngs
 * @param tile pixmap of the tile that's painted
 * @param tileIndex index of tile in tileset list
 */
void TileController::saveDisplayTile(QPixmap tile, int tileIndex) {
    //rescale the tile and put it in the list
    QPixmap scaledTile = import->getTileAt(tileIndex);
    scaledTile = tile.scaled(scaledTile.size(), Qt::KeepAspectRatio);
    import->saveTile(scaledTile, tileIndex);
}

/*
 * sets anumation
 * @param animation name
 */
void TileController::setAnimNum(QString animName) {
    animator->setAnimNum(animName);
}
