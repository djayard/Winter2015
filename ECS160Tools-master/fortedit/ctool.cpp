#include <QObject>
#include <QWidget>

#include "ctool.h"
#include "cscene.h"
#include "ui_fortedit.h"

CTool::CTool(CScene* scene)
    : DScene(scene),
      DActive(false)
{
    DPropertiesWidget = new QWidget();
    UpdateTarget(scene);
}

/**
 * Reconnects UI components when a scene changes
 * @param scene scene to connect to
 */
void CTool::UpdateTarget(CScene* scene)
{
    if (scene)
    {
        QObject::connect(scene, SIGNAL(ClickEvent(int,int)), this, SLOT(ClickEvent(int,int)));
        QObject::connect(scene, SIGNAL(DragEvent(int,int)), this, SLOT(DragEvent(int,int)));
        QObject::connect(scene, SIGNAL(ReleaseEvent(int,int)), this, SLOT(ReleaseEvent(int,int)));
        QObject::connect(scene, SIGNAL(KeyEvent(int)), this, SLOT(KeyEvent(int)));
        SetActive();
    }
    else
    {
        SetInactive();
    }

    DScene = scene;
}

/**
 * Sets the tool as active and listening for events
 */
void CTool::SetActive()
{
    DActive = true;
}

/**
 * Sets the tool as inactive and not listening for events
 */
void CTool::SetInactive()
{
    DActive = false;
}

/**
 * @return whether the tool is active or not
 */
bool CTool::IsActive() const
{
    return DActive;
}

/**
 * @return a widget containing the properties for this tool
 */
QWidget* CTool::GetPropertiesWidget() const
{
    return DPropertiesWidget;
}
