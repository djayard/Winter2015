#-------------------------------------------------
#
# Project created by QtCreator 2015-01-14T08:41:13
#
#-------------------------------------------------

QT       += core gui multimedia widgets

TARGET = fortedit
TEMPLATE = app

SOURCES += main.cpp \
    cscene.cpp \
    ctileset.cpp \
    cfortedit.cpp \
    ctool.cpp \
    cterraintool.cpp \
    cterritorytool.cpp \
    cselecttool.cpp \
    c3dtileset.cpp \
    csoundhandler.cpp \
    cobjecttool.cpp \
    cluaeditor.cpp \
    cwalltileset.cpp \
    c3dwalltileset.cpp

HEADERS  += \
    cscene.h \
    cparseexception.h \
    ctileset.h \
    cfortedit.h \
    ctool.h \
    cterraintool.h \
    cterritorytool.h \
    cmapexception.h \
    cselecttool.h \
    c3dtileset.h \
    csoundhandler.h \
    cobjecttool.h \
    cluaeditor.h \
    cwalltileset.h \
    c3dwalltileset.h \
    Binary.h

FORMS    += fortedit.ui \
    terrain_tool_props.ui \
    territory_tool_props.ui \
    select_tool_props.ui \
    music_options.ui \
    object_tool_props.ui \
    engine_invocation_dialog.ui

RESOURCES += \
    resources.qrc

unix {
    INCLUDEPATH += /usr/local/include
    LIBS += -L/usr/local/lib -lfluidsynth
}

win32 {
    INCLUDEPATH += "/Program Files (x86)/FluidSynth/include"
    LIBS += -L"/Program Files (x86)/FluidSynth/bin" -lfluidsynth
}
