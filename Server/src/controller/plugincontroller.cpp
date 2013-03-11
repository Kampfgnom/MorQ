#include "plugincontroller.h"

#include "preferences.h"
#include "plugins/hoster/hosterplugin.h"
#include "plugins/hoster/premiumizemeplugin.h"
#include "plugins/decrypter/decrypterplugin.h"
#include "plugins/decrypter/serienjunkiesdecrypterplugin.h"
#include "plugins/downloadProviders/downloadproviderplugin.h"
#include "plugins/downloadProviders/serienjunkiesproviderplugin.h"


PluginController::PluginController(QObject *parent) :
    QObject(parent)
{
    createHosterPlugins();
    createDecrypterPlugins();
    createDownloadProviderPlugins();
}

QList<HosterPlugin *> PluginController::hosterPlugins() const
{
    return m_hosterPlugins;
}

QList<DecrypterPlugin *> PluginController::decrypterPlugins() const
{
    return m_decrypterPlugins;
}

QList<DownloadProviderPlugin *> PluginController::downloadProviderPlugins() const
{
    return m_downloadProviderPlugins;
}

void PluginController::registerHoster(HosterPlugin *hoster)
{
    m_hosterPlugins.append(hoster);
}

void PluginController::registerDecrypter(DecrypterPlugin *decrypter)
{
    m_decrypterPlugins.append(decrypter);
}

void PluginController::registerDownloadProvider(DownloadProviderPlugin *provider)
{
    m_downloadProviderPlugins.append(provider);
}

void PluginController::createHosterPlugins()
{
    HosterPlugin *plugin = new PremuimizeMePlugin(this);
    HosterPlugin::Account account;
    account.userName = Preferences::premiumizeMeUserName();
    account.password = Preferences::premiumizeMeUserPassword();
    plugin->setAccount(account);

    registerHoster(plugin);
}

void PluginController::createDecrypterPlugins()
{
    SerienJunkiesDecrypterPlugin *plugin = new SerienJunkiesDecrypterPlugin(this);
    registerDecrypter(plugin);
}

void PluginController::createDownloadProviderPlugins()
{
    DownloadProviderPlugin *provider = new SerienjunkiesProviderPlugin(this);
    registerDownloadProvider(provider);
}
