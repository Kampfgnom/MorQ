#include "preferences.h"

#include <QSettings>

static const QString PREMIUMIZEME_USERNAME("hosters/premiumizeme/username");
static const QString PREMIUMIZEME_PASSWORD("hosters/premiumizeme/password");

static const QString EXTRACTDOWNLOADS("downloads/extract");
static const QString EXTRACTFOLDER("downloads/extractFolder");
static const QString DOWNLOADFOLDER("downloads/folder");
static const QString DOWNLOADSMAX("downloads/max");

QString Preferences::premiumizeMeUserName()
{
    QSettings settings;
    return settings.value(PREMIUMIZEME_USERNAME).toString();
}

void Preferences::setPremiumizeMeUserName(const QString &name)
{
    QSettings settings;
    settings.setValue(PREMIUMIZEME_USERNAME, name);
}

QString Preferences::premiumizeMeUserPassword()
{
    QSettings settings;
    return settings.value(PREMIUMIZEME_PASSWORD).toString();
}

void Preferences::setPremiumizeMeUserPassword(const QString &password)
{
    QSettings settings;
    settings.setValue(PREMIUMIZEME_PASSWORD, password);
}

bool Preferences::extractDownloads()
{
    QSettings settings;
    return settings.value(EXTRACTDOWNLOADS).toBool();
}

void Preferences::setExtractDownloads(bool extract)
{
    QSettings settings;
    settings.setValue(EXTRACTDOWNLOADS, extract);
}

QString Preferences::downloadFolder()
{
    QSettings settings;
    return settings.value(DOWNLOADFOLDER).toString();
}

void Preferences::setDownloadFolder(const QString &downloadFolder)
{
    QSettings settings;
    settings.setValue(DOWNLOADFOLDER, downloadFolder);
}

int Preferences::maxDownloads()
{
    QSettings settings;
    return settings.value(DOWNLOADSMAX, 2).toInt();
}

void Preferences::setMaxDownloads(int maxDownloads)
{
    QSettings settings;
    settings.setValue(DOWNLOADSMAX, maxDownloads);
}

QString Preferences::extractFolder()
{
    QSettings settings;
    return settings.value(EXTRACTFOLDER).toString();
}

void Preferences::setExtractFolder(const QString &folder)
{
    QSettings settings;
    settings.setValue(EXTRACTFOLDER, folder);
}
