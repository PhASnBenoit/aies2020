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
    mem = static_cast<T_CAPTEURS *>(shm->data());
    bzero(mem, sizeof(T_CAPTEURS));
}

CSharedMemory::~CSharedMemory()
{
    shm->detach();
    delete shm;
}

int CSharedMemory::getMesTVOC() const
{
    return mem->mesTVOC;
}

int CSharedMemory::getMesECO2() const
{
    return mem->mesECO2;
}

int CSharedMemory::getMesRawH2() const
{
    return mem->mesRawH2;
}

int CSharedMemory::getMesRawEthanol() const
{
    return mem->mesRawEthanol;
}

int CSharedMemory::getMesBaselineTVOC() const
{
    return mem->mesBaselineTVOC;
}

int CSharedMemory::getMesBaselineECO2() const
{
    return mem->mesBaselineECO2;
}


bool CSharedMemory::getCapteurGazFumee() const
{
    return mem->capteurGazFumee;
}


bool CSharedMemory::getCapteurPresence() const
{
    return mem->capteurPresence;
}


float CSharedMemory::getMesTemp() const
{
    return mem->mesTemp;
}

bool CSharedMemory::getCapteurUTv() const
{
    return mem->capteurUTv;
}
///////////////////////////////////////////////
void CSharedMemory::setMesTVOC(int val)
{
    shm->lock();
    mem->mesTVOC = val;
    shm->unlock();
}

void CSharedMemory::setMesECO2(int val)
{
    shm->lock();
    mem->mesECO2 = val;
    shm->unlock();
}

void CSharedMemory::setMesRawH2(int val)
{
    shm->lock();
    mem->mesRawH2 = val;
    shm->unlock();
}

void CSharedMemory::setMesRawEthanol(int val)
{
    shm->lock();
    mem->mesRawEthanol = val;
    shm->unlock();
}

void CSharedMemory::setMesBaselineTVOC(int val)
{
    shm->lock();
    mem->mesBaselineTVOC = val;
    shm->unlock();
}

void CSharedMemory::setMesBaselineECO2(int val)
{
    shm->lock();
    mem->mesBaselineECO2 = val;
    shm->unlock();
}

void CSharedMemory::setCapteurGazFumee(bool val)
{
    shm->lock();
    mem->capteurGazFumee = val;
    shm->unlock();
}

void CSharedMemory::setCapteurPresence(bool val)
{
    shm->lock();
    mem->capteurPresence = val;
    shm->unlock();
}

void CSharedMemory::setMesTemp(float val)
{
    shm->lock();
    mem->mesTemp = val;
    shm->unlock();
}


void CSharedMemory::setCapteurUTv(bool value)
{
    shm->lock();
    mem->capteurUTv = value;
    shm->unlock();
}
