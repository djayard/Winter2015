#ifndef CSCENE_H
#define CSCENE_H

#include <QString>
#include <QVector>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QTimer>
#include <QRectF>
#include <QRect>
#include <QPainter>
#include <QKeyEvent>
#include <QBrush>
#include <QPen>

#include "ctileset.h"
#include "c3dtileset.h"
#include "cwalltileset.h"
#include "c3dwalltileset.h"
#include "csoundhandler.h"

struct SMapObj
{
    enum EType
    {
        typeCastle,
        typeShip,
        typeSiegeWeapon,
        typeWall
    };

    SMapObj() : DRect(0, 0, 1, 1), DType(typeCastle) {}
    SMapObj(const QRect& rect, const EType& type)
        : DRect(rect), DType(type) {}

    QRect DRect;
    EType DType;
};

struct STile
{
    const static int EVEN  = 1 << 4;
    const static int NORTH = 1 << 3;
    const static int EAST  = 1 << 2;
    const static int SOUTH = 1 << 1;
    const static int WEST  = 1;

    enum ESide
    {
        sideBLUE,
        sideRED,
        sideYELLOW,
        sideNONE
    };

    enum EType
    {
        typeAccessible,
        typeInaccessible
    };

    unsigned DX, DY;
    ESide DSide;
    EType DType;
    CTileset::EType DOrientation;
};

/**
 * Graphical object to represent the map. Contains and handles events for objects, terrain and territory
 */
class CScene : public QGraphicsObject
{
    Q_OBJECT
public:
    CScene();
    CScene(const QString&, QGraphicsScene* renderscene, CSoundHandler* soundHandler);

    enum EDisplayMode
    {
        displayMode2D,
        displayMode3D,
        displayModeTrue3D
    };

    QString GetTitle() const;
    void SetTitle(const QString&);

    unsigned GetWidth() const;
    void SetWidth(const unsigned&);

    unsigned GetHeight() const;
    void SetHeight(const unsigned&);

    void SetObjectVisibility(const bool&);
    void SetTerritoryVisiblity(const bool&);

    void PlaceTerrain(const unsigned&, const unsigned&);
    void PlaceInaccessibleTerrain(const unsigned&, const unsigned&);

    void PlaceObject(const unsigned&, const unsigned&, const SMapObj::EType&);
    void SelectObject(const unsigned&, const unsigned&);
    SMapObj* GetSelectedObject() const;
    void MoveSelectedObject(const unsigned&, const unsigned&);

    EDisplayMode GetDisplayMode() const;
    void SetDisplayMode(const EDisplayMode&);

    void SetGridVisible(const bool&);
    bool GetGridVisible() const;

    void FloodFillTerritory(const int&, const int&, const STile::ESide&, const STile::ESide&);

    void SetSelection(const QRect&);
    void RemoveSelection();

    void Invalidate();

    QString Title() const;

    QRectF boundingRect() const;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);
    QString WriteToFile() const;

    QString GetAiPath() const;
    void SetAiPath(const QString&);

    void Set2DTileset(const QString&);
    void Set3DTileset(const QString&);
    QString Get2DTileset();
    QString Get3DTileset();

    int GetNumCastles() const;

signals:
    void ClickEvent(int, int);
    void DragEvent(int, int);
    void ReleaseEvent(int, int);
    void KeyEvent(int);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);
    void keyPressEvent(QKeyEvent*);

private slots:
    void FPSTick();

private:
    void DetermineTileTypes();
    void DrawCastle(QPainter*, const SMapObj&);
    void DrawShip(QPainter*, const SMapObj&);
    void DrawSiegeWeapon(QPainter*, const SMapObj&);
    void DrawWall(QPainter*, const SMapObj&);
    void Parse(const QString&);
    void CalculateBoundingRect();
    void Resize();
    void CheckForErrors() const;

    QString DTitle;
    QVector<SMapObj> DObjects;
    QVector<QVector<STile> > DMap;
    QGraphicsScene* DScene;

    EDisplayMode DDisplayMode;
    CTileset* DCurrentTileset;
    CTileset D2DTileset;
    C3DTileset D3DTileset;
    CWallTileset DWallTileset;
    C3DWallTileset D3DWallTileset;


    QPixmap DCastleTile;
    QPixmap D3DCastleTile;

    QPixmap DShipTile;
    QPixmap D3DShipTile;

    QPixmap DSiegeTile;
    QPixmap D3DSiegeTile;

    QRectF DRect;
    QRect DSelectionRect;
    QBrush DSelectionFillBrush;
    QPen DPen;

    QTimer DFPSTimer;
    unsigned DSelectionPulseAlpha;
    bool DSelectionPulse;

    SMapObj* DSelectedObject;

    unsigned DWidth;
    unsigned DHeight;
    bool DObjectsVisible;
    bool DTerritoryVisible;
    bool DGridVisible;

    QString DAiPath;
    QString D2DTilesetPath;
    QString D3DTilesetPath;

    CSoundHandler* DSoundHandler;
};

#endif // CSCENE_H
