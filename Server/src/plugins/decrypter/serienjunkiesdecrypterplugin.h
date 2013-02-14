#ifndef SERIENJUNKIESDECRYPTERPLUGIN_H
#define SERIENJUNKIESDECRYPTERPLUGIN_H

#include "decrypterplugin.h"

class SerienJunkiesDecrypterPlugin : public DecrypterPlugin
{
    Q_OBJECT
public:
    explicit SerienJunkiesDecrypterPlugin(QObject *parent = 0);

    void handlePackage(DownloadPackage *package);
    bool canHandleUrl(const QUrl &url) const;

private slots:
    void packageFinished();
};

class SerienJunkiesDecryptHandler : public QObject
{
    Q_OBJECT
public:
    SerienJunkiesDecryptHandler(DownloadPackage *package, SerienJunkiesDecrypterPlugin *parent);
    void getInformation();

    DownloadPackage *package() const;

signals:
    void finished();

private slots:
    void requestCaptchaSolution();
    void replyFinished();

private:
    DownloadPackage *m_package;
    SerienJunkiesDecrypterPlugin *m_plugin;
};

#endif // SERIENJUNKIESDECRYPTERPLUGIN_H
