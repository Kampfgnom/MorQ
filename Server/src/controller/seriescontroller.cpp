#include "seriescontroller.h"

#include "controller.h"
#include "plugincontroller.h"

#include "plugins/downloadProviders/downloadproviderplugin.h"

#include "model/series.h"

#include <qserienjunkies.h>

SeriesController::SeriesController(QObject *parent) :
    QObject(parent)
{
}

void SeriesController::findMissingEpisodes(Series *series)
{
    foreach(DownloadProviderPlugin *provider, Controller::plugins()->downloadProviderPlugins()) {
        if(provider->canHandleSeries(series)) {
            provider->findMissingEpisodes(series);
            break;
        }
    }
}
