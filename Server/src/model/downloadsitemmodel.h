#ifndef DOWNLOADSITEMMODEL_H
#define DOWNLOADSITEMMODEL_H

#include <QAbstractItemModel>

namespace QDataSuite {
class AbstractDataAccessObject;
}

class Download;
class DownloadPackage;

class DownloadsItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Columns {
        NameColumn,
        ProgressColumn,
        SpeedColumn,
        SpeedWeightedColumn,
        EtaColumn,
        MessageColumn,
        UserInputColumn,
        UrlColumn,
        RedirectedUrlColumn
    };

    explicit DownloadsItemModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    bool hasChildren(const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

private slots:
    void insertPackage(QObject *object);
    void updatePackage(QObject *object);
    void removePackage(QObject *object);

    void insertDownload(QObject *object);
    void updateDownload(QObject *object);
    void removeDownload(QObject *object);

private:
    QList<DownloadPackage *> m_packagesCache;
    QHash<DownloadPackage *, int> m_packageRows;

    QVariant downloadPrimaryKey(Download *download) const;
    QVariant packagePrimaryKey(DownloadPackage *package) const;

    QModelIndex indexForPackage(DownloadPackage *package) const;
};

#endif // DOWNLOADSITEMMODEL_H
