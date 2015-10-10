#include <QKeyEvent>
#include <QDebug>
#include <QObject>

#include "ctool.h"
#include "cselecttool.h"
#include "cscene.h"
#include "ui_select_tool_props.h"


CSelectTool::CSelectTool(CScene* scene)
    : CTool(scene)
{
    Ui::SelectToolProperties Prop;
    Prop.setupUi(DPropertiesWidget);
    DXSpinner = Prop.XCoordinateSpinner;
    DYSpinner = Prop.YCoordinateSpinner;
    DXSpinner->setValue(0);
    DYSpinner->setValue(0);

    QObject::connect(Prop.DeleteCastleButton, SIGNAL(clicked()), this, SLOT(DeleteSelection()));
    QObject::connect(Prop.XCoordinateSpinner, SIGNAL(valueChanged(int)), this, SLOT(MoveObjectX(int)));
    QObject::connect(Prop.YCoordinateSpinner, SIGNAL(valueChanged(int)), this, SLOT(MoveObjectY(int)));
}

/**
 * Callback for a mouse click. Attempts to select an object under the mouse cursor
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CSelectTool::ClickEvent(int x, int y)
{
    if (!IsActive())
    {
        return;
    }

    if (x < 0 || y < 0)
    {
        return;
    }

    DXSpinner->setMaximum(DScene->GetWidth() - 1);
    DYSpinner->setMaximum(DScene->GetHeight() - 1);

    DScene->SelectObject(x, y);
    SMapObj* Selected = DScene->GetSelectedObject();
    if (Selected)
    {
        DXSpinner->setValue(Selected->DRect.x());
        DYSpinner->setValue(Selected->DRect.y());
    }
    else
    {
        DXSpinner->setValue(0);
        DYSpinner->setValue(0);
    }
}

/**
 * Callback for mouse drag. Will move the currently selected object if any
 * @param x x coordinate in tiles
 * @param y y coordinate in tiles
 */
void CSelectTool::DragEvent(int x, int y)
{
    if (!IsActive())
    {
        return;
    }

    if (x < 0 || y < 0)
    {
        return;
    }

    SMapObj* Selected = DScene->GetSelectedObject();
    if (!Selected)
    {
        return;
    }

    DXSpinner->setValue(x);
    DYSpinner->setValue(y);
    DScene->MoveSelectedObject(x, y);
}

/**
 * Unused
 */
void CSelectTool::ReleaseEvent(int, int)
{
    // Empty
}

/**
 * Callback for key events
 * @param key key event
 */
void CSelectTool::KeyEvent(int key)
{
    if (!IsActive() || !DScene)
    {
        return;
    }

    if (key == Qt::Key_Delete || key == Qt::Key_Backspace)
    {
        // Delete castles
        DScene->RemoveSelection();
    }
}

/**
 * Deletes the current selection, if any
 */
void CSelectTool::DeleteSelection()
{
    if (!IsActive())
    {
        return;
    }

    DScene->RemoveSelection();
    DXSpinner->setValue(0);
    DYSpinner->setValue(0);
}

/**
 * Moves the current selection's x coordinate in tiles
 * @param x x coordinate in tiles
 */
void CSelectTool::MoveObjectX(const int& x)
{
    if (!IsActive())
    {
        return;
    }

    SMapObj* Selected = DScene->GetSelectedObject();
    if (Selected)
    {
        DScene->MoveSelectedObject(x, Selected->DRect.y());
    }
}

/**
 * Moves the current selection's y coordinate in tiles
 * @param y y coordinate in tiles
 */
void CSelectTool::MoveObjectY(const int& y)
{
    if (!IsActive())
    {
        return;
    }

    SMapObj* Selected = DScene->GetSelectedObject();
    if (Selected)
    {
        DScene->MoveSelectedObject(Selected->DRect.x(), y);
    }
}
