#include "cir.h"

CIr::CIr(QObject *parent) :
    QObject(parent)
{

}

CIr::~CIr()
{

}

void CIr::irOnOff()
{
    QProcess::startDetached("irsend", QStringList() << "SEND_ONCE SAMSUNG KEY_POWER"); // Commande utilisation de la télécommande virtuel lire
}


