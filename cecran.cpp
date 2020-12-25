#include "cecran.h"

CEcran::CEcran(QObject *parent, int no) :
    QObject(parent)
{
    mNoEcran = no; // numéro de l'écran dynamique
    mProt = new CTelecommandeRs232Samsung(this, "/dev/ttyAMA0");
    mUTv = new CGpio(UTV);
}

CEcran::~CEcran()
{
    delete mUTv;
    delete mProt;
}

bool CEcran::getU()
{
  mValU = mUTv->lire();
  return mValU;
}

int CEcran::putOnCec()
{
    system("echo \"on 0\" | cec-client -s -d 1");   //allumage du téléviseur
    return 0;

}

int CEcran::putOffCec()
{
    system("echo \"standby 0\" | cec-client -s -d 1");   //mise en veille du téléviseur
    return 0;

}

int CEcran::putOnRs()
{
    mProt->setAlimentation(mNoEcran, ON);
    return 0;

}

int CEcran::putOffRs()
{
    mProt->setAlimentation(mNoEcran, OFF);
    return 0;

}
