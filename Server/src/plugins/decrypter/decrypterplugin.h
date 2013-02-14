#ifndef DECRYPTERPLUGIN_H
#define DECRYPTERPLUGIN_H

#include <QObject>

class DownloadPackage;

class DecrypterPlugin : public QObject
{
    Q_OBJECT
public:
    explicit DecrypterPlugin(QObject *parent = 0);

    virtual void handlePackage(DownloadPackage *package) = 0;
    virtual bool canHandleUrl(const QUrl &url) const = 0;

signals:
    void finishedPackage(DownloadPackage *package);

};

#endif // DECRYPTERPLUGIN_H
