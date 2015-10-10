#-------------------------------------------------
#
# Project created by QtCreator 2015-01-16T17:56:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TileImport
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    tileimport.cpp \
    tilecontroller.cpp \
    animator.cpp

HEADERS  += mainwindow.h \
    tileimport.h \
    tilecontroller.h \
    animator.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    data/2DCastleCannon.dat \
    data/2DCastleSelect.dat \
    data/2DTerrain.dat \
    data/2DWallFloor.dat \
    data/3DBurn.dat \
    data/3DCannon.dat \
    data/3DCannonball.dat \
    data/3DCannonPlume.dat \
    data/3DCastle.dat \
    data/3DExplosion.dat \
    data/3DFloor.dat \
    data/3DTerrain.dat \
    data/3DWall.dat \
    data/Bricks.dat \
    data/Digits.dat \
    data/FontKingthingsBlack.dat \
    data/FontKingthingsWhite.dat \
    data/Mortar.dat \
    data/SoundClips.dat \
    data/Target.dat \
    pngs/2DCastleCannon.png \
    pngs/2DCastleSelect.png \
    pngs/2DTerrain.png \
    pngs/2DWalls.png \
    pngs/3DBurn.png \
    pngs/3DCannon.png \
    pngs/3DCannonball.png \
    pngs/3DCannonPlume.png \
    pngs/3DCastles.png \
    pngs/3DExplosions.png \
    pngs/3DFloor.png \
    pngs/3DTerrain.png \
    pngs/3DWallsAll.png \
    pngs/Bricks.png \
    pngs/Digits.png \
    pngs/FontKingthingsBlack.png \
    pngs/FontKingthingsWhite.png \
    pngs/Mortar.png \
    pngs/Target.png
