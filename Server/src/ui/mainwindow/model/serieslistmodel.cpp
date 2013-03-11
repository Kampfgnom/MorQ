#include "serieslistmodel.h"

#include "controller/controller.h"

#include "model/series.h"

SeriesListModel::SeriesListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

QVariant SeriesListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return seriesByIndex(index)->title();
    }

    return QVariant();
}

int SeriesListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return Controller::seriesDao()->count();
}

Series *SeriesListModel::seriesByIndex(const QModelIndex &index) const
{
    return Controller::seriesDao()->readAll().at(index.row());
}
