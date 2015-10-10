#include <QString>
#include <QStringList>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QUnhandledException>
#include <QDebug>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItem>
#include <QImage>
#include <QRect>
#include <QPainter>
#include <QPaintEngine>
#include <QSet>
#include <QColor>
#include <QObject>

#include "cscene.h"
#include "cparseexception.h"
#include "cmapexception.h"

CScene::CScene()
    :
    DTitle("Untitled"),
    DScene(0),
    DDisplayMode(displayMode2D),
    DSelectionPulseAlpha(200),
    DSelectionPulse(false),
    DSelectedObject(0),
    DWidth(10),
    DHeight(10),
    DObjectsVisible(true),
    DTerritoryVisible(false),
    DGridVisible(false),
    DSoundHandler(NULL)
{
    Resize();
}

/**
 * Create a scene from the .map file found at the given path and attach the scene to the given QGraphicsScene
 * @param path path to .map file
 * @param renderscene QGraphicsScene to contain the scene
 */
CScene::CScene(const QString& path, QGraphicsScene* renderscene, CSoundHandler* soundHandler)
    : DScene(renderscene),
      DDisplayMode(displayMode2D),
      DSelectionFillBrush(QColor(100, 100, 255, 100)),
      DPen(QColor(255, 255, 255), 1, Qt::DotLine, Qt::SquareCap, Qt::RoundJoin),
      DSelectionPulseAlpha(200),
      DSelectionPulse(false),
      DSelectedObject(0),
      DWidth(0),
      DHeight(0),
      DObjectsVisible(true),
      DTerritoryVisible(false),
      DGridVisible(false),
      DSoundHandler(soundHandler)
{
    try
    {
        Parse(path);
    }
    catch (CParseException& E)
    {
        QMessageBox* Msg = new QMessageBox;
        Msg->setText(QString("Couldn't load map %1,\n%2" ).arg(path).arg(E.Message()));
        Msg->show();
    }

    setFlags(QGraphicsItem::ItemIsFocusable);
    DScene->addItem(this);
    DScene->setFocusItem(this);

    QObject::connect(&DFPSTimer, SIGNAL(timeout()), this, SLOT(FPSTick()));
    DFPSTimer.setInterval(1000 / 5); // 5 FPS
    SetDisplayMode(displayMode2D);
}

/**
 * @return the map title
 */
QString CScene::GetTitle() const
{
    return DTitle;
}

/**
 * Sets the map title to the given string
 * @param title new map title
 */
void CScene::SetTitle(const QString& title)
{
    DTitle = title;
}

/**
 * Returns the maps width in tiles
 */
unsigned CScene::GetWidth() const
{
    return DWidth;
}

/**
 * Sets the map width
 * @param width width in tiles
 */
void CScene::SetWidth(const unsigned& width)
{
    DWidth = width;
    CalculateBoundingRect();
    Resize();
}

/**
 * Returns the map height in tiles
 */
unsigned CScene::GetHeight() const
{
    return DHeight;
}

/**
 * Sets the map height
 * @param height height in tiles
 */
void CScene::SetHeight(const unsigned& height)
{
    DHeight = height;
    CalculateBoundingRect();
    Resize();
}

/**
 * Toggles the visibility of all objects on the map
 * @param visible visibility
 */
void CScene::SetObjectVisibility(const bool& visible)
{
    if (DObjectsVisible != visible)
    {
        DObjectsVisible = visible;
        update(boundingRect());
    }
}

/**
 * Toggles the visibility of colored territory on the map
 * @param visible visibility
 */
void CScene::SetTerritoryVisiblity(const bool& visible)
{
    if (DTerritoryVisible != visible)
    {
        DTerritoryVisible = visible;
        update(boundingRect());
    }
}

/**
 * Places accessible terrain at the given coordinates
 * @param x mouse x coordinate in tiles
 * @param y mouse y coordinate in tiles
 */
void CScene::PlaceTerrain(const unsigned& x, const unsigned& y)
{
    // Make sure there is no object at this position
    for (int i = 0; i < DObjects.size(); ++i)
    {
        SMapObj Obj = DObjects.at(i);
        if (Obj.DRect.contains(x, y)
                && (Obj.DType == SMapObj::typeShip || Obj.DType == SMapObj::typeSiegeWeapon))
        {
            return;
        }
    }

    DMap[y][x].DType = STile::typeAccessible;

    // Assign side to first !NONE side found
    for (int i = 0; i < 3 && DMap[y][x].DSide == STile::sideNONE; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if ((boundingRect().contains((y - 1 + j) * 12, (x - 1 + i) * 12))
                    && DMap[y - 1 + j][x - 1 + i].DSide != STile::sideNONE)
            {
                DMap[y][x].DSide = DMap[y - 1 + j][x - 1 + i].DSide;
                break;
            }
        }
    }

    Invalidate();
}

/**
 * Places inaccessible terrain at the given coordinates
 * @param x mouse x coordinate in tiles
 * @param y mouse y coordiante in tiles
 */
void CScene::PlaceInaccessibleTerrain(const unsigned& x, const unsigned& y)
{
    // Make sure there is no object at this position
    for (int i = 0; i < DObjects.size(); ++i)
    {
        SMapObj Obj = DObjects.at(i);
        if (Obj.DRect.contains(x, y)
                && (Obj.DType == SMapObj::typeCastle || Obj.DType == SMapObj::typeWall))
        {
            return;
        }
    }

    DMap[y][x].DType = STile::typeInaccessible;
    DMap[y][x].DSide = STile::sideNONE;
    Invalidate();
}

/**
 * Sorting function based on y coordinates to standardize rendering order
 * @param a first object
 * @param b second object
 * @return the preceding object based on the y coordinate
 */
bool YSort(const SMapObj& a, SMapObj& b)
{
    if (a.DRect.y() != b.DRect.y())
    {
        return a.DRect.y() < b.DRect.y() ;
    }

    return a.DRect.x() < b.DRect.x();
}

/**
 * Places an object at the current mouse position
 * @param x mouse x coordinate in tiles
 * @param y mouse y coordinate in tiles
 * @param type object type to place
 */
void CScene::PlaceObject(const unsigned& x, const unsigned& y, const SMapObj::EType& type)
{
    STile::EType TileType = DMap[y][x].DType;
    CTileset::EType Orientation = DMap[y][x].DOrientation;
    if (TileType == STile::typeInaccessible && (type == SMapObj::typeCastle || type == SMapObj::typeWall))
    {
        return;
    }
    else if (TileType == STile::typeAccessible
             && (type == SMapObj::typeShip || type == SMapObj::typeSiegeWeapon))
    {
        return;
    }
    else if (Orientation == CTileset::typeWater && type == SMapObj::typeSiegeWeapon)
    {
        return;
    }
    else if (Orientation != CTileset::typeWater && type == SMapObj::typeShip)
    {
        return;
    }

    QRect rect(x, y, 1, 1);
    if (type == SMapObj::typeCastle || type == SMapObj::typeShip) {
        rect.setWidth(2);
        rect.setHeight(2);
    }

    // Check that we are not placing the object on top of another object
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects[i].DRect.intersects(rect))
        {
            return;
        }
    }

    SMapObj Object(rect, type);
    DObjects.push_back(Object);
    qSort(DObjects.begin(), DObjects.end(), YSort);
    Invalidate();
}

/**
 * Determines if an object exists at the given coordinates and returns it for selection
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CScene::SelectObject(const unsigned& x, const unsigned& y)
{
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects.at(i).DRect.contains(x, y))
        {
            DSelectedObject = &(DObjects[i]);
            Invalidate();
            return;
        }
    }

    DSelectedObject = NULL;
    Invalidate();
}

/**
 * @return the currently selected object, or null
 */
SMapObj *CScene::GetSelectedObject() const
{
    return DSelectedObject;
}

/**
 * Moves the selected object to the destination coordinates. Will prevent objects from moving on top of each other
 * @param newX x coordinate in tiles
 * @param newY y coordinate in tiles
 */
void CScene::MoveSelectedObject(const unsigned& newX, const unsigned& newY)
{
    if (DSelectedObject == NULL)
    {
        return;
    }

    QRect NewRect(newX, newY, DSelectedObject->DRect.width(), DSelectedObject->DRect.height());

    // Check that we cant move castles or walls over inaccessible terrain
    for (int i = NewRect.x(); i < NewRect.x() + NewRect.width(); ++i)
    {
        for (int j = NewRect.y(); j < NewRect.y() + NewRect.height(); ++j)
        {
            if (DMap[j][i].DType == STile::typeInaccessible
                    && (DSelectedObject->DType == SMapObj::typeCastle || DSelectedObject->DType == SMapObj::typeWall))
            {
                return;
            } else if (DMap[j][i].DType == STile::typeAccessible
                       && (DSelectedObject->DType == SMapObj::typeShip || DSelectedObject->DType == SMapObj::typeSiegeWeapon))
            {
                return;
            } else if (DMap[j][i].DType == STile::typeInaccessible && DMap[j][i].DOrientation == CTileset::typeWater
                       && DSelectedObject->DType == SMapObj::typeSiegeWeapon)
            {
                return;
            }
        }
    }


    // Check that we are not placing the object on top of another object
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects.at(i).DRect.intersects(NewRect))
        {
            return;
        }
    }

    DSelectedObject->DRect = NewRect;
    Invalidate();
}

/**
 * @return the current display mode, one of 2D or 3D
 */
CScene::EDisplayMode CScene::GetDisplayMode() const
{
    return DDisplayMode;
}

/**
 * Sets the display mode
 * @param mode display mode to set
 */
void CScene::SetDisplayMode(const CScene::EDisplayMode& mode)
{
    DDisplayMode = mode;

    switch (DDisplayMode)
    {
        case displayMode2D:
            DCurrentTileset = &D2DTileset;
            DFPSTimer.stop();
            break;
        case displayMode3D:
            DCurrentTileset = &D3DTileset;
            DFPSTimer.start();
            break;
    default: break;
    }

    Invalidate();
}

/**
 * Toggles the rendering of the tile grid
 * @param state visibility
 */
void CScene::SetGridVisible(const bool& state)
{
    DGridVisible = state;
    Invalidate();
}

/**
 * @return the visibility of the tile grid
 */
bool CScene::GetGridVisible() const
{
    return DGridVisible;
}

/**
 * Fills all accessible territory starting at the given coordinates with a given territory color
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 * @param oldColor territory color to replace
 * @param newColor new territory color
 */
void CScene::FloodFillTerritory(const int& x, const int& y, const STile::ESide& oldColor, const STile::ESide& newColor)
{
    if (y < DMap.size() && x < DMap.at(y).size())
    {
        STile Tile = DMap[y][x];

        if (Tile.DType == STile::typeAccessible && Tile.DSide != newColor)
        {
            DMap[y][x].DSide = newColor;
            FloodFillTerritory(x, y - 1, oldColor, newColor); // west
            FloodFillTerritory(x, y + 1, oldColor, newColor); // east
            FloodFillTerritory(x - 1, y, oldColor, newColor); // north
            FloodFillTerritory(x + 1, y, oldColor, newColor); // south
            FloodFillTerritory(x - 1, y - 1, oldColor, newColor); // nw
            FloodFillTerritory(x - 1, y + 1, oldColor, newColor); // ne
            FloodFillTerritory(x + 1, y - 1, oldColor, newColor); // sw
            FloodFillTerritory(x + 1, y + 1, oldColor, newColor); // se
        }
    }
}

/**
 * Sets the selection rectangle
 * @param rect selection rectangle
 * @deprecated
 */
void CScene::SetSelection(const QRect& rect)
{
    DSelectionRect = rect;
    Invalidate();
}

/**
 * Deletes the currently selected object(s)
 */
void CScene::RemoveSelection()
{
    if (DSelectedObject == NULL)
    {
        return;
    }

    QVector<SMapObj>::iterator It = DObjects.begin();
    while (It != DObjects.end())
    {
        if (DSelectedObject->DRect == It->DRect)
        {
            It = DObjects.erase(It);
            DSelectedObject = NULL;
            Invalidate();
            return;
        }
        else
        {
            ++It;
        }
    }
}

/**
 * Calls for a rendering pass
 */
void CScene::Invalidate()
{
    if (!DScene)
    {
        return;
    }

    DetermineTileTypes();
    update(boundingRect());
    DScene->update();
}

/**
 * @return the bounding rectangle for the graphics widget
 */
QRectF CScene::boundingRect() const
{
    return DRect;
}

/**
 * Rendering function, renders all aspects of the map
 * @param painter painter to render with
 */
void CScene::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    bool even = true;
    // Terrain
    for (int i = 0; i < DMap.size(); ++i)
    {
        for (int j = 0; j < DMap.at(i).size(); ++j)
        {
            if (DMap.at(i).at(j).DType == STile::typeInaccessible)
            {
                painter->drawPixmap(j * 12, i * 12, 12, 12, DCurrentTileset->GetTile(DMap.at(i).at(j).DOrientation));
            }
            else if (!DTerritoryVisible)
            {
                if (even)
                {
                    painter->drawPixmap(j * 12, i * 12, 12, 12, DCurrentTileset->GetTile(CTileset::typeGrassEven));
                }
                else
                {
                    painter->drawPixmap(j * 12, i * 12, 12, 12, DCurrentTileset->GetTile(CTileset::typeGrassOdd));
                }
            }

            even = !even;
        }

        if (DMap.at(0).size() % 2 == 0) {
            even = !even;
        }
    }

    // Territory
    if (DTerritoryVisible)
    {
        for (int m = 0; m < DMap.size(); ++m)
        {
            for (int n = 0; n < DMap.at(m).size(); ++n)
            {
                if(DMap.at(m).at(n).DSide == STile::sideBLUE)
                {
                    painter->drawPixmap(n * 12, m * 12, 12, 12, D2DTileset.GetTile(CTileset::typeFloorBlue));
                }
                else if (DMap.at(m).at(n).DSide == STile::sideRED)
                {
                    painter->drawPixmap(n * 12, m * 12, 12, 12, D2DTileset.GetTile(CTileset::typeFloorRed));
                }
                else if (DMap.at(m).at(n).DSide == STile::sideYELLOW)
                {
                    painter->drawPixmap(n * 12, m * 12, 12, 12, D2DTileset.GetTile(CTileset::typeFloorYellow));
                }
            }
        }
    }

    // Selection
    if (DSelectedObject && DObjectsVisible)
    {
        unsigned Alpha = (DDisplayMode == displayMode3D) ? DSelectionPulseAlpha : 255;
        QRect Rect = DSelectedObject->DRect;
        Rect = QRect(Rect.x() * 12, Rect.y() * 12, Rect.width() * 12, Rect.height() * 12);

        if (boundingRect().contains(Rect))
        {
            STile::ESide Side = DMap.at(DSelectedObject->DRect.y())
                                    .at(DSelectedObject->DRect.x())
                                    .DSide;

            if (Side == STile::sideBLUE)
            {
                painter->fillRect(Rect, QColor(0, 0, 255, Alpha));
            }
            else if (Side == STile::sideRED)
            {
                painter->fillRect(Rect, QColor(255, 0, 0, Alpha));
            }
            else if (Side == STile::sideYELLOW)
            {
                painter->fillRect(Rect, QColor(255, 255, 0, Alpha));
            }

            painter->drawRect(Rect);
        }
    }

    if (DSelectionRect.left() > 0 && DSelectionRect.top() > 0)
    {
        painter->fillRect(DSelectionRect, DSelectionFillBrush);
        painter->drawRect(DSelectionRect);
    }

    // Objects
    for (int i = 0; DObjectsVisible && i < DObjects.size(); ++i)
    {
        SMapObj Obj = DObjects.at(i);
        if (Obj.DType == SMapObj::typeCastle)
        {
            DrawCastle(painter, Obj);
        }
        else if (Obj.DType == SMapObj::typeShip)
        {
            DrawShip(painter, Obj);
        }
        else if (Obj.DType == SMapObj::typeSiegeWeapon)
        {
            DrawSiegeWeapon(painter, Obj);
        }
        else if (Obj.DType == SMapObj::typeWall)
        {
            DrawWall(painter, Obj);
        }
    }

    painter->drawRect(12, 12, DWidth * 12, DHeight * 12);

    // Draw the grid if visible
    if (!DGridVisible)
    {
        return;
    }

    painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
    painter->setPen(DPen);
    for (unsigned i = 1; i < DHeight + 2; ++i)
    {
        painter->drawLine(12, 12 * i, (DWidth + 1) * 12, i * 12);
    }

    for (unsigned i = 1; i < DWidth + 2; ++i)
    {
        painter->drawLine(12 * i, 12, i * 12, (DHeight + 1) * 12);
    }
}

/**
 * Callback event for mouse clicks
 * @param event mouse event
 */
void CScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF click = event->lastScenePos();

    int x = click.x() / 12;
    int y = click.y() / 12;

    if (y >= 0 && y < DMap.size() && x >= 0 && x < DMap.at(0).size())
    {
        emit ClickEvent(x, y);
    }
}

/**
 * Callback for mouse movement
 * @param event mouse event
 */
void CScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF click = event->lastScenePos();

    int x = click.x() / 12;
    int y = click.y() / 12;

    if (y >= 0 && y < DMap.size() && x >= 0 && x < DMap.at(0).size())
    {
        emit DragEvent(x, y);
    }

    QGraphicsItem::mouseMoveEvent(event);
}

/**
 * Callback for mouse click release
 * @param event mouse event
 */
void CScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF click = event->lastScenePos();

    int x = click.x() / 12;
    int y = click.y() / 12;

    if (y >= 0 && y < DMap.size() && x >= 0 && x < DMap.at(0).size())
    {
        emit ReleaseEvent(x, y);
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

/**
 * Callback for key presses
 * @param event key event
 */
void CScene::keyPressEvent(QKeyEvent* event)
{
    emit KeyEvent(event->key());
    QGraphicsItem::keyPressEvent(event);
}

/**
 * Determines the shoreline and other tile types displayed on the map
 */
void CScene::DetermineTileTypes()
{
    bool even = true;
    for (int i = 1; i < DMap.size() - 1; ++i)
    {
        for (int j = 1; j < DMap.at(i).size() - 1; ++j)
        {
            even = !even;
            if (DMap.at(i).at(j).DType != STile::typeInaccessible)
            {
                continue;
            }

            int type = 0;
            // Check neighbors (4 directions)
            type |= DMap.at(i - 1).at(j).DType == STile::typeAccessible ? STile::NORTH : 0;
            type |= DMap.at(i + 1).at(j).DType == STile::typeAccessible ? STile::SOUTH : 0;
            type |= DMap.at(i).at(j + 1).DType == STile::typeAccessible ? STile::EAST : 0;
            type |= DMap.at(i).at(j - 1).DType == STile::typeAccessible ? STile::WEST : 0;

            if (type != 0)
            {
                type |= (even) ? STile::EVEN : 0;
                DMap[i][j].DOrientation = D2DTileset.GetType(type);
            }
            else
            {
                // Need some further checking... (8 directions)
                bool NW = DMap.at(i - 1).at(j - 1).DType == STile::typeAccessible;
                bool NE = DMap.at(i - 1).at(j + 1).DType == STile::typeAccessible;
                bool SE = DMap.at(i + 1).at(j + 1).DType == STile::typeAccessible;
                bool SW = DMap.at(i + 1).at(j - 1).DType == STile::typeAccessible;

                if (!(NW || NE || SE || SW))
                {
                    DMap[i][j].DOrientation = CTileset::typeWater;
                }
                else
                {
                    // TODO: Improve binary encoding such that these types are included
                    if (NW && NE && SE && SW)
                    {
                        DMap[i][j].DOrientation = CTileset::typeHole;
                    }
                    else if (NW && SE)
                    {
                        DMap[i][j].DOrientation = CTileset::typeShoreNWSE;
                    }
                    else if (NE && SW)
                    {
                        DMap[i][j].DOrientation = CTileset::typeShoreNESW;
                    }
                    else if (NW)
                    {
                        DMap[i][j].DOrientation = CTileset::typeShoreNW;
                    }
                    else if (NE)
                    {
                        DMap[i][j].DOrientation = CTileset::typeShoreNE;
                    }
                    else if (SE)
                    {
                        DMap[i][j].DOrientation = CTileset::typeShoreSE;
                    }
                    else if (SW)
                    {
                        DMap[i][j].DOrientation = CTileset::typeShoreSW;
                    }
                    else
                    {
                        // Debug purposes
                        DMap[i][j].DOrientation = CTileset::typeFloorEven;
                    }
                }
            }
        }

        if (DMap.at(0).size() % 2 == 0) {
            even = !even;
        }
    }
}

/**
 * Draws the given castle with the given painter
 * @param painter painter to draw with
 * @param castle castle to draw
 */
void CScene::DrawCastle(QPainter* painter, const SMapObj& castle)
{
    if (DDisplayMode == displayMode2D)
    {
        painter->drawPixmap(castle.DRect.x() * 12,
                            castle.DRect.y() * 12,
                            castle.DRect.width() * 12,
                            castle.DRect.height() * 12,
                            DCastleTile);
    }
    else
    {
        painter->drawPixmap(castle.DRect.x() * 12,
                            castle.DRect.y() * 12 - 6,
                            castle.DRect.width() * 12,
                            D3DCastleTile.height(),
                            D3DCastleTile);
    }
}

/**
 * Draws the given ship with the given painter
 * @param painter painter to draw with
 * @param ship ship to draw
 */
void CScene::DrawShip(QPainter* painter, const SMapObj& ship)
{
    if (DDisplayMode == displayMode2D)
    {
        painter->drawPixmap(ship.DRect.x() * 12,
                            ship.DRect.y() * 12,
                            ship.DRect.width() * 12,
                            ship.DRect.height() * 12,
                            DShipTile);
    }
    else
    {
        painter->drawPixmap(ship.DRect.x() * 12,
                            ship.DRect.y() * 12 - 6,
                            ship.DRect.width() * 12,
                            D3DShipTile.height(),
                            D3DShipTile);
    }
}

/**
 * Draws the given siege weapon with the given painter
 * @param painter painter to draw with
 * @param wep siege weapon to draw
 */
void CScene::DrawSiegeWeapon(QPainter* painter, const SMapObj& wep)
{
    if (DDisplayMode == displayMode2D)
    {
        painter->drawPixmap(wep.DRect.x() * 12,
                            wep.DRect.y() * 12,
                            wep.DRect.width() * 12,
                            wep.DRect.height() * 12,
                            DSiegeTile);
    }
    else
    {
        painter->drawPixmap(wep.DRect.x() * 12,
                            wep.DRect.y() * 12,
                            wep.DRect.width() * 12,
                            wep.DRect.height() * 12,
                            D3DSiegeTile);
    }
}

/**
 * Determines the type of wall fragment to draw and draws it with the given painter
 * @param painter painter to draw with
 * @param wall wall object to draw
 */
void CScene::DrawWall(QPainter* painter, const SMapObj& wall)
{
    unsigned X = wall.DRect.x() * 12;
    unsigned Y = wall.DRect.y() * 12;

    int type = 0;
    for (int i = 0; i < DObjects.size(); ++i)
    {
        SMapObj O = DObjects.at(i);
        if (O.DType != SMapObj::typeWall || O.DRect == wall.DRect)
        {
            continue;
        }

        type |= (O.DRect.y() == wall.DRect.y() - 1 && O.DRect.x() == wall.DRect.x()) ? CWallTileset::NORTH : 0;
        type |= (O.DRect.y() == wall.DRect.y() + 1 && O.DRect.x() == wall.DRect.x()) ? CWallTileset::SOUTH : 0;
        type |= (O.DRect.y() == wall.DRect.y() && O.DRect.x() == wall.DRect.x() - 1) ? CWallTileset::WEST : 0;
        type |= (O.DRect.y() == wall.DRect.y() && O.DRect.x() == wall.DRect.x() + 1) ? CWallTileset::EAST : 0;

        if (DDisplayMode == displayMode2D)
        {
            continue;
        }

        type |= (O.DRect.y() == wall.DRect.y() - 1 && O.DRect.x() == wall.DRect.x() - 1) ? C3DWallTileset::NORTH_WEST : 0;
        type |= (O.DRect.y() == wall.DRect.y() - 1 && O.DRect.x() == wall.DRect.x() + 1) ? C3DWallTileset::NORTH_EAST : 0;
        type |= (O.DRect.y() == wall.DRect.y() + 1 && O.DRect.x() == wall.DRect.x() - 1) ? C3DWallTileset::SOUTH_WEST : 0;
        type |= (O.DRect.y() == wall.DRect.y() + 1 && O.DRect.x() == wall.DRect.x() + 1) ? C3DWallTileset::SOUTH_EAST : 0;
    }

    STile::ESide Side = DMap.at(wall.DRect.y()).at(wall.DRect.x()).DSide;
    if (DDisplayMode == displayMode2D)
    {
        CWallTileset::EWallType WallType = DWallTileset.GetType(type);
        painter->drawPixmap(X, Y, DWallTileset.GetWallTile(WallType, Side));
    }
    else
    {
        C3DWallTileset::E3DWallType WallType = D3DWallTileset.GetType(type);
        painter->drawPixmap(X, Y - 12, D3DWallTileset.GetWallTile(WallType, Side));
    }
}

/**
 * Parses a .map file and loads all necessary information into the current object
 * @param path path to .map file
 */
void CScene::Parse(const QString& path)
{
    QString Line;
    QFile File(path);

    if (!File.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        throw CParseException("Can't open the file");
    }

    QTextStream TextIn(&File);

    // Read in title
    DTitle = TextIn.readLine();

    // Load tileset
    Set2DTileset(TextIn.readLine());

    // Load 3D tileset
    Set3DTileset(TextIn.readLine());

    // Load Wall tileset
    DWallTileset = CWallTileset("data/2DWallFloor.dat");

    // Load 3D Wall tileset
    D3DWallTileset = C3DWallTileset("data/3DWall.dat");

    // Load in AI Script path
    DAiPath = TextIn.readLine();

    // Load the sound set
    if( !(DSoundHandler->LoadSoundSet(TextIn.readLine().remove('\n').remove('\r'))))
    {
        DSoundHandler->LoadDefaultFiles();
    }

    // Read in dimensions
    Line = TextIn.readLine();

    // To Do: Change file type to whatever it will be
    if (Line.contains(".lua"))
    {
        DAiPath = Line;
        Line = TextIn.readLine();
    }

    if (Line.contains(".dat"))
    {
        // To Do: ACTUALLY READ IN SOUND FILE
        Line = TextIn.readLine();
    }

    QStringList DimList = Line.split(' ');

    if (DimList.size() < 2)
    {
        throw CParseException("The map is missing one or more dimensions");
    }

    DWidth = DimList[0].toUInt();
    DHeight = DimList[1].toUInt();

    if (DWidth < 8 || DHeight < 8)
    {
        throw CParseException("One or more of the map's dimensions is lower than 8");
    }

    Resize();
    CalculateBoundingRect();

    // Start reading in map data
    for (unsigned i = 0; i < DHeight + 2; ++i)
    {
        for (unsigned j = 0; j < DWidth + 2; ++j)
        {
            STile Tile;
            Line = TextIn.read(1);

            if (Line[0] == 'B')
            {
                Tile.DSide = STile::sideBLUE;
            }
            else if (Line[0] == 'R')
            {
                Tile.DSide = STile::sideRED;
            }
            else if (Line[0] == 'Y')
            {
                Tile.DSide = STile::sideYELLOW;
            }
            else if (Line[0] == ' ')
            {
                Tile.DSide = STile::sideNONE;
            }
            else
            {
                throw CParseException("Unknown side specified in map data!");
            }

            Tile.DX = j;
            Tile.DY = i;
            Tile.DOrientation = CTileset::typeWater;

            if (Tile.DSide == STile::sideNONE)
            {
                Tile.DType = STile::typeInaccessible;
            }
            else
            {
                Tile.DType = STile::typeAccessible;
            }

            DMap[i][j] = Tile;
        }

        // Go to the next line
        TextIn.readLine();
    }

    // Read in castles
    Line = TextIn.readLine();
    DObjects.resize(Line.toUInt());

    for (int i = 0; i < DObjects.size(); ++i)
    {
        Line = TextIn.readLine();
        DimList = Line.split(' ');

        if (DimList.size() < 2)
        {
            throw CParseException("A Castle has incomplete dimensions");
        }

        SMapObj Castle(QRect(DimList[0].toUInt(), DimList[1].toUInt(), 2, 2), SMapObj::typeCastle);
        DObjects[i] = Castle;
    }

    // TODO: Read in Ships / Siege weapons (need to define in map format)

    File.close();

    // Load assets
    // TODO: Probably want to do this somewhere else
    QImage Image;
    Image.load("pngs/2DCastleCannon.png");
    QPixmap Pixmap = QPixmap::fromImage(Image);
    DCastleTile = Pixmap.copy(0, 0, 24, 24);

    Image.load("pngs/3DCastles.png");
    Pixmap = QPixmap::fromImage(Image);
    D3DCastleTile = Pixmap.copy(0, 0, 24, 36);

    Image.load("pngs/2DShips.png");
    Pixmap = QPixmap::fromImage(Image);
    DShipTile = Pixmap.copy(0, 24, 24, 24);

    Image.load("pngs/3DShips.png");
    Pixmap = QPixmap::fromImage(Image);
    D3DShipTile = Pixmap.copy(0, 36, 24, 36);

    Image.load("pngs/2DSiege.png");
    Pixmap = QPixmap::fromImage(Image);
    DSiegeTile = Pixmap.copy(0, 0, 12, 12);

    Image.load("pngs/3DSiege.png");
    Pixmap = QPixmap::fromImage(Image);
    D3DSiegeTile = Pixmap.copy(0, 0, 12, 12);
}

/**
 * Calculates the bounding rectangle that encompasses the map
 */
void CScene::CalculateBoundingRect()
{
    DRect = QRectF(0, 0, (DWidth + 2) * 12, (DHeight + 2) * 12);
}

/**
 * Resizes the map to the internal width and height values
 */
void CScene::Resize()
{
    qDebug() << DHeight << DWidth;
    if (DHeight < 8 || DWidth < 8)
    {
        return;
    }

    DMap.resize(DHeight + 2);
    for (int i = 0; i < DMap.size(); ++i)
    {
        DMap[i].resize(DWidth + 2);
    }

    Invalidate();
}

/**
 * Checks the map for various errors, such as a player not having any castles
 */
void CScene::CheckForErrors() const
{
    // Check that at least two players exist
    QSet<STile::ESide> MapSides;

    for (unsigned i = 0; i < DHeight + 2; ++i)
    {
        for (unsigned j = 0; j < DWidth + 2; ++j)
        {
            STile Tile = DMap[i][j];

            if (Tile.DSide == STile::sideNONE && Tile.DType == STile::typeAccessible)
            {
                throw CMapException("There is land with no player side assigned to it");
            }

            if (Tile.DSide != STile::sideNONE)
            {
                MapSides.insert(Tile.DSide);

                // Check for territory not separated by water
                if (i > 0 && j > 0 && i < DMap.size() - 1 && j < DMap.size() - 1)
                {
                    bool error = false;
                    STile::ESide colorChecker = Tile.DSide;
                    STile TileChecker = DMap[i-1][j]; // Check North
                    error = (TileChecker.DSide != STile::sideNONE && TileChecker.DSide != colorChecker) || error;

                    TileChecker = DMap[i+1][j]; // Check South
                    error = (TileChecker.DSide != STile::sideNONE && TileChecker.DSide != colorChecker) || error;

                    TileChecker = DMap[i][j+1]; // Check East
                    error = (TileChecker.DSide != STile::sideNONE && TileChecker.DSide != colorChecker) || error;

                    TileChecker = DMap[i][j-1]; // Check West
                    error = (TileChecker.DSide != STile::sideNONE && TileChecker.DSide != colorChecker) || error;

                    if (error)
                    {
                        throw CMapException("Water must separate each Player's territory");
                    }
                }
            }
        }
    }

    if (MapSides.size() < 2)
    {
        // There's only one player
        throw CMapException("There is only one player, please add at least one more player");
    }

    // Check that each player has at least one castle
    for (int i = 0; i < DObjects.size(); ++i)
    {
        unsigned X = DObjects[i].DRect.x();
        unsigned Y = DObjects[i].DRect.y();

        if (DMap[Y][X].DSide != STile::sideNONE && DObjects[i].DType == SMapObj::typeCastle)
        {
            MapSides.remove(DMap[Y][X].DSide);
        }
    }

    if (MapSides.size() != 0)
    {
        // Not everyone has at least one castle
        throw CMapException("Not every player has at least one castle, please add castles");
    }
}

/**
 * Ticks the FPS for animated tilesets, also provides a cheap lerping method for selection animations
 */
void CScene::FPSTick()
{
    if (!DScene)
    {
        return;
    }

    // Cheap linear lerping
    DSelectionPulseAlpha += DSelectionPulse ? 50 : -50;
    if (DSelectionPulseAlpha >= 255)
    {
        DSelectionPulseAlpha = 255;
        DSelectionPulse = false;
    }
    else if (DSelectionPulseAlpha <= 100)
    {
        DSelectionPulseAlpha = 100;
        DSelectionPulse = true;
    }

    D3DTileset.TickFrame();
    update(boundingRect());
    DScene->update();
}

/**
 * Exports the object to a .map file
 * @param soundData soundset handler
 * @return .map format representation of the current object
 */
QString CScene::WriteToFile() const
{
    CheckForErrors();
    QString mapText;

    mapText = DTitle + "\n";
    mapText += D2DTilesetPath + "\n";
    mapText += D3DTilesetPath + "\n";
    mapText += DAiPath + "\n";
    mapText += DSoundHandler->CreateSoundSet() + '\n';
    mapText += QString::number(DWidth) + " " + QString::number(DHeight) + "\n";

    for (unsigned i = 0; i < DHeight + 2; ++i)
    {
        for (unsigned j = 0; j < DWidth + 2; ++j)
        {
            if (DMap.at(i).at(j).DSide == STile::sideRED)
            {
                mapText += "R";
            }

            if (DMap.at(i).at(j).DSide == STile::sideBLUE)
            {
                mapText += "B";
            }

            if (DMap.at(i).at(j).DSide == STile::sideYELLOW)
            {
                mapText += "Y";
            }

            if (DMap.at(i).at(j).DSide == STile::sideNONE)
            {
                mapText += " ";
            }
        }
        mapText += "\n";
    }

    unsigned NumCastles = 0;
    unsigned NumShips = 0;
    unsigned NumSiege = 0;
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects[i].DType == SMapObj::typeCastle)
        {
            ++NumCastles;
        }
        else if (DObjects[i].DType == SMapObj::typeShip)
        {
            ++NumShips;
        }
        else
        {
            ++NumSiege;
        }
    }

    mapText += QString::number(NumCastles) + "\n";
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects[i].DType != SMapObj::typeCastle)
        {
            continue;
        }

        mapText += QString::number(DObjects[i].DRect.x()) + " " + QString::number(DObjects[i].DRect.y()) + "\n";
    }

    mapText += QString::number(NumShips) + "\n";
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects[i].DType != SMapObj::typeShip)
        {
            continue;
        }

        mapText += QString::number(DObjects[i].DRect.x()) + " " + QString::number(DObjects[i].DRect.y()) + "\n";
    }

    return mapText;
}

/**
 * @return the map's title
 */
QString CScene::Title() const
{
    return DTitle;
}

/**
 * @return the AI script path
 */
QString CScene::GetAiPath() const
{
    return DAiPath;
}

/**
 * Sets the AI script path
 * @param path path to AI script
 */
void CScene::SetAiPath(const QString& path)
{
    DAiPath = path;
}

/**
 * Sets the 2D tileset to be used
 * @param path path to 2D tileset .dat file
 */
void CScene::Set2DTileset(const QString& path)
{
    D2DTileset = CTileset(path);
    D2DTilesetPath = path;
}

/**
 * Sets the 3D tileset to be used
 * @param path path to the 3D tileset .dat file
 */
void CScene::Set3DTileset(const QString& path)
{
    D3DTileset = C3DTileset(path);
    D3DTilesetPath = path;
}

/**
 * @return the 2D tileset .dat path in use
 */
QString CScene::Get2DTileset()
{
    return D2DTilesetPath;
}

/**
 * @return the 3D tileset .dat path in use
 */
QString CScene::Get3DTileset()
{
    return D3DTilesetPath;
}

/**
 * Gets the number of castles in the scene
 * @return number of castles in the scene
 */
int CScene::GetNumCastles() const
{
    int Num = 0;
    for (int i = 0; i < DObjects.size(); ++i)
    {
        if (DObjects.at(i).DType == SMapObj::typeCastle)
        {
            ++Num;
        }
    }

    return Num;
}
