#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QSettings>
#include <QProcess>

#include "cfortedit.h"
#include "cmapexception.h"
#include "cscene.h"
#include "ui_fortedit.h"
#include "cterraintool.h"

#include "csoundHandler.h"

#define INVOCATION_TARGET_KEY "invocation_target"
#define INVOCATION_MODE_KEY "invocation_mode"
#define INVOCATION_AI_DIFFICULTY_KEY "invocation_difficulty"
#define INVOCATION_WIND_KEY "invocation_wind"
#define INVOCATION_INDEX_KEY "invocation_index"

/**
 * Constructs and displays the main UI for FortEdit
 * @param parent QWidget parent, typically null
 */
CFortEdit::CFortEdit(QWidget *parent) :
    QMainWindow(parent),
    DUi(new Ui::FortEdit),
    DActiveTool(0),
    DTerrainTool(0),
    DTerritoryTool(0),
    DObjectTool(0),
    DSelectTool(0),
    DSoundHandler(this),
    DLuaEditor(0)
{
    DUi->setupUi(this);
    DGraphicsScene = new QGraphicsScene();
    DUi->GraphicsView->setMouseTracking(true);
    DUi->GraphicsView->setScene(DGraphicsScene);
    DUi->GraphicsView->setUpdatesEnabled(true);

    // Ui connectivity / setup
    QObject::connect(DUi->ActionOpen, SIGNAL(triggered()), this, SLOT(OpenMap()));
    QObject::connect(DUi->ActionSaveAs, SIGNAL(triggered()), this, SLOT(SaveMapAs()));
    QObject::connect(DUi->ActionSave, SIGNAL(triggered()), this, SLOT(SaveMap()));
    QObject::connect(DUi->ActionNew, SIGNAL(triggered()), this, SLOT(NewMap()));
    QObject::connect(DUi->MapNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(UpdateSceneTitle(QString)));
    QObject::connect(DUi->MapWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateSceneWidth(int)));
    QObject::connect(DUi->MapHeightSpinBox, SIGNAL(valueChanged(int)), this, SLOT(UpdateSceneHeight(int)));
    QObject::connect(DUi->ObjectVisibilityToggle, SIGNAL(toggled(bool)), this, SLOT(UpdateSceneObjectVisibility(bool)));
    QObject::connect(DUi->GridCheckBox, SIGNAL(toggled(bool)), this, SLOT(UpdateSceneGridVisibility(bool)));
    QObject::connect(DUi->DisplayModeCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(UpdateSceneDisplayMode(QString)));

    QObject::connect(DUi->ToolTerrain, SIGNAL(clicked()), this, SLOT(UpdateActiveTool()));
    QObject::connect(DUi->ToolTerritory, SIGNAL(clicked()), this, SLOT(UpdateActiveTool()));
    QObject::connect(DUi->ToolObject, SIGNAL(clicked()), this, SLOT(UpdateActiveTool()));
    QObject::connect(DUi->ToolSelect, SIGNAL(clicked()), this, SLOT(UpdateActiveTool()));

    QObject::connect(DUi->AudioOptions, SIGNAL(clicked()), this, SLOT(UpdateActiveTool()));
    QObject::connect(DUi->ActionSaveSoundSet, SIGNAL(triggered()), this, SLOT(SaveSoundSet()));
    QObject::connect(DUi->ActionLoadSoundSet, SIGNAL(triggered()), &DSoundHandler, SLOT(LoadSoundSet()));

    QObject::connect(DUi->LuaEditorButton, SIGNAL(clicked()), this, SLOT(ToggleLuaEditor()));

    QObject::connect(DUi->PlayButton, SIGNAL(clicked()), this, SLOT(InvokeEngineDialog()));

    // Invocation dialog connectivity
    DInvocationDialog = new QDialog;
    DInvocationUi.setupUi(DInvocationDialog);

    LoadSettings();
    DInvocationUi.executablePath->setText(DEnginePath);
    DInvocationUi.modeCombo->setCurrentIndex(DInvocationMode);
    DInvocationUi.aiCombo->setCurrentIndex(DInvocationDifficulty);
    DInvocationUi.castleIndexCombo->setCurrentIndex(DInvocationIndex);
    DInvocationUi.windCheck->setChecked(DInvocationWind);

    QObject::connect(DInvocationUi.locateButton, SIGNAL(clicked()), this, SLOT(LocateEngine()));
    QObject::connect(DInvocationUi.modeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateInvocationConfig()));
    QObject::connect(DInvocationUi.aiCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateInvocationConfig()));
    QObject::connect(DInvocationUi.castleIndexCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(UpdateInvocationConfig()));
    QObject::connect(DInvocationUi.windCheck, SIGNAL(stateChanged(int)), this, SLOT(UpdateInvocationConfig()));
    QObject::connect(DInvocationUi.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(InvokeEngine(QAbstractButton*)));

    // TODO: Make sure this order is consistent... This is pretty bad as it stands
    DUi->PropertyPageStack->addWidget(DTerrainTool.GetPropertiesWidget());
    DUi->PropertyPageStack->addWidget(DTerritoryTool.GetPropertiesWidget());
    DUi->PropertyPageStack->addWidget(DSelectTool.GetPropertiesWidget());
    DUi->PropertyPageStack->addWidget(DSoundHandler.GetPropertiesWidget());
    DUi->PropertyPageStack->addWidget(DObjectTool.GetPropertiesWidget());

    DScene = new CScene(":/default-maps/res/new.map", DGraphicsScene, &DSoundHandler);
    DUi->ActionSave->setEnabled(false);
    UpdateUi();
}

CFortEdit::~CFortEdit()
{
    delete DUi;
    if (DScene)
    {
        delete DScene;
    }

    SaveSettings();
}

/**
 * Updates the window title and map properties if a {@link CScene} is loaded
 */
void CFortEdit::UpdateUi() {
    if (!DScene)
    {
        return;
    }

    UpdateWindowTitle();
    UpdateMapProperties();
}

/**
 * Updates the window title with the currently loaded {@link CScene}'s title, or "FortEdit" otherwise
 */
void CFortEdit::UpdateWindowTitle()
{
    if (DScene)
    {
        this->setWindowTitle(QString("FortEdit - %1" ).arg(DScene->GetTitle()));
    }
    else
    {
        this->setWindowTitle(QString("FortEdit"));
    }
}

/**
 * Updates the title, width and height of the currently loaded {@link CScene} with the values from the respective UI elements
 */
void CFortEdit::UpdateMapProperties()
{
    if (!DScene)
    {
        DUi->MapNameLineEdit->setText("");
        return;
    }

    DUi->MapNameLineEdit->setText(DScene->GetTitle());
    DUi->MapWidthSpinBox->setValue(DScene->GetWidth());
    DUi->MapHeightSpinBox->setValue(DScene->GetHeight());
}

/**
 * Clears all child widgets from a given QLayout
 * @param layout QLayout to remove children from
 */
void CFortEdit::ClearLayout(QLayout* layout)
{
    QLayoutItem* Child;
    while ((Child = layout->takeAt(0)) != 0)
    {
        if (Child->layout())
        {
            ClearLayout(Child->layout());
        }

        if (Child->widget())
        {
            layout->removeWidget(Child->widget());
        }

        layout->removeItem(Child);
    }
}

void CFortEdit::LoadSettings()
{
    QSettings Settings(QDir::current().absoluteFilePath("fortnitta.ini"), QSettings::NativeFormat);

    DEnginePath = Settings.value(INVOCATION_TARGET_KEY, "<none>").toString();
    DInvocationMode = Settings.value(INVOCATION_MODE_KEY, 0).toInt();
    DInvocationDifficulty = Settings.value(INVOCATION_AI_DIFFICULTY_KEY, 0).toInt();
    DInvocationWind = Settings.value(INVOCATION_WIND_KEY, false).toBool();
    DInvocationIndex = Settings.value(INVOCATION_INDEX_KEY, -1).toInt();
}

void CFortEdit::SaveSettings()
{
    QSettings Settings(QDir::current().absoluteFilePath("fortnitta.ini"), QSettings::NativeFormat);
    Settings.setValue(INVOCATION_TARGET_KEY, DEnginePath);
    Settings.setValue(INVOCATION_MODE_KEY, DInvocationMode);
    Settings.setValue(INVOCATION_AI_DIFFICULTY_KEY, DInvocationDifficulty);
    Settings.setValue(INVOCATION_WIND_KEY, DInvocationWind);
    Settings.setValue(INVOCATION_INDEX_KEY, DInvocationIndex);
}

void CFortEdit::InvokeEngineDialog()
{
    DInvocationUi.castleIndexCombo->clear();
    DInvocationUi.castleIndexCombo->addItem("<none>");
    for (int i = 0; i < DScene->GetNumCastles(); ++i)
    {
        DInvocationUi.castleIndexCombo->addItem(QString::number(i + 1));
    }

    DInvocationDialog->show();
}

void CFortEdit::InvokeEngine(QAbstractButton* button)
{
    if (DInvocationUi.buttonBox->button(QDialogButtonBox::Ok) == button)
    {
        // Save the current map into a temp map file
        QString MapPath = QDir::current().absoluteFilePath("temp.map");
        QFile outputFile(MapPath);
        try
        {
            DScene->SetAiPath(QDir::current().absoluteFilePath("temp.ai"));
            QString MapData = DScene->WriteToFile();

            outputFile.open(QIODevice::WriteOnly);
            if (!outputFile.isOpen()){
                QMessageBox* Msg = new QMessageBox;
                Msg->setText(QString("Error saving! %1,\nCan't open the file. Is it in use by another program?" ).arg(MapPath));
                Msg->show();
            } else {
                QTextStream outStream(&outputFile);
                outStream << MapData;
                outputFile.close();
            }
        }
        catch (CMapException& E)
        {
            QMessageBox* Msg = new QMessageBox;
            Msg->setText(QString("Error saving! %1,\n%2" ).arg(MapPath).arg(E.Message()));
            Msg->show();
            return;
        }

        QStringList Arg;
        Arg << "-dev ";
        Arg << "-mode " + DInvocationUi.modeCombo->itemText(DInvocationMode).toUpper();
        Arg << "-map " + MapPath;
        Arg << "-ai " + DInvocationUi.aiCombo->itemText(DInvocationDifficulty).toUpper();
        Arg <<  (DInvocationWind ? "-wind ON" : "-wind OFF");

        if (DInvocationIndex > 0)
        {
            Arg << "-castle " << QString::number(DInvocationIndex);
        }

        qDebug() << Arg;

        QProcess* FortNitta = new QProcess(0);
        FortNitta->start(DEnginePath, Arg);
    }
}

void CFortEdit::LocateEngine()
{
    DEnginePath = QFileDialog::getOpenFileName(0,
                                  "Locate the FortNitta Executable",
                                  QDir::currentPath());

    if (DEnginePath.length() > 0)
    {
        DInvocationUi.executablePath->setText(DEnginePath);
    }
}

void CFortEdit::UpdateInvocationConfig()
{
    DInvocationMode = DInvocationUi.modeCombo->currentIndex();
    DInvocationDifficulty = DInvocationUi.aiCombo->currentIndex();
    DInvocationWind = DInvocationUi.windCheck->isChecked();
    DInvocationIndex = DInvocationUi.castleIndexCombo->currentIndex();
}

/**
 * Sets the current active tool based on the tool button that was clicked in the UI
 */
void CFortEdit::UpdateActiveTool()
{
    // Get rid of the handle from the previous active tool
    if (DActiveTool)
    {
        DActiveTool->UpdateTarget(0);
    }

    if (!DScene)
    {
        return;
    }

    qDebug() << QObject::sender()->objectName();

    int Index = 0;
    if (QObject::sender()->objectName() == "ToolTerrain")
    {
        DActiveTool = &DTerrainTool;
        Index = 1;
        DScene->SetTerritoryVisiblity(false);
        DUi->EditorTabs->setTabText(0, "Terrain Tool");
    }
    else if (QObject::sender()->objectName() == "ToolTerritory")
    {
        DActiveTool = &DTerritoryTool;
        Index = 2;
        DScene->SetTerritoryVisiblity(true);
        DUi->EditorTabs->setTabText(0, "Territory Tool");
    }
    else if (QObject::sender()->objectName() == "ToolObject")
    {
        DActiveTool = &DObjectTool;
        Index = 5;
        DUi->ObjectVisibilityToggle->setChecked(true);
        DScene->SetObjectVisibility(true);
        DScene->SetTerritoryVisiblity(false);
        DUi->EditorTabs->setTabText(0, "Object Placement Tool");
    }
    else if (QObject::sender()->objectName() == "ToolSelect")
    {
        DActiveTool = &DSelectTool;
        Index = 3;
        DUi->ObjectVisibilityToggle->setChecked(true);
        DScene->SetObjectVisibility(true);
        DScene->SetTerritoryVisiblity(false);
        DUi->EditorTabs->setTabText(0, "Select Tool");
    }
    else if (QObject::sender()->objectName() == "AudioOptions")
    {
        Index = 4;
        DUi->EditorTabs->setTabText(0, "Audio Select");
    }
    else
    {
        DActiveTool = 0;
        DScene->SetTerritoryVisiblity(false);
    }

    // Connect the new active tool to the scene
    if (DActiveTool)
    {
        DActiveTool->UpdateTarget(DScene);
    }

    DUi->PropertyPageStack->setCurrentIndex(Index);
}

/**
 * Updates the current {@link CScene} title
 * @param title new title for the {@link CScene}
 */
void CFortEdit::UpdateSceneTitle(const QString& title)
{
    if (DScene)
    {
        DScene->SetTitle(title);
        UpdateWindowTitle();
    }
}

/**
 * Updates the current {@link CScene} width
 * @param width new width for the {@link CScene}
 */
void CFortEdit::UpdateSceneWidth(const int& width)
{
    if (DScene)
    {
        DScene->SetWidth(width);
    }
}

/**
 * Updates the current {@link CScene} height
 * @param height new height for the {@link CScene}
 */
void CFortEdit::UpdateSceneHeight(const int& height)
{
    if (DScene)
    {
        DScene->SetHeight(height);
    }
}

/**
 * Toggles between showing objects on the {@link CScene} or not
 * @param value true/false to show objects on the {@link CScene}
 */
void CFortEdit::UpdateSceneObjectVisibility(const bool& value)
{
    if (DScene)
    {
        DScene->SetObjectVisibility(value);
    }
}

/**
 * Toggles the grid visibility
 * @param value true/false for grid visibility
 */
void CFortEdit::UpdateSceneGridVisibility(const bool& value)
{
    if (DScene)
    {
        DScene->SetGridVisible(value);
    }
}

/**
 * Changes the current display mode of the current {@link CScene}
 * @param mode String representation of the new display mode, one of "2D", "3D"
 */
void CFortEdit::UpdateSceneDisplayMode(const QString& mode)
{
    if (DScene)
    {
        if (mode == "2D")
        {
            DScene->SetDisplayMode(CScene::displayMode2D);
        }
        else if (mode == "3D")
        {
            DScene->SetDisplayMode(CScene::displayMode3D);
        }
        else
        {
            // TODO: "real" 3D mode, if time
        }
    }
}

/**
 * Toggles the visibility of the Lua script editor window
 */
void CFortEdit::ToggleLuaEditor()
{
    DLuaEditor.show();
}

/**
 * Opens a file dialog requesting a .map path from the user, and subsequently loads it into a {@link CScene}
 */
void CFortEdit::OpenMap()
{
    DMapFilePath = QFileDialog::getOpenFileName(0,
                                  "Choose a map file",
                                  QDir::currentPath(),
                                  "FortNitta Map Files (*.map)");

    if (DMapFilePath.length() > 0)
    {
        if (DScene)
        {
            delete DScene;
        }

        DScene = new CScene(DMapFilePath, DGraphicsScene, &DSoundHandler);
        DLuaEditor.LoadFile(DScene->GetAiPath());
        UpdateUi();
        DScene->Invalidate();
        DUi->ActionSave->setEnabled(true);
    }
}

/**
 * Opens a file dialog asking the user for a path to save the current map to, or saves the current map if already exists on disk
 */
void CFortEdit::SaveMap()
{
    QFile outputFile(DMapFilePath);
    try
    {
        DScene->SetAiPath(DLuaEditor.SaveFile(DMapFilePath, DScene->Title()));
        QString MapData = DScene->WriteToFile();

        outputFile.open(QIODevice::WriteOnly);
        if (!outputFile.isOpen()){
            QMessageBox* Msg = new QMessageBox;
            Msg->setText(QString("Error saving! %1,\nCan't open the file. Is it in use by another program?" ).arg(DMapFilePath));
            Msg->show();
        } else {
            QTextStream outStream(&outputFile);
            outStream << MapData;
            outputFile.close();
        }
    }
    catch (CMapException& E)
    {
        QMessageBox* Msg = new QMessageBox;
        Msg->setText(QString("Error saving! %1,\n%2" ).arg(DMapFilePath).arg(E.Message()));
        Msg->show();
    }
}

/**
 * Opens a file dialog asking the user for a path to save the current map to
 */
void CFortEdit::SaveMapAs()
{
    DMapFilePath = QFileDialog::getSaveFileName(0,
                                  "Choose a destination",
                                  QDir::currentPath(),
                                  "FortNitta Map Files (*.map)");

    if (DMapFilePath.length() > 0)
    {
        // Save map to path
        QFile outputFile(DMapFilePath);
        try
        {
            DScene->SetAiPath(DLuaEditor.SaveFile(DMapFilePath, DScene->Title()));
            QString MapData = DScene->WriteToFile();

            outputFile.open(QIODevice::WriteOnly);
            if (!outputFile.isOpen()){
                QMessageBox* Msg = new QMessageBox;
                Msg->setText(QString("Error saving! %1,\nCan't open the file. Is it in use by another program?" ).arg(DMapFilePath));
                Msg->show();
            } else {
                QTextStream outStream(&outputFile);
                outStream << MapData;
                outputFile.close();
            }
            DUi->ActionSave->setEnabled(true);

        }
        catch (CMapException& E)
        {
            QMessageBox* Msg = new QMessageBox;
            Msg->setText(QString("Error saving! %1,\n%2" ).arg(DMapFilePath).arg(E.Message()));
            Msg->show();
        }
    }
}

/**
 * Sets the current {@link CScene} to the default "new" scene
 */
void CFortEdit::NewMap()
{
    bool New = true;
    if (DScene)
    {
        QMessageBox* Alert = new QMessageBox;
        Alert->setText("A map is already open");
        Alert->setInformativeText("Do you want to close the current map and create a new one?");
        Alert->setStandardButtons(QMessageBox::Yes |
                                   QMessageBox::No);
        Alert->setDefaultButton(QMessageBox::No);
        int Response = Alert->exec();
        New = Response == QMessageBox::Yes;
    }

    if (New)
    {
        if (DScene)
        {
            delete DScene;
        }

        DSoundHandler.LoadDefaultFiles();
        DScene = new CScene(":/default-maps/res/new.map", DGraphicsScene, &DSoundHandler);
        UpdateUi();
        DLuaEditor.clear();
        DUi->ActionSave->setEnabled(false);
        DMapFilePath = "";
    }
}

/**
 * Saves the current sound set configuration to a .dat file.
 * @return true if set saved succesfully, else false.
 */
bool CFortEdit::SaveSoundSet()
{
    QString destination = QFileDialog::getSaveFileName(0,
                                  "Choose a destination",
                                  QDir::currentPath(),
                                  "FortNitta Sound Set (*.dat)");

    return DSoundHandler.CreateSoundSet(destination);
}

/**
 * Returns the save path of the current map. Utilized by the DSoundHandler
 * to save sound sets.
 * @return Copy of DMapFilePath.
 */
QString CFortEdit::MapFilePath() const
{
    return DMapFilePath;
}
