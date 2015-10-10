#ifndef CCASTLETOOL_H
#define CCASTLETOOL_H

#include "ctool.h"
#include "cscene.h"

/**
 * A tool to place various objects on the map
 */
class CObjectTool : public CTool
{
    Q_OBJECT
public:
    CObjectTool(CScene*);

protected slots:
    void ClickEvent(int, int);
    void DragEvent(int, int);
    void ReleaseEvent(int, int);
    void KeyEvent(int);

private:
    SMapObj::EType DPlaceType;

private slots:
    void SetPlaceType(const QString&);
};

#endif // CCASTLETOOL_H
