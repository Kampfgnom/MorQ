#ifndef EPISODESLISTMODEL_H
#define EPISODESLISTMODEL_H

#include <QAbstractListModel>

class Season;

class EpisodesListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit EpisodesListModel(QObject *parent = 0);
    
    void setSeason(Season *season);

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

private:
    Season *m_season;
    
};

#endif // EPISODESLISTMODEL_H
