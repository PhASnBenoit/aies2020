#include "cecran.h"

CEcran::CEcran(QObject *parent, int no) :
    QObject(parent)
{
    mNoEcran = no; // numéro de l'écran dynamique
    mProt = new CTelecommandeRs232Samsung(this, "/dev/ttyAMA0");
}

CEcran::~CEcran()
{
    delete mProt;
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
