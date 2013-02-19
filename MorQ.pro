TEMPLATE = subdirs

CONFIG      +=  ordered
SUBDIRS     =   QDataSuite \
                QSerienJunkies \
                QuunRar \
                Server


QDataSuite.subdir           =   lib/QDataSuite

QSerienJunkies.subdir       =   lib/QSerienJunkies

QuunRar.subdir              =   lib/QuunRar

Server.subdir               =   Server
Server.depends              =   QDataSuite \
                                QSerienJunkies \
                                QuunRar

include(Model/Model.pri)
