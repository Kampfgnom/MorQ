#include "preferences.h"

#include <QSettings>

static const QString PREMIUMIZEME_USERNAME("hosters/premiumizeme/username");
static const QString PREMIUMIZEME_PASSWORD("hosters/premiumizeme/password");

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
