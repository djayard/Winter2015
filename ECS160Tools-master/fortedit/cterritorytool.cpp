#include <QObject>

#include "cterritorytool.h"
#include "cscene.h"
#include "ui_territory_tool_props.h"

CTerritoryTool::CTerritoryTool(CScene* scene)
    : CTool(scene),
      DSide(STile::sideRED)
{
    Ui::TerritoryToolProperties Prop;
    Prop.setupUi(DPropertiesWidget);
    QObject::connect(Prop.SideCombobox, SIGNAL(currentIndexChanged(QString)), this, SLOT(SideChange(QString)));
}

/**
 * Places the current territory into the chunk of land containing the given coordinates
 * @param x x coordinates in tiles
 * @param y y coordinates in tiles
 */
void CTerritoryTool::Modify(const unsigned& x, const unsigned& y) const
{
    if (!DScene || !IsActive())
    {
        return;
    }

    DScene->FloodFillTerritory(x, y, DSide, DSide);
    DScene->Invalidate();
}

/**
 * Callback for mouse clicks. Fills the given coordinates with the current territory color
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CTerritoryTool::ClickEvent(int x, int y)
{
    Modify(x, y);
}

/**
 * Unused
 */
void CTerritoryTool::DragEvent(int, int)
{
    // With flood fill we no longer need drag event
}

/**
 * Unused
 */
void CTerritoryTool::ReleaseEvent(int, int)
{
    // Don't care yet
}

/**
 * Unused
 */
void CTerritoryTool::KeyEvent(int)
{
    // Don't care yet
}

/**
 * Changes the placing territory color
 * @param side new color to place
 */
void CTerritoryTool::SideChange(const QString& side)
{
    if (side == "Red")
    {
        DSide = STile::sideRED;
    }
    else if (side == "Blue")
    {
        DSide = STile::sideBLUE;
    }
    else if (side == "Yellow")
    {
        DSide = STile::sideYELLOW;
    }
}
