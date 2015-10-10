#ifndef CFORTEDIT_H
#define CFORTEDIT_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QTimer>

#include "cscene.h"
#include "ctool.h"
#include "cterraintool.h"
#include "cterritorytool.h"
#include "cobjecttool.h"
#include "cselecttool.h"
#include "csoundHandler.h"
#include "cluaeditor.h"
#include "ui_engine_invocation_dialog.h"

namespace Ui
{
    class FortEdit;
}

/**
 * Main GUI class for the entire application. Strings together all Ui components
 */
class CFortEdit : public QMainWindow
{
    Q_OBJECT

public:
    explicit CFortEdit(QWidget* parent = 0);
    ~CFortEdit();

public slots:
    void UpdateUi();
    void UpdateWindowTitle();
    void UpdateMapProperties();

    bool SaveSoundSet();
    QString MapFilePath() const;

private:
    void ClearLayout(QLayout*);
    void LoadSettings();
    void SaveSettings();
    Ui::FortEdit* DUi;
    CScene* DScene;
    QGraphicsScene* DGraphicsScene;

    CTool* DActiveTool;
    CTerrainTool DTerrainTool;
    CTerritoryTool DTerritoryTool;
    CObjectTool DObjectTool;
    CSelectTool DSelectTool;
    CSoundHandler DSoundHandler;
    CLuaEditor DLuaEditor;
    QString DMapFilePath;
    QString DEnginePath;
    int DInvocationMode;
    int DInvocationDifficulty;
    bool DInvocationWind;
    int DInvocationIndex;
    Ui::EngineInvocationDialog DInvocationUi;
    QDialog* DInvocationDialog;

private slots:
    void InvokeEngineDialog();
    void InvokeEngine(QAbstractButton*);
    void LocateEngine();
    void UpdateInvocationConfig();
    void UpdateActiveTool();
    void UpdateSceneTitle(const QString&);
    void UpdateSceneWidth(const int&);
    void UpdateSceneHeight(const int&);
    void UpdateSceneObjectVisibility(const bool&);
    void UpdateSceneGridVisibility(const bool&);
    void UpdateSceneDisplayMode(const QString&);
    void ToggleLuaEditor();
    void OpenMap();
    void SaveMap();
    void SaveMapAs();
    void NewMap();
};

#endif // CFORTEDIT_H
