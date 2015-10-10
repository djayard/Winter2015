#ifndef CSOUNDHANDLER_H
#define CSOUNDHANDLER_H

#include <QSignalMapper>
#include <QDir>
#include <QSoundEffect>
#include <fluidsynth.h>

#include "ui_music_options.h"

//Forward declaration needed for use of CFortEdit*
class CFortEdit;

/**
 * Manager class to handle all things related to sounds
 */
class CSoundHandler: public QObject
{
    Q_OBJECT

    public:

        /**
         * Constructor for CSoundHandler. Parent must be non-NULL for LoadSoundSet slot.
         * @param parent Pointer to this object's parent.
         */
        CSoundHandler(CFortEdit *parent = 0);
        ~CSoundHandler();

        /**
         * Returns widget containing the UI of this.
         * @return The property widget for the tool.
         */
        QWidget* GetPropertiesWidget();

        const static int NUM_SOUNDS = 25;
        const static int NUM_MIDI = 4;
        const static QString SOUND_FONT;
        const static QString SOUNDS_FOLDER;

    public slots:
        /**
         * Attempts to load a soundSet dat file. This object's parent must non-NULL.
         * This function causes a QFileDialog to appear.
         * @return true upon success, else false.
         */
        bool LoadSoundSet();

        /**
         * Attempts to load a soundSet dat file. The file's name and location
         * is specified by the filename parameter.
         * @param filename File path to the soundset.
         * @return true upon success, else false.
         */
        bool LoadSoundSet(const QString filename);

        /**
         * Sets the soundSet settings to their default. Called by CSoundHandler(CFortEdit*)
         * and CFortEdit::NewMap().
         */
        void LoadDefaultFiles();

        /**
         * Saves active soundSet as a dat file at a specified destination.
         * @param path The requested name of the dat file.
         * @return true if set saved successfully, else false.
         */
        bool CreateSoundSet(const QString& destinationPath);

        /**
         * Saves active soundSet as a dat file with a title matching
         * the save name of the active map of CSoundHandler::DParent.
         * The file is saved to ./sounds/soundSets/
         * @return The path to the saved file relative to the application's work directory.
         */
        QString CreateSoundSet();

   private slots:
        /**
         * Requests a path to a sound file from the user. The object's parent mus be non-NULL.
         * This slot is only meant to be called by CSoundHandler::DBrowseManager.
         * @param id List index of the sound the user wishes to select.
         * @return The path to the sound file relative to the application's work directory.
         */
        QString SelectFile(int id);

        /**
         * Copies the file indicated by filename to the sounds folder. This slot is meant
         * to only be called by CSoundHandler::SelectFile(int).
         * @param filename Absolute path to the file that is to be copied.
         * @return The path to the sound file relative to the application's work directory.
         */
        QString CopyFile(QString filename);

        /**
         * If the file indicated by id is currently playing, this slot ensures that
         * the playback volume is updated.
         * @param id List index of the sound whose QSlider has changed value.
         * @return true if (id == CSoundHandler::DCurrentlyPlaying), else false.
         */
        bool Volume(int id);

        /**
         * Attempts to start playback of the sound file indicated by id.
         * @param id List index of the sound whose Play button has been clicked.
         * @return true if the file's playback was started, else false.
         */
        bool Play(int id);

        /**
         * Attempts to stop sound playback. This function is called at the start of many
         * other member functions.
         * @return true if a sound's playback was interrupted, else false.
         */
        bool Stop();


    private:
        /**
         * Assister function to CSoundHandler::LoadDefaultFiles() and
         * CSoundHandler::LoadSoundSet().
         */
        void LoadDefaultFile(int id);


        CFortEdit* DParent;
        QWidget* DPropertiesWidget;
        QSignalMapper *DBrowseManager, *DPlayManager, *DVolumeManager;
        Ui::MusicProperties DProperties;
        QString DLastVisitedDirectory;

        QDir    DSoundFolder;
        QLabel* DLabels[NUM_SOUNDS];
        QSlider* DVolume[NUM_SOUNDS];
        QString DSoundPaths[NUM_SOUNDS];
        QSoundEffect* DWavFile;

        int DCurrentlyPlaying;
        const static QString DEFAULT_FILES[NUM_SOUNDS];
        const static QString SETS_FOLDER;


        //Copied from CSoundLibrary.cpp in Linux team sources
        fluid_settings_t* DFluidSettings; //!< Fluid settings
        fluid_synth_t* DFluidSynthesizer; //!< Fluid synthesizer
        fluid_player_t* DFluidPlayer; //!< Fluid player
        fluid_audio_driver_t* DFluidAudioDriver; //!< Fluid audio driver


};

#endif // CSOUNDHANDLER_H
