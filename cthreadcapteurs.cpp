#include "cthreadcapteurs.h"

CThreadCapteurs::CThreadCapteurs(QObject *parent) : QObject(parent)
{
    shm = new CSharedMemory(this);
    tmr = new QTimer(this);
    connect(tmr, &QTimer::timeout, this, &CThreadCapteurs::on_timeout);
    tmr->setInterval(1000);

    captPres = new CCapteurPres(this);
    captTemp = new CCapteurTemp();
    captQa = new CAdafruit_SGP30();

}

CThreadCapteurs::~CThreadCapteurs()
{
    delete captPres;
    delete captTemp;
    delete captQa;
    delete tmr;
    delete shm;
}

void CThreadCapteurs::on_timeout()
{
    //  acquisition des capteurs et sauve dans shm
    float val = captTemp->getTemp();
    if ( (val<-99) || (val>99))
        val = -99.9f;
    shm->setMesTemp(val);
}

void CThreadCapteurs::go()
{
    tmr->start();  // lance l'acquisition
}
