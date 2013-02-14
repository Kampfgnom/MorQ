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
        package->setParent(this);
        m_packageRows.insert(package, m_packagesCache.size());
        m_packagesCache.append(package);
    }
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
    static double progress = 0.0;
    static QByteArray captchaData;

    if(!index.parent().isValid()) {
        DownloadPackage *package = m_packagesCache.at(index.row());

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case NameColumn:
                return package->name();

            case ProgressColumn:
                progress = qAbs(100.0 * package->progress()); // ensure, that it is positive. Damn doubles
                return QString("%1 %").arg(QString::number(progress, 'f', 2));

            case SpeedColumn:
                speed = double(package->speed() / 1000.0);
                if(qFuzzyIsNull(speed))
                    return QVariant();

                return QString("%1 KB/s").arg(QString::number(speed,'f',2));

            case SpeedWeightedColumn:
                weightedSpeed = double(package->speedWeighted() / 1000.0);
                if(qFuzzyIsNull(speed))
                    return QVariant();

                return QString("%1 KB/s").arg(QString::number(weightedSpeed,'f',2));

            case EtaColumn:
                return package->eta().toString("hh:mm:ss");

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
                progress = qAbs(100.0 * dl->progress()); // ensure, that it is positive. Damn doubles
                return QString("%1 %").arg(QString::number(progress, 'f', 2));

            case SpeedColumn:
                speed = double(dl->speed() / 1000.0);
                if(qFuzzyIsNull(speed))
                    return QVariant();

                return QString("%1 KB/s").arg(QString::number(speed,'f',2));

            case SpeedWeightedColumn:
                weightedSpeed = double(dl->speedWeighted() / 1000.0);
                if(qFuzzyIsNull(speed))
                    return QVariant();

                return QString("%1 KB/s").arg(QString::number(weightedSpeed,'f',2));

            case EtaColumn:
                return dl->eta().toString("hh:mm:ss");

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

            static QFileIconProvider provider;
            // TODO: This might be slow.
            QTemporaryFile file(QString("XXXXXX").append(dl->fileName())); // XXXXXX so that QTemporaryFile does not append these
                                                                           // which would overwrite the original extension
            file.open(); // Open, because QFileIconProvider only works for existing files... This little fucker
            QFileInfo info(file);
            QIcon icon = provider.icon(info);

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
    return 8;
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
    endInsertRows();
}

void DownloadsItemModel::updatePackage(QObject *object)
{
    DownloadPackage *updatedPackage = static_cast<DownloadPackage *>(object);
    QVariant key = packagePrimaryKey(updatedPackage);
    QMutableListIterator<DownloadPackage *> it(m_packagesCache);
    int i = 0;
    while(it.hasNext()) {
        it.next();
        if(it.value() == object) {
            emit dataChanged(index(i, 0,QModelIndex()),
                             index(i, columnCount(QModelIndex()),QModelIndex()));
            return;
        }

        if(packagePrimaryKey(it.value()) == key) {
            it.remove();
            it.insert(updatedPackage);
            m_packageRows.insert(updatedPackage, i);
            emit dataChanged(index(i, 0,QModelIndex()),
                             index(i, columnCount(QModelIndex()),QModelIndex()));
            return;
        }
        ++i;
    }
}

void DownloadsItemModel::removePackage(QObject *object)
{
    DownloadPackage *removedPackage = static_cast<DownloadPackage *>(object);
    QVariant key = packagePrimaryKey(removedPackage);
    QMutableListIterator<DownloadPackage *> it(m_packagesCache);
    int i = 0;
    while(it.hasNext()) {
        it.next();
        if(it.value() == object
                || packagePrimaryKey(it.value()) == key) {
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

    beginInsertRows(parent, i, i);
    endInsertRows();
    emit dataChanged(index(i,0,parent),
                     index(i,columnCount(parent), parent));
}

void DownloadsItemModel::removeDownload(QObject *object)
{
}

QVariant DownloadsItemModel::packagePrimaryKey(DownloadPackage *package) const
{
    QDataSuite::MetaProperty keyProperty = Controller::downloadPackagesDao()->dataSuiteMetaObject().primaryKeyProperty();
    return keyProperty.read(package);
}

QModelIndex DownloadsItemModel::indexForPackage(DownloadPackage *package) const
{
    return createIndex(m_packageRows.value(package), 0, package);
}

QVariant DownloadsItemModel::downloadPrimaryKey(Download *download) const
{
    QDataSuite::MetaProperty keyProperty = Controller::downloadsDao()->dataSuiteMetaObject().primaryKeyProperty();
    return keyProperty.read(download);
}
