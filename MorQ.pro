TEMPLATE = subdirs

CONFIG      +=  ordered
SUBDIRS     =   QDataSuite \
                QSerienJunkies \
                QShareHosterDownload \
                Server


QDataSuite.subdir           =  lib/QDataSuite

QSerienJunkies.subdir       =  lib/QSerienJunkies

QShareHosterDownload.subdir =  lib/QShareHosterDownload

Server.subdir               =  Server
Server.depends              =  QDataSuite \
                               QSerienJunkies \
                               QShareHosterDownload
