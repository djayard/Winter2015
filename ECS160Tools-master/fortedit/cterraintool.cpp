#include <QObject>
#include <QString>
#include <QFileDialog>

#include "cterraintool.h"
#include "cscene.h"
#include "ui_terrain_tool_props.h"

CTerrainTool::CTerrainTool(CScene* scene)
    : CTool(scene),
      DPlaceAccessible(true)
{   
    Ui::TerrainToolProperties Prop;
    Prop.setupUi(DPropertiesWidget);
    QObject::connect(Prop.TerrainTypeComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(AccessibilityChange(QString)));
    QObject::connect(Prop.Tileset2DFileButton, SIGNAL(clicked()), this, SLOT(Tileset2DChange()));
    QObject::connect(Prop.Tileset3DFileButton, SIGNAL(clicked()), this, SLOT(Tileset3DChange()));
}

/**
 * Sets the placement mode for the terrain
 * @param accessibility accessible vs inaccessible
 */
void CTerrainTool::SetPlaceAccessible(const bool& accessibility)
{
    DPlaceAccessible = accessibility;
}

/**
 * Places the current terrain type at the given tile coordinates
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CTerrainTool::Modify(const unsigned& x, const unsigned& y) const
{
    if (!DScene || !IsActive())
    {
        return;
    }

    if (DPlaceAccessible)
    {
        DScene->PlaceTerrain(x, y);
    }
    else
    {
        DScene->PlaceInaccessibleTerrain(x, y);
    }
}

/**
 * Callback for mouse clicks. Places the current terrain type at the mouse coordinates
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CTerrainTool::ClickEvent(int x, int y)
{
    Modify(x, y);
}

/**
 * Callback for mouse drag. Places the current terrain type at the mouse coordinates
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CTerrainTool::DragEvent(int x, int y)
{
    Modify(x, y);
}

/**
 * Unused
 */
void CTerrainTool::ReleaseEvent(int, int)
{
    // Don't care yet
}

/**
 * Unused
 */
void CTerrainTool::KeyEvent(int)
{
    // Don't care yet
}

/**
 * UI callback function to change the accessbility type of the placing terrain
 * @param index
 */
void CTerrainTool::AccessibilityChange(const QString& index)
{
    SetPlaceAccessible(index == "Accessible");
}

/**
 * Allows the user to change the 2D tileset used. Prompts the user for a new 2D tileset .dat file
 */
void CTerrainTool::Tileset2DChange()
{
    QString filePath = QFileDialog::getOpenFileName(0,
                                  "Choose a 2D tileset file",
                                  QDir::currentPath(),
                                  "Tileset dat file (*.dat)");

    if (filePath.length() > 0)
    {
        DScene->Set2DTileset(filePath);
        DScene->Invalidate();
    }
}

/**
 * Allows the user to change the 3D tileset used. Prompts the user for a new 3D tileset .dat file
 */
void CTerrainTool::Tileset3DChange()
{
    QString filePath = QFileDialog::getOpenFileName(0,
                                  "Choose a 3D tileset file",
                                  QDir::currentPath(),
                                  "Tileset dat file (*.dat)");

    if (filePath.length() > 0)
    {
        DScene->Set3DTileset(filePath);
        DScene->Invalidate();
    }
}
