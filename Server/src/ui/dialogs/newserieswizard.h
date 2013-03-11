#ifndef NEWSERIESWIZARD_H
#define NEWSERIESWIZARD_H

#include <QWizard>

#include <QTimer>

#include "plugins/downloadProviders/downloadproviderplugin.h"

namespace Ui {
class NewSeriesWizard;
}

class QCompleter;
class DownloadProviderPlugin;

class NewSeriesWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit NewSeriesWizard(QWidget *parent = 0);
    ~NewSeriesWizard();

private slots:
    void on_comboBoxProvider_currentIndexChanged(int index);

    void on_lineEditName_textEdited(const QString &text);

    void complete(QList<DownloadProviderPlugin::SeriesData> series);
    void timeout();

    void finish();

private:
    Ui::NewSeriesWizard *ui;

    DownloadProviderPlugin *m_currentDownloadProvider;
    DownloadProviderPlugin::SeriesData m_currentSeries;
    QCompleter *m_completer;
    int m_currentSearchId;
    QList<DownloadProviderPlugin::SeriesData> m_mostRecentResults;
    QTimer m_timer;

    void setSeries(DownloadProviderPlugin::SeriesData series);
};

#endif // NEWSERIESWIZARD_H
