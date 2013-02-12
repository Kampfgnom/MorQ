#include "downloadsitemmodel.h"

#include <download.h>

#include <QDataSuite/abstractdataaccessobject.h>
#include <QDataSuite/metaobject.h>
#include <QDataSuite/metaproperty.h>

#include <QDebug>
#include <QTime>

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
        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case FileNameColumn:
                return dl->fileName();
            case ProgressColumn:
                return QString("%1 %")
                        .arg(QString::number(100.0 * float(dl->bytesDownloaded()) / float(dl->fileSize()),
                                             'f',
                                             2));
            case SpeedColumn:
                return QString("%1 KB/s")
                        .arg(QString::number(float(dl->speed()) / 1000.0,
                                             'f',
                                             2));
            case SpeedWeightedColumn:
                return QString("%1 KB/s")
                        .arg(QString::number(float(dl->speedWeighted()) / 1000.0,
                                             'f',
                                             2));
            case EtaColumn:
                return dl->eta().toString("hh:mm:ss");
            case UrlColumn:
                return dl->url();
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
    return 6;
}

QModelIndex DownloadsItemModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QModelIndex DownloadsItemModel::index(int row, int column, const QModelIndex &parent) const
{
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
