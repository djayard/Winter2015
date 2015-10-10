#ifndef CTOOL_H
#define CTOOL_H

#include <QWidget>

#include "cscene.h"

/**
 * Base class to represent a tool for the editor
 */
class CTool : public QObject
{
    Q_OBJECT
public:
    CTool(CScene*);

    void UpdateTarget(CScene*);

    void SetActive();
    void SetInactive();
    bool IsActive() const;

    QWidget* GetPropertiesWidget() const;

protected:
    CScene* DScene;
    QWidget* DPropertiesWidget;
    bool DActive;

protected slots:
    virtual void ClickEvent(int, int) = 0;
    virtual void DragEvent(int, int) = 0;
    virtual void ReleaseEvent(int, int) = 0;
    virtual void KeyEvent(int) = 0;
};

#endif // CTOOL_H
