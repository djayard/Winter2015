#ifndef CSELECTTOOL_H
#define CSELECTTOOL_H

#include <QRect>
#include <QVector>
#include <QSpinBox>

#include "ctool.h"
#include "cscene.h"

/**
 * Tool for selecting objects on the map allows for moving and deleting them as well
 */
class CSelectTool : public CTool
{
    Q_OBJECT
public:
    CSelectTool(CScene*);

private:
    QRect DRect;
    QSpinBox *DXSpinner;
    QSpinBox *DYSpinner;

protected:
    void ClickEvent(int, int);
    void DragEvent(int, int);
    void ReleaseEvent(int, int);
    void KeyEvent(int);

protected slots:
    void DeleteSelection();
    void MoveObjectX(const int&);
    void MoveObjectY(const int&);
};

#endif // CSELECTTOOL_H
