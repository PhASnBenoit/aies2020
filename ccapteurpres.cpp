#include <ccapteurpres.h>

CCapteurPres::CCapteurPres(QObject *parent) :
    QObject(parent)
{
    mPres = new CGpio(PRES);
}

CCapteurPres::~CCapteurPres()
{
    delete mPres;
}

bool CCapteurPres::getPresence()
{
    return static_cast<bool>(mPres->lire());
}


