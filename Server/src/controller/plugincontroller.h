#ifndef PLUGINCONTROLLER_H
#define PLUGINCONTROLLER_H

#include <QObject>

class HosterPlugin;

class PluginController : public QObject
{
    Q_OBJECT
public:
    explicit PluginController(QObject *parent = 0);
    
    QList<HosterPlugin *> hosterPlugins() const;

    void registerHoster(HosterPlugin *hoster);

private:
    QList<HosterPlugin *> m_hosterPlugins;

    void createHosterPlugins();
};

#endif // PLUGINCONTROLLER_H
