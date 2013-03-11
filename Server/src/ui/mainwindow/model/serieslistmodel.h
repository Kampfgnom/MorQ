#ifndef SERIESLISTMODEL_H
#define SERIESLISTMODEL_H

#include <QAbstractListModel>

class Series;

class SeriesListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SeriesListModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

    Series *seriesByIndex(const QModelIndex &index) const;

signals:
    
public slots:
    
};

#endif // SERIESLISTMODEL_H
