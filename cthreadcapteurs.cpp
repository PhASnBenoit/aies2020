#include "cthreadcapteurs.h"

CThreadCapteurs::CThreadCapteurs(QObject *parent) : QObject(parent)
{
    shm = new CSharedMemory(this);
    tmr = new QTimer(this);
    connect(tmr, &QTimer::timeout, this, &CThreadCapteurs::on_timeout);
    tmr->setInterval(1000);

    captPres = new CCapteurPres(this);
    captTemp = new CCapteurTemp();
    captFumee = new CCapteurGazFumeeMQ_2(this);
    captQa = new CAdafruit_SGP30();
    captQa->begin();
    qDebug() << "[CThreadCapteurs::CThreadCapteurs] Thread lancé";
}

CThreadCapteurs::~CThreadCapteurs()
{
    delete captPres;
    delete captTemp;
    delete captFumee;
    delete captQa;
    delete tmr;
    delete shm;
}

void CThreadCapteurs::on_timeout()
{
    static int cpt=0;

    //  acquisition des capteurs et sauve dans shm
    // température
    float val = captTemp->getTemp();
    if ( (val<-99) || (val>99))
        val = -99.9f;
    shm->setMesTemp(val);
    // Présence
    shm->setCapteurPresence(captPres->getPresence());
    // fumée
    shm->setCapteurGazFumee(captFumee->getCapteur());
    // qualité air
    if (! captQa->IAQmeasure()) {
        shm->setMesTVOC(-1);
        shm->setMesTVOC(-1);
    } else {
        shm->setMesTVOC(captQa->TVOC);
        shm->setMesECO2(captQa->eCO2);
    } // else
    if (! captQa->IAQmeasureRaw()) {
        shm->setMesRawH2(-1);
        shm->setMesRawEthanol(-1);
    } else {
        shm->setMesRawH2(captQa->rawH2);
        shm->setMesRawEthanol(captQa->rawEthanol);
    } // else
    cpt++;
    if (cpt == 20) {
        cpt=0;
        quint16 TVOC_base, eCO2_base;
        if (! captQa->getIAQBaseline(&eCO2_base, &TVOC_base)) {
            shm->setMesBaselineTVOC(-1);
            shm->setMesBaselineECO2(-1);
        } else {
            shm->setMesBaselineTVOC(TVOC_base);
            shm->setMesBaselineECO2(eCO2_base);
        } // else
    } // if cpt
}

void CThreadCapteurs::go()
{
    tmr->start();  // lance l'acquisition
}
