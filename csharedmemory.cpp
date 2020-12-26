#include "csharedmemory.h"
#include <strings.h>

CSharedMemory::CSharedMemory(QObject *parent) : QObject(parent)
{
    shm = new QSharedMemory(KEY, this);
    if (!shm->attach()) {
        // création shm
        if (!shm->create(sizeof(T_CAPTEURS)))
            qDebug() << shm->errorString();
        if (!shm->attach())
            qDebug() << shm->errorString();
    } // if attach
    capt = static_cast<T_CAPTEURS *>(shm->data());
    bzero(capt, sizeof(T_CAPTEURS));
}

CSharedMemory::~CSharedMemory()
{
    shm->detach();
    delete shm;
}

int CSharedMemory::getMesTVOC() const
{
    return capt->mesTVOC;
}

int CSharedMemory::getMesECO2() const
{
    return capt->mesECO2;
}

int CSharedMemory::getMesRawH2() const
{
    return capt->mesRawH2;
}


int CSharedMemory::getMesRawEthanol() const
{
    return capt->mesRawEthanol;
}


int CSharedMemory::getMesBaselineTVOC() const
{
    return capt->mesBaselineTVOC;
}

int CSharedMemory::getMesBaselineECO2() const
{
    return capt->mesBaselineECO2;
}


bool CSharedMemory::getCapteurGazFumee() const
{
    return capt->capteurGazFumee;
}


bool CSharedMemory::getCapteurPresence() const
{
    return capt->capteurPresence;
}


float CSharedMemory::getMesTemp() const
{
    return capt->mesTemp;
}

void CSharedMemory::setMesTVOC(int val) const
{
    shm->lock();
    capt->mesTVOC = val;
    shm->unlock();
}

void CSharedMemory::setMesECO2(int val) const
{
    shm->lock();
    capt->mesECO2 = val;
    shm->unlock();
}

void CSharedMemory::setMesRawH2(int val) const
{
    shm->lock();
    capt->mesRawH2 = val;
    shm->unlock();
}

void CSharedMemory::setMesRawEthanol(int val) const
{
    shm->lock();
    capt->mesRawEthanol = val;
    shm->unlock();
}

void CSharedMemory::setMesBaselineTVOC(int val) const
{
    shm->lock();
    capt->mesBaselineTVOC = val;
    shm->unlock();
}

void CSharedMemory::setMesBaselineECO2(int val) const
{
    shm->lock();
    capt->mesBaselineECO2 = val;
    shm->unlock();
}

void CSharedMemory::setCapteurGazFumee(bool val) const
{
    shm->lock();
    capt->capteurGazFumee = val;
    shm->unlock();
}

void CSharedMemory::setCapteurPresence(bool val) const
{
    shm->lock();
    capt->capteurPresence = val;
    shm->unlock();
}

void CSharedMemory::setMesTemp(float val) const
{
    shm->lock();
    capt->mesTemp = val;
    shm->unlock();
}
