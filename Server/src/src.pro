LIBPATH = ../../lib/

QDATASUITE_BASEPATH = $$LIBPATH/QDataSuite
QDATASUITE_PATH = $$QDATASUITE_BASEPATH/QDataSuite
include($$QDATASUITE_PATH/QDataSuite.pri)

QPERSISTENCE_PATH = $$QDATASUITE_BASEPATH/QPersistence
include($$QPERSISTENCE_PATH/QPersistence.pri)

#QRESTSERVER_PATH = $$QDATASUITE_BASEPATH/QRestServer
#include($$QRESTSERVER_PATH/QRestServer.pri)

#QHAL_PATH = $$QRESTSERVER_PATH/lib/QHal
#include($$QHAL_PATH/QHal.pri)

QSERIENJUNKIES_PATH = $$LIBPATH/QSerienJunkies
include($$QSERIENJUNKIES_PATH/QSerienJunkies.pri)

QUUNRAR_PATH = $$LIBPATH/QuunRar
include($$QUUNRAR_PATH/QuunRar.pri)

### General config ###

TARGET          = MorQServer
VERSION         = 0.0.0
TEMPLATE        = app
QT              += sql widgets network xml
CONFIG          += c++11
QMAKE_CXXFLAGS  += $$QDATASUITE_COMMON_QMAKE_CXXFLAGS
DEFINES         += "VERSION=\"$$VERSION\""
LIBS            += -framework AppKit

### QDataSuite ###

INCLUDEPATH     += $$QDATASUITE_INCLUDEPATH
LIBS            += $$QDATASUITE_LIBS


### QPersistence ###

INCLUDEPATH     += $$QPERSISTENCE_INCLUDEPATH
LIBS            += $$QPERSISTENCE_LIBS


### QSerienJunkies ###

LIBS            += $$QSERIENJUNKIES_LIBS
INCLUDEPATH     += $$QSERIENJUNKIES_INCLUDEPATH


#### QuunRar ###

LIBS            += $$QUUNRAR_LIBS
INCLUDEPATH     += $$QUUNRAR_INCLUDEPATH


#### QHttpServer ###

#INCLUDEPATH     += $$QHTTPSERVER_INCLUDEPATH
#LIBS            += $$QHTTPSERVER_LIBS


#### QHal ###

#LIBS            += $$QHAL_LIBS
#INCLUDEPATH     += $$QHAL_INCLUDEPATH


### Files ###

HEADERS += \
    ui/mainwindow/mainwindow.h \
    controller/downloadcontroller.h \
    ui/mainwindow/model/downloadsitemmodel.h \
    controller/controller.h \
    ui/preferences/preferenceswindow.h \
    preferences.h \
    controller/plugincontroller.h \
    plugins/hoster/hosterplugin.h \
    plugins/hoster/premiumizemeplugin.h \
    controller/downloader.h \
    controller/linkscontroller.h \
    plugins/decrypter/decrypterplugin.h \
    plugins/decrypter/serienjunkiesdecrypterplugin.h \
    ui/mainwindow/downloadsitemdelegate.h \
    controller/extractioncontroller.h \
    controller/seriescontroller.h \
    ui/dialogs/newserieswizard.h \
    plugins/downloadProviders/downloadproviderplugin.h \
    plugins/downloadProviders/serienjunkiesproviderplugin.h \
    ui/mainwindow/model/serieslistmodel.h \
    ui/mainwindow/serieslistitemdelegate.h \
    ui/mainwindow/model/seasonslistmodel.h \
    model/download.h \
    model/downloadpackage.h \
    model/videodownloadlink.h \
    model/episode.h \
    model/season.h \
    model/series.h \
    ui/mainwindow/model/episodeslistmodel.h

SOURCES += main.cpp \
    ui/mainwindow/mainwindow.cpp \
    controller/downloadcontroller.cpp \
    ui/mainwindow/model/downloadsitemmodel.cpp \
    controller/controller.cpp \
    ui/preferences/preferenceswindow.cpp \
    preferences.cpp \
    controller/plugincontroller.cpp \
    plugins/hoster/hosterplugin.cpp \
    plugins/hoster/premiumizemeplugin.cpp \
    controller/downloader.cpp \
    controller/linkscontroller.cpp \
    plugins/decrypter/decrypterplugin.cpp \
    plugins/decrypter/serienjunkiesdecrypterplugin.cpp \
    ui/mainwindow/downloadsitemdelegate.cpp \
    controller/extractioncontroller.cpp \
    controller/seriescontroller.cpp \
    ui/dialogs/newserieswizard.cpp \
    plugins/downloadProviders/downloadproviderplugin.cpp \
    plugins/downloadProviders/serienjunkiesproviderplugin.cpp \
    ui/mainwindow/model/serieslistmodel.cpp \
    ui/mainwindow/serieslistitemdelegate.cpp \
    ui/mainwindow/model/seasonslistmodel.cpp \
    model/download.cpp \
    model/downloadpackage.cpp \
    model/videodownloadlink.cpp \
    model/series.cpp \
    model/episode.cpp \
    model/season.cpp \
    ui/mainwindow/model/episodeslistmodel.cpp

FORMS += \
    ui/mainwindow/mainwindow.ui \
    ui/preferences/preferenceswindow.ui \
    ui/dialogs/newserieswizard.ui

RESOURCES += \
    ui/resources/uiresources.qrc

OBJECTIVE_SOURCES +=

