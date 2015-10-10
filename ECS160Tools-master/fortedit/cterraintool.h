#ifndef CTERRAINTOOL_H
#define CTERRAINTOOL_H

#include <QString>

#include "ctool.h"
#include "cscene.h"

/**
 * Tool to place inaccessible and accessible terrain on the map
 */
class CTerrainTool : public CTool
{
    Q_OBJECT
public:
    CTerrainTool(CScene*);

    void SetPlaceAccessible(const bool&);

private:
    void Modify(const unsigned&, const unsigned&) const;

    bool DPlaceAccessible;

protected slots:
    void ClickEvent(int, int);
    void DragEvent(int, int);
    void ReleaseEvent(int, int);
    void KeyEvent(int);

private slots:
    void AccessibilityChange(const QString&);
    void Tileset2DChange();
    void Tileset3DChange();
};

#endif // CTERRAINTOOL_H
