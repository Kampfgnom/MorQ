#ifndef DOWNLOADSITEMMODEL_H
#define DOWNLOADSITEMMODEL_H

#include <QAbstractItemModel>

namespace QDataSuite {
class AbstractDataAccessObject;
}

class Download;

class DownloadsItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Columns {
        FileNameColumn,
        ProgressColumn,
        SpeedColumn,
        SpeedWeightedColumn,
        EtaColumn,
        UrlColumn,
        RedirectedUrlColumn
    };

    explicit DownloadsItemModel(QDataSuite::AbstractDataAccessObject *dao, QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    QModelIndex parent(const QModelIndex &child) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;

private slots:
    void insertObject(QObject *object);
    void updateObject(QObject *object);
    void removeObject(QObject *object);

private:
    QDataSuite::AbstractDataAccessObject *m_dao;
    QList<Download *> m_cache;

    Download *download(const QModelIndex &index) const;
    QVariant primaryKey(Download *download) const;
};

#endif // DOWNLOADSITEMMODEL_H
