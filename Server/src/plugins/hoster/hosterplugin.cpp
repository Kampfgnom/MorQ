#include "hosterplugin.h"


HosterPlugin::HosterPlugin(QObject *parent) :
    QObject(parent)
{
}

HosterPlugin::Account HosterPlugin::account() const
{
    return m_account;
}

void HosterPlugin::setAccount(const Account &account)
{
    m_account = account;
}
