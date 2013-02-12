LIBPATH = ../../lib/

QDATASUITE_BASEPATH = $$LIBPATH/QDataSuite
QDATASUITE_PATH = $$QDATASUITE_BASEPATH/QDataSuite
include($$QDATASUITE_PATH/QDataSuite.pri)

QPERSISTENCE_PATH = $$QDATASUITE_BASEPATH/QPersistence
include($$QPERSISTENCE_PATH/QPersistence.pri)

QRESTSERVER_PATH = $$QDATASUITE_BASEPATH/QRestServer
include($$QRESTSERVER_PATH/QRestServer.pri)

QHAL_PATH = $$QRESTSERVER_PATH/lib/QHal
include($$QHAL_PATH/QHal.pri)

QSHAREHOSTERDOWNLOAD_PATH = $$LIBPATH/QShareHosterDownload
include($$QSHAREHOSTERDOWNLOAD_PATH/QShareHosterDownload.pri)

QSERIENJUNKIES_PATH = $$LIBPATH/QSerienJunkies
include($$QSERIENJUNKIES_PATH/QSerienJunkies.pri)

include(../../Model/Model.pri)

### General config ###

TARGET          = MorQServer
VERSION         = 0.0.0
TEMPLATE        = app
QT              += sql widgets network xml
CONFIG          += c++11
QMAKE_CXXFLAGS  += $$QDATASUITE_COMMON_QMAKE_CXXFLAGS
DEFINES         += "VERSION=\"$$VERSION\""

### QDataSuite ###

INCLUDEPATH     += $$QDATASUITE_INCLUDEPATH
LIBS            += $$QDATASUITE_LIBS


### QPersistence ###

INCLUDEPATH     += $$QPERSISTENCE_INCLUDEPATH
LIBS            += $$QPERSISTENCE_LIBS


### QShareHosterDownload ###

LIBS            += $$QSHAREHOSTERDOWNLOAD_LIBS
INCLUDEPATH     += $$QSHAREHOSTERDOWNLOAD_INCLUDEPATH


### QSerienJunkies ###

LIBS            += $$QSERIENJUNKIES_LIBS
INCLUDEPATH     += $$QSERIENJUNKIES_INCLUDEPATH


### QHttpServer ###

INCLUDEPATH     += $$QHTTPSERVER_INCLUDEPATH
LIBS            += $$QHTTPSERVER_LIBS


### QHal ###

LIBS            += $$QHAL_LIBS
INCLUDEPATH     += $$QHAL_INCLUDEPATH


### Files ###

HEADERS += \
    ui/mainwindow/mainwindow.h \
    controller/downloadcontroller.h \
    model/downloadsitemmodel.h \
    ui/mainwindow/downloadspage.h \
    controller/controller.h \
    ui/preferences/preferenceswindow.h \
    preferences.h

SOURCES += main.cpp \
    ui/mainwindow/mainwindow.cpp \
    controller/downloadcontroller.cpp \
    model/downloadsitemmodel.cpp \
    ui/mainwindow/downloadspage.cpp \
    controller/controller.cpp \
    ui/preferences/preferenceswindow.cpp \
    preferences.cpp

FORMS += \
    ui/mainwindow/mainwindow.ui \
    ui/mainwindow/downloadspage.ui \
    ui/preferences/preferenceswindow.ui

RESOURCES += \
    ui/resources/uiresources.qrc

