#include "seriestreemodel.h"

#include "controller/controller.h"

#include "model/series.h"
#include "model/season.h"
#include "model/episode.h"

SeriesTreeModel::SeriesTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    connect(Controller::seriesDao(), &QDataSuite::AbstractDataAccessObject::objectInserted,
            this, &SeriesTreeModel::insertSeries);
    connect(Controller::seriesDao(), &QDataSuite::AbstractDataAccessObject::objectUpdated,
            this, &SeriesTreeModel::updateSeries);
    connect(Controller::seriesDao(), &QDataSuite::AbstractDataAccessObject::objectRemoved,
            this, &SeriesTreeModel::removeSeries);

    connect(Controller::seasonsDao(), &QDataSuite::AbstractDataAccessObject::objectInserted,
            this, &SeriesTreeModel::insertSeason);
    connect(Controller::seasonsDao(), &QDataSuite::AbstractDataAccessObject::objectUpdated,
            this, &SeriesTreeModel::updateSeason);
    connect(Controller::seasonsDao(), &QDataSuite::AbstractDataAccessObject::objectRemoved,
            this, &SeriesTreeModel::removeSeason);

    connect(Controller::episodesDao(), &QDataSuite::AbstractDataAccessObject::objectInserted,
            this, &SeriesTreeModel::insertEpisode);
    connect(Controller::episodesDao(), &QDataSuite::AbstractDataAccessObject::objectUpdated,
            this, &SeriesTreeModel::updateEpisode);
    connect(Controller::episodesDao(), &QDataSuite::AbstractDataAccessObject::objectRemoved,
            this, &SeriesTreeModel::removeEpisode);

    foreach(QObject *object, Controller::seriesDao()->readAllObjects()) {
        insertSeries(object);
    }
}

QVariant SeriesTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(!index.parent().isValid()) {
        Series *series = seriesByIndex(index);
        if(!series)
            return QVariant();

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case TitleColumn:
                return series->title();
            case UrlColumn:
                return series->serienJunkiesUrl();

            default:
                break;
            }
        }
    }
    else if(!index.parent().parent().isValid()) {
        Season *season = seasonByIndex(index);

        if(!season)
            return QVariant();

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case TitleColumn:
                return season->serienJunkiesTitle();
            case UrlColumn:
                return season->serienJunkiesUrl();

            default:
                break;
            }
        }
    }
    else if(!index.parent().parent().parent().isValid()) {
        Episode *episode = episodeByIndex(index);

        if(!episode)
            return QVariant();

        if(role == Qt::DisplayRole) {
            switch(index.column()) {
            case TitleColumn:
                return QString::number(episode->number());
            case UrlColumn:
                return QString("some url");

            default:
                break;
            }
        }
    }

    return QVariant();
}

QVariant SeriesTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Vertical)
        return QVariant();

    if(role == Qt::DisplayRole) {
        switch(section) {
        case TitleColumn:
            return QVariant("Title");
        case UrlColumn:
            return QVariant("URL");
        default:
            break;
        }
    }

    return QVariant();
}

int SeriesTreeModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return Controller::seriesDao()->count();

    if(!parent.parent().isValid()) {
        Series *series = seriesByIndex(parent);
        if(!series)
            return 0;

        return series->seasons().size();
    }

    Season *season = seasonByIndex(parent);

    if(!season)
        return 0;

    int ret = season->episodes().size();
    return ret;
}

int SeriesTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

bool SeriesTreeModel::hasChildren(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return !m_seriesRows.isEmpty();

    if(!parent.parent().isValid()) {
        Series *series = seriesByIndex(parent);
        if(!series)
            return false;

        return !series->seasons().isEmpty();
    }

    Season *season = seasonByIndex(parent);

    if(!season)
        return false;

    bool ret = !season->episodes().isEmpty();
    return ret;
}

QModelIndex SeriesTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row, column, parent))
        return QModelIndex();

    if(parent.parent().isValid()) {
        Season *season = seasonByIndex(parent);
        return createIndex(row, column, season->episodes().at(row));
    }

    if(parent.isValid()) {
        Series *series = seriesByIndex(parent);
        return createIndex(row, column, series->seasons().at(row));
    }

    Series *series = Controller::seriesDao()->readAll().at(row);
    return createIndex(row, column, series);
}

QModelIndex SeriesTreeModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    Series *series = seriesByIndex(child);
    if(series)
        return QModelIndex();

    Season *season = seasonByIndex(child);
    if(season) {
        series = season->series();
        if(!series || !m_seriesRows.contains(series))
            return QModelIndex();

        return indexForSeries(series);
    }

    Episode *episode = episodeByIndex(child);
    if(episode) {
        season = episode->season();
        if(!season || !m_seasonRows.contains(season))
            return QModelIndex();

        return indexForSeason(season);
    }

    return QModelIndex();
}

Series *SeriesTreeModel::seriesByIndex(const QModelIndex &index) const
{
    Series *series = static_cast<Series *>(index.internalPointer());
    if(!m_seriesRows.contains(series))
        return nullptr;

    return series;
}

Season *SeriesTreeModel::seasonByIndex(const QModelIndex &index) const
{
    Season *season = static_cast<Season *>(index.internalPointer());
    if(!m_seasonRows.contains(season))
        return nullptr;

    return season;
}

Episode *SeriesTreeModel::episodeByIndex(const QModelIndex &index) const
{
    Episode *episode = static_cast<Episode *>(index.internalPointer());
    if(!m_episodeRows.contains(episode))
        return nullptr;

    return episode;
}


void SeriesTreeModel::insertSeries(QObject *object)
{
    Series *series = static_cast<Series *>(object);

    int index = Controller::seriesDao()->readAll().indexOf(series);
    beginInsertRows(QModelIndex(), index, index);

    _insertSeries(series);

    endInsertRows();
}

void SeriesTreeModel::_insertSeries(Series *series)
{
    adjustAllSeriesRows();

    foreach(Season *dl, series->seasons()) {
        _insertSeason(dl);
    }
}

void SeriesTreeModel::updateSeries(QObject *object)
{
    Series *series = static_cast<Series *>(object);
    if(!m_seriesRows.contains(series))
        return;

    int i = m_seriesRows[series];
    emit dataChanged(index(i, 0),
                     index(i, columnCount()));

    foreach(Season *dl, series->seasons()) {
        _removeSeason(dl);
        _insertSeason(dl);
    }
}

void SeriesTreeModel::removeSeries(QObject *object)
{
    Series *series = static_cast<Series *>(object);
    if(!m_seriesRows.contains(series))
        return;

    int i = m_seriesRows[series];

    beginRemoveRows(QModelIndex(), i, i);

    m_seriesRows.remove(series);
    adjustAllSeriesRows();

    foreach(Season *dl, series->seasons()) {
        _removeSeason(dl);
    }

    endRemoveRows();
}

void SeriesTreeModel::adjustAllSeriesRows()
{
    int i = 0;
    foreach(Series *s, Controller::seriesDao()->readAll()) {
        m_seriesRows.insert(s, i);
        ++i;
    }
}

void SeriesTreeModel::insertSeason(QObject *object)
{
    Season *season = static_cast<Season *>(object);
    if(!season || !season->series())
        return;

    QModelIndex parent = indexForSeries(season->series());
    if(!parent.isValid())
        return;

    int index = season->series()->seasons().indexOf(season);

    beginInsertRows(parent, index, index);

    _insertSeason(season);

    endInsertRows();
}

void SeriesTreeModel::_insertSeason(Season *season)
{
    adjustAllSeasonRows(season->series());

    foreach(Episode *episode, season->episodes()) {
        _insertEpisode(episode);
    }
}

void SeriesTreeModel::updateSeason(QObject *object)
{
    Season *season = static_cast<Season *>(object);
    QModelIndex parent = indexForSeries(season->series());
    if(!parent.isValid())
        return;

    if(!m_seasonRows.contains(season)) {
        int index = season->series()->seasons().indexOf(season);

        beginInsertRows(parent, index, index);

        _insertSeason(season);

        endInsertRows();
    }
    else {
        int i = m_seasonRows[season];

        emit dataChanged(index(i, 0, parent),
                         index(i, columnCount(), parent));
    }
}

void SeriesTreeModel::removeSeason(QObject *object)
{
    Season *season = static_cast<Season *>(object);
    if(!m_seasonRows.contains(season))
        return;

    QModelIndex parent = indexForSeries(season->series());
    if(!parent.isValid())
        return;

    int i = m_seasonRows[season];

    beginRemoveRows(parent, i, i);

    _removeSeason(season);

    endRemoveRows();
}

void SeriesTreeModel::_removeSeason(Season *season)
{
    int i = 0;
    foreach(Season *s, season->series()->seasons()) {
        m_seasonRows.remove(s);
        if(s != season) {
            m_seasonRows.insert(s, i);
            ++i;
        }
    }

    foreach(Episode *e, season->episodes()) {
        _removeEpisode(e);
    }
}

void SeriesTreeModel::adjustAllSeasonRows(Series *series)
{
    int i = 0;
    foreach(Season *season, series->seasons()) {
        m_seasonRows.insert(season, i);
        ++i;
    }
}




void SeriesTreeModel::insertEpisode(QObject *object)
{
    Episode *episode = static_cast<Episode *>(object);
    if(!episode || !episode->season())
        return;

    QModelIndex parent = indexForSeason(episode->season());
    if(!parent.isValid())
        return;

    int index = episode->season()->episodes().indexOf(episode);

    beginInsertRows(parent, index, index);

    _insertEpisode(episode);

    endInsertRows();
}

void SeriesTreeModel::_insertEpisode(Episode *episode)
{
    adjustAllEpisodeRows(episode->season());
}

void SeriesTreeModel::updateEpisode(QObject *object)
{
    Episode *episode = static_cast<Episode *>(object);
    QModelIndex parent = indexForSeason(episode->season());
    if(!parent.isValid())
        return;

    if(!m_episodeRows.contains(episode)) {
        int index = episode->season()->episodes().indexOf(episode);

        beginInsertRows(parent, index, index);

        _insertEpisode(episode);

        endInsertRows();
    }
    else {
        int i = m_episodeRows[episode];

        emit dataChanged(index(i, 0, parent),
                         index(i, columnCount(), parent));
    }
}

void SeriesTreeModel::removeEpisode(QObject *object)
{
    Episode *episode = static_cast<Episode *>(object);
    if(!m_episodeRows.contains(episode))
        return;

    QModelIndex parent = indexForSeason(episode->season());
    if(!parent.isValid())
        return;

    int i = m_episodeRows[episode];

    beginRemoveRows(parent, i, i);

    _removeEpisode(episode);

    endRemoveRows();
}

void SeriesTreeModel::_removeEpisode(Episode *episode)
{
    int i = 0;
    foreach(Episode *e, episode->season()->episodes()) {
        m_episodeRows.remove(e);
        if(e != episode) {
            m_episodeRows.insert(e, i);
            ++i;
        }
    }
}

void SeriesTreeModel::adjustAllEpisodeRows(Season *season)
{
    int i = 0;
    foreach(Episode *e, season->episodes()) {
        m_episodeRows.insert(e, i);
        ++i;
    }
}









QModelIndex SeriesTreeModel::indexForSeries(Series *series) const
{
    if(!series || !m_seriesRows.contains(series))
        return QModelIndex();

    return createIndex(m_seriesRows.value(series), 0, series);
}

QModelIndex SeriesTreeModel::indexForSeason(Season *season) const
{
    if(!season || !m_seasonRows.contains(season))
        return QModelIndex();

    return createIndex(m_seasonRows.value(season), 0, season);
}
