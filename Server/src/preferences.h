#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QString>

class Preferences
{
public:
    static QString premiumizeMeUserName();
    static void setPremiumizeMeUserName(const QString &name);
    static QString premiumizeMeUserPassword();
    static void setPremiumizeMeUserPassword(const QString &password);
};

#endif // PREFERENCES_H
