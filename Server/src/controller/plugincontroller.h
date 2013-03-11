#ifndef PLUGINCONTROLLER_H
#define PLUGINCONTROLLER_H

#include <QObject>

class HosterPlugin;
class DecrypterPlugin;
class DownloadProviderPlugin;

class PluginController : public QObject
{
    Q_OBJECT
public:
    explicit PluginController(QObject *parent = 0);
    
    QList<HosterPlugin *> hosterPlugins() const;
    QList<DecrypterPlugin *> decrypterPlugins() const;
    QList<DownloadProviderPlugin *> downloadProviderPlugins() const;

    void registerHoster(HosterPlugin *hoster);
    void registerDecrypter(DecrypterPlugin *decrypter);
    void registerDownloadProvider(DownloadProviderPlugin *provider);

private:
    QList<HosterPlugin *> m_hosterPlugins;
    QList<DecrypterPlugin *> m_decrypterPlugins;
    QList<DownloadProviderPlugin *> m_downloadProviderPlugins;

    void createHosterPlugins();
    void createDecrypterPlugins();
    void createDownloadProviderPlugins();
};

#endif // PLUGINCONTROLLER_H
