#include "ccapteurgazfumeemq_2.h"

CCapteurGazFumeeMQ_2::CCapteurGazFumeeMQ_2(QObject *parent) :
    QObject(parent)
{
    gpioCF = new CGpio(CF);
}

CCapteurGazFumeeMQ_2::~CCapteurGazFumeeMQ_2()
{
    delete gpioCF;
}

bool CCapteurGazFumeeMQ_2::getCapteur()
{
    int val = gpioCF->lire();
    qDebug() << "[CCapteurGazFumeeMQ_2::lireCapteur] Fumée="<<val;
    if (val>128)
        return true;
    else
        return false;
}
