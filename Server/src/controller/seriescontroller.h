#ifndef SERIESCONTROLLER_H
#define SERIESCONTROLLER_H

#include <QObject>

class Series;

class SeriesController : public QObject
{
    Q_OBJECT
public:
    explicit SeriesController(QObject *parent = 0);
    
    void findMissingEpisodes(Series *series);
};

#endif // SERIESCONTROLLER_H
