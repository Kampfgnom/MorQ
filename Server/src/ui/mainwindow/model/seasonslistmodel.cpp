#include "seasonslistmodel.h"

#include "model/series.h"
#include "model/season.h"

SeasonsListModel::SeasonsListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_series(nullptr)
{
}

void SeasonsListModel::setSeries(Series *series)
{
    beginResetModel();
    m_series = series;
    endResetModel();
}


QVariant SeasonsListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return QString("S%1").arg(seasonByIndex(index)->number(),2,10,QChar('0'));
    }

    return QVariant();
}

int SeasonsListModel::rowCount(const QModelIndex &parent) const
{
    if(!m_series)
        return 0;

    if(parent.isValid())
        return 0;

    return m_series->seasons().size();
}

Season *SeasonsListModel::seasonByIndex(const QModelIndex &index) const
{
    return m_series->seasons().at(index.row());
}
