#include "episodeslistmodel.h"

#include "model/season.h"
#include "model/episode.h"

EpisodesListModel::EpisodesListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_season(nullptr)
{
}

void EpisodesListModel::setSeason(Season *season)
{
    beginResetModel();
    m_season = season;
    endResetModel();
}

QVariant EpisodesListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    Episode *episode = m_season->episodes().at(index.row());

    switch(role) {
    case Qt::DisplayRole:
        return QString("E%1 - %2")
                .arg(episode->number(),2,10,QChar('0'))
                .arg(episode->serienJunkiesTitle());
    }

    return QVariant();
}

int EpisodesListModel::rowCount(const QModelIndex &parent) const
{
    if(!m_season)
        return 0;

    if(parent.isValid())
        return 0;

    return m_season->episodes().size();
}
