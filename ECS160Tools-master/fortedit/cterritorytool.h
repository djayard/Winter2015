#ifndef CTERRITORYTOOL_H
#define CTERRITORYTOOL_H

#include "cscene.h"
#include "ctool.h"

/**
 * Tool to control the territory allocation on the map
 */
class CTerritoryTool : public CTool
{
    Q_OBJECT
public:
    CTerritoryTool(CScene*);

private:
    void Modify(const unsigned&, const unsigned&) const;

    STile::ESide DSide;

protected slots:
    void ClickEvent(int, int);
    void DragEvent(int, int);
    void ReleaseEvent(int, int);
    void KeyEvent(int);

private slots:
    void SideChange(const QString&);
};

#endif // CTERRITORYTOOL_H
