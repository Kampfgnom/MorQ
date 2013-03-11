#include "downloadproviderplugin.h"

DownloadProviderPlugin::DownloadProviderPlugin(const QString &name, QObject *parent) :
    QObject(parent),
    m_name(name)
{
    connect(this, &DownloadProviderPlugin::foundSeries,
            this, &DownloadProviderPlugin::setSeries);
}

QString DownloadProviderPlugin::name() const
{
    return m_name;
}

QString DownloadProviderPlugin::errorString()
{
    return m_errorString;
}

QList<DownloadProviderPlugin::SeriesData> DownloadProviderPlugin::series() const
{
    return m_series;
}


void DownloadProviderPlugin::setErrorString(const QString &errorString) const
{
    m_errorString = errorString;
}

void DownloadProviderPlugin::setSeries(QList<DownloadProviderPlugin::SeriesData> series)
{
    m_series = series;
}
