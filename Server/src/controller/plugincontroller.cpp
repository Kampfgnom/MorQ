#include "plugincontroller.h"

#include "preferences.h"
#include "plugins/hoster/hosterplugin.h"
#include "plugins/hoster/premiumizemeplugin.h"
#include "plugins/decrypter/decrypterplugin.h"
#include "plugins/decrypter/serienjunkiesdecrypterplugin.h"


PluginController::PluginController(QObject *parent) :
    QObject(parent)
{
    createHosterPlugins();
    createDecrypterPlugins();
}

QList<HosterPlugin *> PluginController::hosterPlugins() const
{
    return m_hosterPlugins;
}

QList<DecrypterPlugin *> PluginController::decrypterPlugins() const
{
    return m_decrypterPlugins;
}

void PluginController::registerHoster(HosterPlugin *hoster)
{
    m_hosterPlugins.append(hoster);
}

void PluginController::registerDecrypter(DecrypterPlugin *decrypter)
{
    m_decrypterPlugins.append(decrypter);
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
