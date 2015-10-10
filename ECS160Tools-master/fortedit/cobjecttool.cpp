#include "cscene.h"
#include "cobjecttool.h"
#include "ui_object_tool_props.h"

CObjectTool::CObjectTool(CScene* scene)
    : CTool(scene),
      DPlaceType(SMapObj::typeCastle)
{
    Ui::ObjectToolProperties Prop;
    Prop.setupUi(DPropertiesWidget);
    QObject::connect(Prop.PlaceTypeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(SetPlaceType(QString)));
}

/**
 * Handles a mouse click event. Will place the selected object type under the cursor
 * @param x mouse x tile coordinate
 * @param y mouse y tile coordinate
 */
void CObjectTool::ClickEvent(int x, int y)
{
    if (!IsActive())
    {
        return;
    }

    DScene->PlaceObject(x, y, DPlaceType);
}

/**
 * Handles a mouse drag event. Will move selected objects
 * @param x mouse x tile coordinate
 * @param y mouse y tile coordinate
 */
void CObjectTool::DragEvent(int x, int y)
{
    if (!IsActive())
    {
        return;
    }

    DScene->PlaceObject(x, y, DPlaceType);
}

/**
 * Unused
 */
void CObjectTool::ReleaseEvent(int, int)
{
    // Don't care yet
}

/**
 * Unused
 */
void CObjectTool::KeyEvent(int)
{
    // Don't care yet
}

/**
 * Change the target object type
 * @param type new target object type, one of "Castle", "Ship", or "Siege Weapon"
 */
void CObjectTool::SetPlaceType(const QString& type)
{
    if (type == "Castle")
    {
        DPlaceType = SMapObj::typeCastle;
    }
    else if (type == "Ship")
    {
        DPlaceType = SMapObj::typeShip;
    }
    else if (type == "Siege Weapon")
    {
        DPlaceType = SMapObj::typeSiegeWeapon;
    }
    else
    {
        DPlaceType = SMapObj::typeWall;
    }
}
