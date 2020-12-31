#include "cthreadcapteurs.h"

#define CAPTPRES 0
#define CAPTQAIR 1
#define CAPTTEMP 2
#define CAPTFUMEE 3

CThreadCapteurs::CThreadCapteurs(QByteArray pc, QObject *parent) : QObject(parent)
{
    shm = new CSharedMemory(this);
    tmr = new QTimer(this);
    connect(tmr, &QTimer::timeout, this, &CThreadCapteurs::on_timeout);
    tmr->setInterval(1000);

    _pc = pc;
    if (_pc.at(CAPTPRES)) captPres = new CCapteurPres(this);
    if (_pc.at(CAPTQAIR)) {
        captQa = new CAdafruit_SGP30();
        captQa->begin();
    } // QAir
    if (_pc.at(CAPTTEMP)) captTemp = new CCapteurTemp();
    if (_pc.at(CAPTFUMEE)) captFumee = new CCapteurGazFumeeMQ_2(this);
    utv = new CUTv();
    qDebug() << "[CThreadCapteurs::CThreadCapteurs] Thread lancé";
}

CThreadCapteurs::~CThreadCapteurs()
{
    if (_pc.at(CAPTFUMEE)) delete captFumee;
    if (_pc.at(CAPTTEMP))  delete captTemp;
    if (_pc.at(CAPTQAIR)) delete captQa;
    if (_pc.at(CAPTPRES)) delete captPres;
    delete utv;
    delete tmr;
    delete shm;
}

void CThreadCapteurs::on_timeout()
{
    static int cpt=0;

    //  acquisition des capteurs et sauve dans shm
    // température
    if (_pc.at(CAPTTEMP)) {
        float val = captTemp->getTemp();
        if ( (val<-99) || (val>99))
            val = -99.9f;
        shm->setMesTemp(val);
    } // if CAPTTEMP
    // U TV
    shm->setCapteurUTv(utv->getU());
    // Présence
    if (_pc.at(CAPTPRES))
        shm->setCapteurPresence(captPres->getPresence());
    // fumée
    if (_pc.at(CAPTFUMEE))
        shm->setCapteurGazFumee(captFumee->getCapteur());
    // qualité air
    if (_pc.at(CAPTQAIR)) {
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
    } // if CAPTQAIR
}

void CThreadCapteurs::go()
{
    tmr->start();  // lance l'acquisition
}
