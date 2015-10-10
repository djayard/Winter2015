#include <QDebug>
#include <QFileDialog>
#include <QElapsedTimer>
#include "csoundhandler.h"
#include "cfortedit.h"

const QString CSoundHandler::DEFAULT_FILES[] =
{
    "place.mid", "rebuild.mid", "taps.mid", "win.mid",
    "aim2.wav", "cannon0.wav", "cannon1.wav", "ceasefire2.wav",
    "explosion0.wav","explosion1.wav", "explosion2.wav", "explosion3.wav",
    "fire2.wav", "groundexplosion0.wav", "groundexplosion1.wav", "gust2.wav",
    "place.wav", "ready2.wav", "tick.wav", "tock.wav",
    "triumph.wav", "waterexplosion0.wav", "waterexplosion1.wav", "waterexplosion2.wav",
    "waterexplosion3.wav"
};

const QString CSoundHandler::SOUNDS_FOLDER = "sounds/";
const QString CSoundHandler::SETS_FOLDER = "soundSets/";
const QString CSoundHandler::SOUND_FONT = "sounds/generalsoundfont.sf2";

CSoundHandler::CSoundHandler(CFortEdit *parent)
{
    QObject* area;
    QString labelName = "label", playName = "playButton",
            stopName = "stopButton", browseName = "browseButton",
            volumeName = "volume", number;

    DParent = parent;
    DPropertiesWidget = new QWidget();
    DProperties.setupUi(DPropertiesWidget);
    area = DProperties.midiArea;
    bool disableMidi = false;

    DPlayManager = new QSignalMapper(this);
    DVolumeManager = new QSignalMapper(this);
    DBrowseManager = new QSignalMapper(this);

    connect(DPlayManager, SIGNAL(mapped(int)), this, SLOT(Play(int)));
    connect(DVolumeManager, SIGNAL(mapped(int)), this, SLOT(Volume(int)));
    connect(DBrowseManager, SIGNAL(mapped(int)), this, SLOT(SelectFile(int)));

    DFluidSettings = new_fluid_settings();
#ifdef  Q_OS_WIN
    {
        if( !fluid_settings_setstr(DFluidSettings, "audio.driver", "dsound"))
        {
                disableMidi = true;
        }
    }
#elif defined( Q_OS_MAC )
    {
        if( !fluid_settings_setstr(DFluidSettings, "audio.driver", "coreaudio"))
        {
                disableMidi = true;
        }
    }
#elif defined( Q_OS_LINUX )
    {
        if( !fluid_settings_setstr(DFluidSettings, "audio.driver", "alsa"))
        {
                disableMidi = true;
        }
    }
#else
    {
        qDebug << "OS not supported."
        exit(2);
    }
#endif
    DFluidSynthesizer = new_fluid_synth(DFluidSettings);
    DFluidPlayer = new_fluid_player(DFluidSynthesizer);
    DFluidAudioDriver = new_fluid_audio_driver(DFluidSettings, DFluidSynthesizer);
    if( fluid_synth_sfload(DFluidSynthesizer, "sounds/generalsoundfont.sf2", 1) == FLUID_FAILED)
    {
       disableMidi = true;
    }


    for( int i = 0; i < NUM_SOUNDS ; ++i )
    {
        if( area != DProperties.wavArea && i >= NUM_MIDI)
        {
            area = DProperties.wavArea;
        }

        number.setNum(i);
        labelName.append(number);
        playName.append(number);
        stopName.append(number);
        browseName.append(number);
        volumeName.append(number);

        QToolButton *play, *stop, *browse;
        QSlider *volume;

        DLabels[i] = area->findChild<QLabel*>(labelName);
        play = area->findChild<QToolButton*>(playName);
        stop = area->findChild<QToolButton*>(stopName);
        browse = area->findChild<QToolButton*>(browseName);
        DVolume[i] = volume = area->findChild<QSlider*>(volumeName);

        DPlayManager->setMapping(play, i);
        connect(stop, SIGNAL(clicked()), this, SLOT(Stop()));
        DBrowseManager->setMapping(browse, i);
        DVolumeManager->setMapping(volume,i);

        connect(play, SIGNAL(clicked()), DPlayManager, SLOT(map()));
        connect(browse, SIGNAL(clicked()), DBrowseManager, SLOT(map()));
        connect(volume, SIGNAL(valueChanged(int)), DVolumeManager, SLOT(map()));

        labelName.remove(number);
        playName.remove(number);
        stopName.remove(number);
        browseName.remove(number);
        volumeName.remove(number);

        if( disableMidi && i < NUM_MIDI)
        {
            play->setEnabled(false);
            play->setVisible(false);
            stop->setEnabled(false);
            stop->setVisible(false);
        }

    }

    DLastVisitedDirectory = ".";

    DSoundFolder = QDir::current();
    DSoundFolder.mkdir(SOUNDS_FOLDER);
    DSoundFolder.cd(SOUNDS_FOLDER);

    DCurrentlyPlaying = -1;
    DWavFile = 0;

    LoadDefaultFiles();
}

void CSoundHandler::LoadDefaultFiles()
{
    for( int i = 0; i < NUM_SOUNDS; ++i)
    {
        LoadDefaultFile(i);
    }
}

CSoundHandler::~CSoundHandler()
{
    Stop();
    delete DPropertiesWidget;
    delete DPlayManager;
    delete DBrowseManager;
    delete DVolumeManager;


    if( DWavFile )
    {
        delete DWavFile;
    }

    delete_fluid_audio_driver(DFluidAudioDriver);
    delete_fluid_player(DFluidPlayer);
    delete_fluid_synth(DFluidSynthesizer);
    delete_fluid_settings(DFluidSettings);

}

QWidget* CSoundHandler::GetPropertiesWidget()
{
    return DPropertiesWidget;
}

QString CSoundHandler::SelectFile(int id)
{
    Stop();
    QString filename;

    if( id < NUM_MIDI )
    {
        filename = QFileDialog::getOpenFileName(DParent, "Open MIDI", DLastVisitedDirectory, "MIDI (*.mid)");
    }
    else
    {
        filename = QFileDialog::getOpenFileName(DParent, "Open WAV", DLastVisitedDirectory, "WAV (*.wav)");
    }

    if( filename.isEmpty() )
        return filename;

    DLastVisitedDirectory = filename;
    int index = DLastVisitedDirectory.lastIndexOf('/');
    DLastVisitedDirectory.remove(index, 255);

    filename = CopyFile(filename);

    DLabels[id]->setText(filename.section('/', -1, -1));
    DSoundPaths[id] = filename;

    return filename;

}

QString CSoundHandler::CopyFile(QString filename)
{
    //checks if the file was choosen from the SOUNDS_FOLDER
    if( filename.contains(DSoundFolder.absolutePath()) )
        return filename.section('/',-2,-1);

    QString destinationPath = DSoundFolder.absoluteFilePath(filename.section('/',-1,-1));

    QString currSuffix, prevSuffix = ".";
    for( int i = 0; QFile::exists(destinationPath); ++i)
     {
        currSuffix = '_' + QString::number(i) + '.';
        destinationPath.replace(prevSuffix, currSuffix);
        prevSuffix = currSuffix;
     }

    qDebug() << filename << "\n";
    qDebug() << destinationPath << "\n";

    if( !QFile::copy(filename,destinationPath) )
    {
        exit(5);
    }

    return destinationPath.section('/',-2,-1);
}

QString CSoundHandler::CreateSoundSet()
{
    QString savedSetName;
    savedSetName.clear();

    if( !DParent )
    {
        return savedSetName;
    }

    QString mapPath = DParent->MapFilePath();
    mapPath.replace(".map",".dat", Qt::CaseInsensitive);
    DSoundFolder.mkdir(SETS_FOLDER);
    savedSetName = DSoundFolder.absoluteFilePath(SETS_FOLDER + mapPath.section('/',-1,-1));

    if( CreateSoundSet(savedSetName) )
    {
        return savedSetName.section('/',-3,-1);
    }
    else
    {
        savedSetName.clear();
        return savedSetName;
    }
}

bool CSoundHandler::CreateSoundSet(const QString &destinationPath)
{
    Stop();
    QFile file(destinationPath);
    file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);
    QTextStream destination(&file);

    if( destination.status() != QTextStream::Ok )
    {
        file.close();
        return false;
    }

    destination << destinationPath.section("/",-1,-1) << "\n\n";
    int i;
    for( i = 0; i < NUM_MIDI; ++i )
    {
        destination << DSoundPaths[i] << '\n';
    }

    destination << '\n';

    for( ; i < NUM_SOUNDS; ++i )
    {
        destination << DSoundPaths[i] << '\n';
    }

    destination << '\n';

    for( i = 0; i < NUM_SOUNDS; ++i)
    {
        destination << DVolume[i]->value() / 100.0 << ' ';
    }

    destination << '\n';

    file.close();

    return true;
}

bool CSoundHandler::LoadSoundSet()
{
    Stop();
    QString filename = QFileDialog::getOpenFileName(DParent, "Select Sound Set", DSoundFolder.absolutePath(), "SoundSet (*.dat)");
    return LoadSoundSet(filename);
}

bool CSoundHandler::LoadSoundSet(const QString filename)
{
    Stop();
    const short int BUFFER_SIZE = 255;
    QDir workDirectory = QDir::current();

    QFile set(filename);
    set.open(QFile::ReadOnly | QFile::Text);
    if( !set.isReadable())
    {
        qDebug() << "Not readable.";
        return false;
    }
    qDebug() << "Readable";


    int i, charsRead;
    char buffer[BUFFER_SIZE];
    bool loadDefault = false;

    set.readLine(BUFFER_SIZE);
    set.readLine(BUFFER_SIZE);
    for( i = 0; i < NUM_MIDI; ++i )
    {
        loadDefault = true;
        charsRead = set.readLine(buffer, BUFFER_SIZE);
        //if line is not blank
        if( charsRead > 1)
        {
            //trim off '\n'
            buffer[charsRead-1] = '\0';

            //check if file exists
            if( workDirectory.exists(buffer) )
            {
                DSoundPaths[i] = buffer;
                DLabels[i]->setText(DSoundPaths[i].section('/',-1,-1));
                loadDefault = false;
            }
        }

        if( loadDefault )
        {
            LoadDefaultFile(i);
        }
    }

    set.readLine(BUFFER_SIZE);
    for( ; i < NUM_SOUNDS; ++i )
    {
        loadDefault = true;
        charsRead = set.readLine(buffer, BUFFER_SIZE);
        //if line is not blank
        if( charsRead > 1)
        {
            //trim off '\n'
            buffer[charsRead-1] = '\0';

            //check if file exists
            if( workDirectory.exists(buffer) )
            {
                DSoundPaths[i] = buffer;
                DLabels[i]->setText(DSoundPaths[i].section('/',-1,-1));
                loadDefault = false;
            }
        }

        if( loadDefault )
        {
            LoadDefaultFile(i);
        }
    }

    QTextStream stream(&set);
    float volume;
    stream >> volume;
    for( i = 0; i < NUM_SOUNDS && !stream.atEnd();  ++i )
    {
        DVolume[i]->setValue((int)(volume * 100 + 0.5));
        stream >> volume;
    }
    for( ; i < NUM_SOUNDS; ++i )
    {
        DVolume[i]->setValue(100);
    }

    set.close();

    return true;

}

void CSoundHandler::LoadDefaultFile(int id)
{
    if ( DSoundFolder.exists(DEFAULT_FILES[id]) )
    {
        DLabels[id]->setText(DEFAULT_FILES[id]);
        DSoundPaths[id] = SOUNDS_FOLDER + DEFAULT_FILES[id];
    }
    else
    {
        DLabels[id]->setText("Default Not Found.");
        DSoundPaths[id] = "";
    }

    DVolume[id]->setValue(100);
}

bool CSoundHandler::Volume(int id)
{
    if( id == DCurrentlyPlaying )
    {
        if( id >= NUM_MIDI )
        {
            DWavFile->setVolume(DVolume[id]->value() / 100.0);
        }
        else
        {
            fluid_synth_set_gain(DFluidSynthesizer, (0.2 * DVolume[id]->value() / 100.0));
        }

        return true;
    }

    return false;
}

bool CSoundHandler::Play(int id)
{
    if( id >= NUM_SOUNDS )
    {
        return false;
    }
    else if( DSoundPaths[id].isEmpty() )
    {
        return false;
    }

    Stop();

    if( id < NUM_MIDI )
    {
        delete_fluid_player(DFluidPlayer);
        DFluidPlayer = new_fluid_player(DFluidSynthesizer);

        fluid_synth_set_gain(DFluidSynthesizer, (0.2 * DVolume[id]->value() / 100.0));
        fluid_player_add(DFluidPlayer, DSoundPaths[id].toLocal8Bit());
        fluid_player_set_loop(DFluidPlayer, 1);
        DCurrentlyPlaying = id;
        fluid_player_play(DFluidPlayer);

    }
    else
    {
        if( DWavFile )
        {
            delete DWavFile;
        }

        DWavFile = new QSoundEffect(this);
        DWavFile->setSource(QUrl::fromLocalFile(DSoundPaths[id]));

        if( DWavFile->status() == QSoundEffect::Error )
        {
            delete DWavFile;
            DWavFile = 0;
            return false;
        }

        DWavFile->setVolume(DVolume[id]->value()/100.0);
        DCurrentlyPlaying = id;
        DWavFile->play();
    }

    return true;
}

bool CSoundHandler::Stop()
{
    qDebug() << "Stop";
    if( DCurrentlyPlaying < 0 )
    {
        return false;
    }

    if( DWavFile )
    {
        DWavFile->stop();
        while( DWavFile->isPlaying() );
        delete DWavFile;
        DWavFile = 0;
        DCurrentlyPlaying = -1;
    }
    else
    {
        fluid_player_stop(DFluidPlayer);
        fluid_synth_set_gain(DFluidSynthesizer, 0.0);
        DCurrentlyPlaying = -1;
    }

    return true;
}

