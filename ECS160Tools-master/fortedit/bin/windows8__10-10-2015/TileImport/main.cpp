#include "mainwindow.h"
#include <QApplication>

#include <QColorDialog>

#include "tileimport.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.fillTileList();
    w.show();

    return a.exec();
}
