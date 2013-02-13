TEMPLATE = subdirs

CONFIG      +=  ordered
SUBDIRS     =   QDataSuite \
                QSerienJunkies \
                Server


QDataSuite.subdir           =   lib/QDataSuite

QSerienJunkies.subdir       =   lib/QSerienJunkies

Server.subdir               =   Server
Server.depends              =   QDataSuite \
                                QSerienJunkies

include(Model/Model.pri)
