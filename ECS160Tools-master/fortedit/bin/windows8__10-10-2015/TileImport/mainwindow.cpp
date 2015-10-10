#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "tileimport.h"
#include "tilecontroller.h"

#include <QLabel>
#include <QString>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QColorDialog>
#include <QCoreApplication>
#include <QTime>
#include <QDebug>

#define STAMP_SIZE 50 //stamp is always a square

//Sets up the UI and instantiates the TileController.
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    draw = false;
    ui->setupUi(this);
    tileController = new TileController();
}

MainWindow::~MainWindow() {
    delete ui;
    delete tileController;
}

/*
 * displays tile and animation names
 */
void MainWindow::fillTileList() {
    ui->listWidget->clear();
    ui->animationList->clear();
    ui->listWidget->addItems(tileController->getTileNames());
    ui->animationList->addItems(tileController->getAnimNames());
}

/*
 * displays large tile selected from list
 * @param index of tile in tile list
 */
void MainWindow::on_listWidget_clicked(const QModelIndex &index) {
    tileIndex = index.row();
    tile = tileController->displayTile(index.row());
    ui->label->setPixmap(tile);
}

/*
 * Animaton
 */
void MainWindow::on_animateButton_clicked() {
    if ("[None]" != ui->animationList->item(0)->text()) {
        if (ui->animateButton->isChecked()) {
            ui->listWidget->clear();
            ui->listWidget->addItems(tileController->getTileNames());
            ui->animateButton->setText("Edit");
            ui->listWidget->setDisabled(true);
            ui->menuBar->setDisabled(true);
            int scan = 0;
            QList<int> tmpAnim;
            while(ui->animateButton->isChecked()) {
                tmpAnim = tileController->getAnimNum();
                if (tmpAnim.count() <= scan) { scan = scan - tmpAnim.count(); }
                ui->label->setPixmap(tileController->displayTile(tmpAnim[scan]));
                scan++;
                //Set FPS (Don't freeze GUI!) 33ms ~ 30 fps
                intDelay = (2078/ui->horizontalSlider->value()) - 5;
                QTime delay = QTime::currentTime().addMSecs(intDelay);
                while (QTime::currentTime() < delay) {QCoreApplication::processEvents();}
            }
        }
        else {
            ui->animateButton->setText("Animate");
            ui->listWidget->setDisabled(false);
            ui->menuBar->setDisabled(false);
            ui->label->clear();
            ui->label->setText("Select a Tile");
        }
    }
    else
        ui->animateButton->toggle();
}

/*
 * opens file manager to open tileset
 */
void MainWindow::on_action_Open_Tileset_triggered() {
    //Path of file to open
    const QString tilesetPath = QFileDialog::getOpenFileName(0,
                                                          "Choose tileset [tileset.dat]",
                                                          QDir::currentPath().append("/data"),
                                                          "Rampart Tilesets (*.dat)");

    //open the file and refresh the data
    if (!tilesetPath.isEmpty()) {
       tileController->setInTilesetPath(tilesetPath);
       tileController->reloadTileset();
       fillTileList();
       ui->label->clear();
    }
}

/*
 * opens file manager to save tileset
 */
void MainWindow::on_actionSave_triggered() {
    //path of file to save
    QString tilesetPath = QFileDialog::getSaveFileName(0,
                                                          "Save tileset [tileset.dat",
                                                          QDir::currentPath().append("/data"),
                                                          "Rampart Map Files (*.dat)");

    //save file
    if(!tilesetPath.isEmpty()) {
        tileController->setOutTilesetPath(tilesetPath);
        tileController->saveTileset();
    }
}

/*
 * opens color dialog to select brush color
 */
void MainWindow::on_pushButton_clicked() {
    QColor color = QColorDialog::getColor();
    if(color.isValid()) {
        this->color = color;
    }
}

/*
 * gets position from mouse event to start paiting
 */
void MainWindow::mousePressEvent(QMouseEvent* mouseEvent) {
    //Get the posiont of the tile in the main window to track brush strokes
    QRect tilePos = ui->label->frameGeometry();
    int tilePosLeft = tilePos.left();
    int tilePosRight = tilePos.right();
    int tilePosTop = tilePos.top();
    int tilePosBottom = tilePos.bottom();

    //Get the position of the mouse
    int mousePosX = mouseEvent->pos().x();
    int mousePosY = mouseEvent->pos().y();

    //make sure the mouse is over the tile
    bool boundedByX = (tilePosLeft <= mousePosX) && (tilePosRight >= mousePosX);
    bool boundedByY = (tilePosTop <= mousePosY) && (tilePosBottom >= mousePosY);
    if(boundedByX && boundedByY) {
        startingPoint = mouseEvent->pos();
        draw = true;
    }
}

/*
 * saves paint strokes to tile
 */
void MainWindow::mouseReleaseEvent(QMouseEvent* mouseEvent) {
    draw = false;
    tileController->saveDisplayTile(tile, tileIndex);
    ui->label->setPixmap(tileController->displayTile(tileIndex));
}

/*
 * adds paint strokes to tile while brushing
 */
void MainWindow::mouseMoveEvent(QMouseEvent* mouseEvent) {
    if(draw) {
        QBrush brush;
        brush.setColor(Qt::white);

        QPainter painter(&tile);

        //creating a "stamp"
        QPixmap stamp(33, 33);
        stamp.fill(this->color);

        //paint on the stamp subtracting 75 to try and get the stamp to line up with cursor
        painter.drawPixmap(mouseEvent->pos().x() - 75, mouseEvent->pos().y() - 75, stamp);

        painter.end();

        //update the view
        tileController->saveDisplayTile(tile, tileIndex);
        ui->label->setPixmap(tile);
    }
}

/*
 * unused
 */
void MainWindow::on_horizontalSlider_valueChanged(int value)
{
    //
}

/*
 * select animation list
 */
void MainWindow::on_animationList_itemClicked(QListWidgetItem *item)
{
    if ("[None]" != item->text())
        tileController->setAnimNum(item->text());
}
