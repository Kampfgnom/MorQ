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
        insertPackage(object);
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
        DownloadPackage *package = packageByIndex(index);
        if(!package)
            return QVariant();

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
        Download *dl = downloadByIndex(index);

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case NameColumn:
                return dl->fileName();

            case ProgressColumn:
                return dl->downloadProgress(); // painting percentages is done by item delegate

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
        return m_packages.size();

    DownloadPackage *package = packageByIndex(parent);
    if(!package)
        return 0;

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

    DownloadPackage *package = packageByIndex(parent);
    if(!package)
        return false;

    return !package->downloads().isEmpty();
}

QModelIndex DownloadsItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    if(parent.isValid()) {
        DownloadPackage *package = packageByIndex(parent);
        return createIndex(row, column, package->downloads().at(row));
    }

    DownloadPackage *package = m_packages.at(row);
    return createIndex(row, column, package);
}

QModelIndex DownloadsItemModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    DownloadPackage *package = packageByIndex(child);
    if(package)
        return QModelIndex();

    Download *download = downloadByIndex(child);
    if(!download)
        return QModelIndex();

    package = download->package();
    if(!package || !m_packageRows.contains(package))
        return QModelIndex();

    return createIndex(m_packageRows.value(package), 0, package);
}

Download *DownloadsItemModel::downloadByIndex(const QModelIndex &index) const
{
    Download *download = static_cast<Download *>(index.internalPointer());
    if(!m_downloadRows.contains(download))
        return nullptr;

    return download;
}

DownloadPackage *DownloadsItemModel::packageByIndex(const QModelIndex &index) const
{
    DownloadPackage *package = static_cast<DownloadPackage *>(index.internalPointer());
    if(!m_packageRows.contains(package))
        return nullptr;

    return package;
}

void DownloadsItemModel::insertPackage(QObject *object)
{
    beginInsertRows(QModelIndex(), m_packages.size(), m_packages.size());

    DownloadPackage *package = static_cast<DownloadPackage *>(object);
    _insertPackage(package);

    endInsertRows();
}

void DownloadsItemModel::_insertPackage(DownloadPackage *package)
{
    m_packageRows.insert(package, m_packages.size());
    m_packages.append(package);

    foreach(Download *dl, package->downloads()) {
        _insertDownload(dl);
    }
}

void DownloadsItemModel::updatePackage(QObject *object)
{
    DownloadPackage *package = static_cast<DownloadPackage *>(object);
    if(!m_packageRows.contains(package))
        return;

    int i = m_packageRows[package];
    emit dataChanged(index(i, 0),
                     index(i, columnCount()));

    foreach(Download *dl, package->downloads()) {
        _removeDownload(dl);
        _insertDownload(dl);
    }
}

void DownloadsItemModel::removePackage(QObject *object)
{
    DownloadPackage *package = static_cast<DownloadPackage *>(object);
    if(!m_packageRows.contains(package))
        return;

    int index = m_packageRows[package];

    beginRemoveRows(QModelIndex(), index, index);

    m_packages.removeAt(index);

    int i = 0;
    foreach(DownloadPackage *p, m_packages) {
        m_packageRows.remove(p);
        m_packageRows.insert(p, i);
        ++i;
    }

    foreach(Download *dl, package->downloads()) {
        _removeDownload(dl);
    }
    endRemoveRows();
}

void DownloadsItemModel::insertDownload(QObject *object)
{
    Download *download = static_cast<Download *>(object);
    if(!download || !download->package())
        return;

    QModelIndex parent = indexForPackage(download->package());
    int index = download->order();

    beginInsertRows(parent, index, index);

    _insertDownload(download);

    endInsertRows();
}

void DownloadsItemModel::_insertDownload(Download *download)
{
    m_downloadRows.insert(download, download->order());
}

void DownloadsItemModel::updateDownload(QObject *object)
{
    Download *download = static_cast<Download *>(object);
    QModelIndex parent = indexForPackage(download->package());

    if(!m_downloadRows.contains(download)) {
        int index = download->order();
        beginInsertRows(parent, index, index);
        _insertDownload(download);
        endInsertRows();
    }
    else {
        int i = m_downloadRows[download];

        emit dataChanged(index(i, 0, parent),
                         index(i, columnCount(), parent));
    }
}

void DownloadsItemModel::removeDownload(QObject *object)
{
    Download *download = static_cast<Download *>(object);
    QModelIndex parent = indexForPackage(download->package());
    int i = m_downloadRows[download];

    beginRemoveRows(parent, i, i);

    _removeDownload(download);

    endRemoveRows();
}

void DownloadsItemModel::_removeDownload(Download *download)
{
    int i = 0;
    foreach(Download *dl, download->package()->downloads()) {
        m_downloadRows.remove(dl);
        if(dl != download) {
            m_downloadRows.insert(dl, i);
            ++i;
        }
    }
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
    if(!package)
        return QModelIndex();

    return createIndex(m_packageRows.value(package), 0, package);
}
