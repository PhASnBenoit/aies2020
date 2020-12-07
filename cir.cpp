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
    QProcess::execute("irsend SEND_ONCE SAMSUNG KEY_POWER"); // Commande utilisation de la télécommande virtuel lire
}


