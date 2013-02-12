#include "downloadsitemmodel.h"

#include <download.h>

#include <QDataSuite/abstractdataaccessobject.h>
#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>

#include <QFileIconProvider>
#include <QDebug>
#include <QTime>
#include <QTemporaryFile>

DownloadsItemModel::DownloadsItemModel(QDataSuite::AbstractDataAccessObject *dao, QObject *parent) :
    QAbstractItemModel(parent),
    m_dao(dao)
{
    connect(m_dao, &QDataSuite::AbstractDataAccessObject::objectInserted,
            this, &DownloadsItemModel::insertObject);
    connect(m_dao, &QDataSuite::AbstractDataAccessObject::objectUpdated,
            this, &DownloadsItemModel::updateObject);
    connect(m_dao, &QDataSuite::AbstractDataAccessObject::objectRemoved,
            this, &DownloadsItemModel::removeObject);

    foreach(QObject *object, m_dao->readAllObjects()) {
        Download *download = static_cast<Download *>(object);
        m_cache.append(download);
    }
}

Qt::ItemFlags DownloadsItemModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant DownloadsItemModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    Download *dl = download(index);

    if(!index.parent().isValid()) {
        double speed = 0.0;
        double weightedSpeed = 0.0;
        double progress = 0.0;
        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case FileNameColumn:
                return dl->fileName();

            case ProgressColumn:
                progress = 100.0 * dl->progress() + 0.000001; // ensure, that it is positive
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
            case FileNameColumn:
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
        case FileNameColumn:
            return QVariant("Name");
        case ProgressColumn:
            return QVariant("Progress");
        case SpeedColumn:
            return QVariant("Speed");
        case SpeedWeightedColumn:
            return QVariant("Av.Speed");
        case EtaColumn:
            return QVariant("ETA");
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
        return m_cache.size();

    return 0;
}

int DownloadsItemModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return 7;
}

QModelIndex DownloadsItemModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

QModelIndex DownloadsItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid())
        return QModelIndex();

    return createIndex(row, column);
}

void DownloadsItemModel::insertObject(QObject *object)
{
    beginInsertRows(QModelIndex(), m_cache.size(), m_cache.size());
    Download *insertedDownload = static_cast<Download *>(object);
    m_cache.append(insertedDownload);
    endInsertRows();
}

void DownloadsItemModel::updateObject(QObject *object)
{
    Download *updatedDownload = static_cast<Download *>(object);
    QVariant key = primaryKey(updatedDownload);
    QMutableListIterator<Download *> it(m_cache);
    int i = 0;
    while(it.hasNext()) {
        it.next();
        if(primaryKey(it.value()) == key) {
            it.remove();
            it.insert(updatedDownload);
            emit dataChanged(index(i, 0,QModelIndex()),
                             index(i, columnCount(QModelIndex()),QModelIndex()));
            return;
        }
        ++i;
    }
}

void DownloadsItemModel::removeObject(QObject *object)
{
    Download *removedDownload = static_cast<Download *>(object);
    QVariant key = primaryKey(removedDownload);
    QMutableListIterator<Download *> it(m_cache);
    int i = 0;
    while(it.hasNext()) {
        it.next();
        if(primaryKey(it.value()) == key) {
            beginRemoveRows(QModelIndex(), i, i);
            it.remove();
            endRemoveRows();
            return;
        }
        ++i;
    }
}

Download *DownloadsItemModel::download(const QModelIndex &index) const
{
    return m_cache.at(index.row());
}

QVariant DownloadsItemModel::primaryKey(Download *download) const
{
    QDataSuite::MetaProperty keyProperty = m_dao->dataSuiteMetaObject().primaryKeyProperty();
    return keyProperty.read(download);
}
