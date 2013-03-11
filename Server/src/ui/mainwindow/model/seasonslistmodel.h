#ifndef SEASONSLISTMODEL_H
#define SEASONSLISTMODEL_H

#include <QAbstractListModel>

class Series;
class Season;

class SeasonsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SeasonsListModel(QObject *parent = 0);

    void setSeries(Series *series);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

    Season *seasonByIndex(const QModelIndex &index) const;

private:
    Series *m_series;
};

#endif // SEASONSLISTMODEL_H
