#ifndef DOWNLOADPROVIDERPLUGIN_H
#define DOWNLOADPROVIDERPLUGIN_H

#include <QObject>

#include <QUrl>

class Series;

class DownloadProviderPlugin : public QObject
{
    Q_OBJECT
public:
    struct SeriesData {
        QString title;
        QUrl url;
    };

    explicit DownloadProviderPlugin(const QString &name, QObject *parent = 0);

    QString name() const;

    virtual void searchSeries(const QString &seriesTitle) = 0;
    virtual bool canHandleSeries(Series *series) const = 0;
    virtual void findMissingEpisodes(Series *series) const = 0;

    QString errorString();

    QList<SeriesData> series() const;

signals:
    void foundSeries(QList<SeriesData> series) const;
    void error() const;

protected:
    void setErrorString(const QString &errorString) const;

private slots:
    void setSeries(QList<SeriesData> series);

private:
    QString m_name;
    mutable QString m_errorString;
    QList<SeriesData> m_series;
};

#endif // DOWNLOADPROVIDERPLUGIN_H
