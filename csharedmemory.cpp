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

int CSharedMemory::getMesH2() const
{
    return capt->mesH2;
}

int CSharedMemory::getMesEthanol() const
{
    return capt->mesEthanol;
}

int CSharedMemory::getMesRawTVOC() const
{
    return capt->mesRawTVOC;
}


int CSharedMemory::getMesRawECO2() const
{
    return capt->mesRawECO2;
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

void CSharedMemory::setMesH2(int val) const
{
    capt->mesH2 = val;
}

void CSharedMemory::setMesEthanol(int val) const
{
    capt->mesEthanol = val;
}

void CSharedMemory::setMesRawTVOC(int val) const
{
    capt->mesRawTVOC = val;
}

void CSharedMemory::setMesRawECO2(int val) const
{
    capt->mesRawECO2 = val;
}

void CSharedMemory::setMesBaselineTVOC(int val) const
{
    capt->mesBaselineTVOC = val;
}

void CSharedMemory::setMesBaselineECO2(int val) const
{
    capt->mesBaselineECO2 = val;
}

void CSharedMemory::setCapteurGazFumee(bool val) const
{
    capt->capteurGazFumee = val;
}

void CSharedMemory::setCapteurPresence(bool val) const
{
    capt->capteurPresence = val;
}

void CSharedMemory::setMesTemp(float val) const
{
    capt->mesTemp = val;
}
