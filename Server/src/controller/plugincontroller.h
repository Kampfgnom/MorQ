#ifndef PLUGINCONTROLLER_H
#define PLUGINCONTROLLER_H

#include <QObject>

class HosterPlugin;
class DecrypterPlugin;

class PluginController : public QObject
{
    Q_OBJECT
public:
    explicit PluginController(QObject *parent = 0);
    
    QList<HosterPlugin *> hosterPlugins() const;
    QList<DecrypterPlugin *> decrypterPlugins() const;

    void registerHoster(HosterPlugin *hoster);
    void registerDecrypter(DecrypterPlugin *decrypter);

private:
    QList<HosterPlugin *> m_hosterPlugins;
    QList<DecrypterPlugin *> m_decrypterPlugins;

    void createHosterPlugins();
    void createDecrypterPlugins();
};

#endif // PLUGINCONTROLLER_H
