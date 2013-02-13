#include "plugincontroller.h"

#include "preferences.h"
#include "plugins/hoster/hosterplugin.h"
#include "plugins/hoster/premiumizemeplugin.h"

PluginController::PluginController(QObject *parent) :
    QObject(parent)
{
    createHosterPlugins();
}

QList<HosterPlugin *> PluginController::hosterPlugins() const
{
    return m_hosterPlugins;
}

void PluginController::registerHoster(HosterPlugin *hoster)
{
    m_hosterPlugins.append(hoster);
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
