#ifndef HOSTERPLUGIN_H
#define HOSTERPLUGIN_H

#include <QObject>

class Download;
class QUrl;
class Downloader;

class HosterPlugin : public QObject
{
public:
    struct Account {
        QString userName;
        QString password;
    };

    HosterPlugin(QObject *parent = 0);

    virtual void getDownloadInformation(Download *download) = 0;
    virtual Downloader *handleDownload(Download *download) = 0;
    virtual bool canHandleUrl(const QUrl &url) const = 0;

    Account account() const;
    void setAccount(const Account &account);

private:
    Account m_account;
};

#endif // HOSTERPLUGIN_H
