#include <ccapteurpres.h>

CCapteurPres::CCapteurPres(QObject *parent) :
    QObject(parent)
{
    varState = false;
    mPres = new CGpio(PRES);
}

CCapteurPres::~CCapteurPres()
{
    delete mPres;
}

bool CCapteurPres::getPresence()
{
    varState = mPres->lire();
//    qDebug() << "VALEUR PRESENCE : " << QString::number(varState);
    if(varState)
    {
        emit sigAffPres(1);
        return true;
    }
    else
    {
        emit sigAffPres(0);
        return false;
    }
}


