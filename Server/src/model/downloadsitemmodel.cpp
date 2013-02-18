#include "downloadsitemmodel.h"

#include "controller/controller.h"

#include <download.h>
#include <downloadpackage.h>

#include <QDataSuite/abstractdataaccessobject.h>
#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>

#include <QFileIconProvider>
#include <QDebug>
#include <QTime>
#include <QTemporaryFile>

DownloadsItemModel::DownloadsItemModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    connect(Controller::downloadsDao(), &QDataSuite::AbstractDataAccessObject::objectInserted,
            this, &DownloadsItemModel::insertDownload);
    connect(Controller::downloadsDao(), &QDataSuite::AbstractDataAccessObject::objectUpdated,
            this, &DownloadsItemModel::updateDownload);
    connect(Controller::downloadsDao(), &QDataSuite::AbstractDataAccessObject::objectRemoved,
            this, &DownloadsItemModel::removeDownload);

    connect(Controller::downloadPackagesDao(), &QDataSuite::AbstractDataAccessObject::objectInserted,
            this, &DownloadsItemModel::insertPackage);
    connect(Controller::downloadPackagesDao(), &QDataSuite::AbstractDataAccessObject::objectUpdated,
            this, &DownloadsItemModel::updatePackage);
    connect(Controller::downloadPackagesDao(), &QDataSuite::AbstractDataAccessObject::objectRemoved,
            this, &DownloadsItemModel::removePackage);

    foreach(QObject *object, Controller::downloadPackagesDao()->readAllObjects()) {
        DownloadPackage *package = static_cast<DownloadPackage *>(object);
        m_packageRows.insert(package, m_packagesCache.size());
        m_packagesCache.append(package);

        foreach(Download *dl, package->downloads()) {
            m_downloadsCache[dl] = package;
        }
    }
}

DownloadsItemModel::~DownloadsItemModel()
{
}

Qt::ItemFlags DownloadsItemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    if(!index.parent().isValid()
            && index.column() == UserInputColumn
            && !data(index, Qt::DisplayRole).isNull()) {
        return f | Qt::ItemIsEditable;
    }

    return f;
}

QVariant DownloadsItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    static double speed = 0.0;
    static double weightedSpeed = 0.0;
    static QFileIconProvider provider;
    static QByteArray captchaData;
    static QTime eta;

    if(!index.parent().isValid()) {
        DownloadPackage *package = m_packagesCache.at(index.row());

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case NameColumn:
                return package->name();

            case ProgressColumn:
                return package->progress(); // painting percentages is done by item delegate

            case FileSizeColumn:
                return humanReadableSize(package->totalFileSize());

            case DownloadedColumn:
                return humanReadableSize(package->bytesDownloaded());

            case SpeedColumn:
                speed = qAbs(double(package->speed()));
                if(qFuzzyIsNull(speed))
                    return QVariant();

                return QString("%1/s").arg(humanReadableSize(speed));

            case SpeedWeightedColumn:
                weightedSpeed = qAbs(double(package->speedWeighted()));
                if(qFuzzyIsNull(weightedSpeed))
                    return QVariant();

                return QString("%1/s").arg(humanReadableSize(weightedSpeed));

            case EtaColumn:
                eta = package->eta();
                if(eta.isNull())
                    return QVariant();
                if(eta == QTime(0,0,0))
                    return QVariant();

                return eta.toString("hh:mm:ss");

            case MessageColumn:
                return package->message();

            case UserInputColumn:
                captchaData = package->captcha();
                if(captchaData.isEmpty())
                    return QVariant();
                return QPixmap::fromImage(QImage::fromData(captchaData));

            case UrlColumn:
                return package->sourceUrl();

            case RedirectedUrlColumn:
                return QVariant();

            default:
                break;
            }
        }
        else if(role == Qt::DecorationRole) {

            static QFileIconProvider provider;
            static QIcon FOLDERICON = provider.icon(QFileIconProvider::Folder);

            switch(index.column()) {
            case NameColumn:
                return FOLDERICON;

            default:
                break;
            }
        }
    }
    else if(!index.parent().parent().isValid()) {
        DownloadPackage *package = m_packagesCache.at(index.parent().row());
        Download *dl = package->downloads().at(index.row());

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case NameColumn:
                return dl->fileName();

            case ProgressColumn:
                return dl->progress(); // painting percentages is done by item delegate

            case FileSizeColumn:
                return humanReadableSize(dl->fileSize());

            case DownloadedColumn:
                return humanReadableSize(dl->bytesDownloaded());

            case SpeedColumn:
                speed = qAbs(double(dl->speed()));
                if(qFuzzyIsNull(speed))
                    return QVariant();

                return QString("%1/s").arg(humanReadableSize(speed));

            case SpeedWeightedColumn:
                weightedSpeed = qAbs(double(dl->speedWeighted()));
                if(qFuzzyIsNull(weightedSpeed))
                    return QVariant();

                return QString("%1/s").arg(humanReadableSize(weightedSpeed));

            case EtaColumn:
                eta = dl->eta();
                if(eta.isNull())
                    return QVariant();
                if(eta == QTime(0,0,0))
                    return QVariant();

                return eta.toString("hh:mm:ss");

            case MessageColumn:
                return dl->message();

            case UrlColumn:
                return dl->url();

            case RedirectedUrlColumn:
                return dl->redirectedUrl();

            default:
                break;
            }
        }
        else if(role == Qt::DecorationRole) {

            QIcon icon;
            if(m_icons.contains(dl->fileName())) {
                icon = m_icons.value(dl->fileName());
            }
            else {
                QTemporaryFile file(QString("XXXXXX").append(dl->fileName())); // XXXXXX so that QTemporaryFile does not append these
                                                                               // which would overwrite the original extension
                file.open(); // Open, because QFileIconProvider only works for existing files... This little fucker
                QFileInfo info(file);
                icon = provider.icon(info);
                m_icons.insert(dl->fileName(), icon);
            }

            switch(index.column()) {
            case NameColumn:
                return icon;
            default:
                break;
            }
        }
    }

    return QVariant();
}

QVariant DownloadsItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QVariant();

    if(role == Qt::DisplayRole) {
        switch(section) {
        case NameColumn:
            return QVariant("Name");
        case FileSizeColumn:
            return QVariant("Size");
        case DownloadedColumn:
            return QVariant("Downloaded");
        case ProgressColumn:
            return QVariant("Progress");
        case SpeedColumn:
            return QVariant("Speed");
        case SpeedWeightedColumn:
            return QVariant("Av.Speed");
        case EtaColumn:
            return QVariant("ETA");
        case MessageColumn:
            return QVariant("Message");
        case UserInputColumn:
            return QVariant("");
        case UrlColumn:
            return QVariant("Url");
        case RedirectedUrlColumn:
            return QVariant("Redirected");
        default:
            break;
        }
    }

    return QVariant();
}

int DownloadsItemModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return m_packagesCache.size();

    DownloadPackage *package = m_packagesCache.at(parent.row());
    return package->downloads().size();
}

int DownloadsItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 11;
}

bool DownloadsItemModel::hasChildren(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return true;

    DownloadPackage *package = qobject_cast<DownloadPackage *>(static_cast<DownloadPackage *>(parent.internalPointer()));
    if(!package)
        return false;

    return !package->downloads().isEmpty();
}

QModelIndex DownloadsItemModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    DownloadPackage *package = qobject_cast<DownloadPackage *>(static_cast<DownloadPackage *>(child.internalPointer()));
    if(package)
        return QModelIndex();

    Download *download =  qobject_cast<Download *>(static_cast<Download *>(child.internalPointer()));
    if(download) {
        package = download->package();
        return index(m_packageRows.value(package), 0, QModelIndex());
    }

    return QModelIndex();
}

QModelIndex DownloadsItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!parent.isValid())
        return createIndex(row, column, m_packagesCache.at(row));
    else if(parent.isValid()) {
        DownloadPackage *package = m_packagesCache.at(parent.row());
        return createIndex(row, column, package->downloads().at(row));
    }

    return QModelIndex();
}

void DownloadsItemModel::insertPackage(QObject *object)
{
    beginInsertRows(QModelIndex(), m_packagesCache.size(), m_packagesCache.size());
    DownloadPackage *insertedPackage = static_cast<DownloadPackage *>(object);
    m_packageRows.insert(insertedPackage, m_packagesCache.size());
    m_packagesCache.append(insertedPackage);

    foreach(Download *dl, insertedPackage->downloads()) {
        m_downloadsCache[dl] = insertedPackage;
    }
    endInsertRows();
}

void DownloadsItemModel::updatePackage(QObject *object)
{
    QMutableListIterator<DownloadPackage *> it(m_packagesCache);
    int i = 0;
    while(it.hasNext()) {
        it.next();
        if(it.value() == object) {
            emit dataChanged(index(i, 0,QModelIndex()),
                             index(i, columnCount(QModelIndex()),QModelIndex()));
            return;
        }
        ++i;
    }
}

void DownloadsItemModel::removePackage(QObject *object)
{
    QMutableListIterator<DownloadPackage *> it(m_packagesCache);
    int i = 0;
    while(it.hasNext()) {
        it.next();
        if(it.value() == object) {
            beginRemoveRows(QModelIndex(), i, i);
            m_packageRows.remove(it.value());
            it.remove();
            endRemoveRows();
            return;
        }
        ++i;
    }
}

void DownloadsItemModel::insertDownload(QObject *object)
{
    Download *download = static_cast<Download *>(object);

    if(!download->package())
        return;

    int index = download->package()->downloads().indexOf(download);
    if(index < 0)
        return;

    QModelIndex parent = indexForPackage(download->package());

    beginInsertRows(parent, index, index);
    endInsertRows();
}

void DownloadsItemModel::updateDownload(QObject *object)
{
    Download *download = static_cast<Download *>(object);

    if(!download->package())
        return;
    int i = download->package()->downloads().indexOf(download);
    if(i < 0)
        return;

    QModelIndex parent = indexForPackage(download->package());

    if(m_downloadsCache[download] != download->package()) {

        beginInsertRows(parent, i, i);
        endInsertRows();

        return;
    }

    emit dataChanged(index(i,0,parent),
                     index(i,columnCount(parent), parent));
}

void DownloadsItemModel::removeDownload(QObject *object)
{
}

QString DownloadsItemModel::humanReadableSize(qint64 bytes) const
{
    static const int THRESH = 1100;
    if(bytes < 0)
        return QString();

    if(bytes == 0)
        return QString("0.0 MB");

    double result = bytes;
    int i = 0;
    while(result > THRESH
          && i < 4) {
        result /= 1000.0;
        ++i;
    }

    QString unit("B");

    switch(i) {
    case 1:
        unit.prepend("K");
        break;
    case 2:
        unit.prepend("M");
        break;
    case 3:
        unit.prepend("G");
        break;
    case 4:
    default:
        unit.prepend("T");
        break;
    }

    return QString("%1 %2").arg(QString::number(result,'f',2)).arg(unit);
}

QModelIndex DownloadsItemModel::indexForPackage(DownloadPackage *package) const
{
    return createIndex(m_packageRows.value(package), 0, package);
}
