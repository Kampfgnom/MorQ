#ifndef SERIESTREEMODEL_H
#define SERIESTREEMODEL_H

#include <QAbstractItemModel>

class Series;
class Season;
class Episode;

class SeriesTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Columns {
        TitleColumn,
        UrlColumn
    };

    explicit SeriesTreeModel(QObject *parent = 0);
    
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;

private:
    QHash<Series *, int> m_seriesRows;
    QHash<Season *, int> m_seasonRows;
    QHash<Episode *, int> m_episodeRows;

    Series *seriesByIndex(const QModelIndex &index) const;
    Season *seasonByIndex(const QModelIndex &index) const;
    Episode *episodeByIndex(const QModelIndex &index) const;

    QModelIndex indexForSeries(Series *series) const;
    QModelIndex indexForSeason(Season *season) const;

    void insertSeries(QObject *object);
    void _insertSeries(Series *series);
    void updateSeries(QObject *object);
    void removeSeries(QObject *object);
    void adjustAllSeriesRows();

    void insertSeason(QObject *object);
    void _insertSeason(Season *season);
    void updateSeason(QObject *object);
    void removeSeason(QObject *object);
    void _removeSeason(Season *season);
    void adjustAllSeasonRows(Series *series);

    void _removeEpisode(Episode *episode);
    void removeEpisode(QObject *object);
    void updateEpisode(QObject *object);
    void _insertEpisode(Episode *episode);
    void insertEpisode(QObject *object);
    void adjustAllEpisodeRows(Season *season);
};

#endif // SERIESTREEMODEL_H
