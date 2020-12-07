#include "ctelecommanders232samsung.h"

CTelecommandeRs232Samsung::CTelecommandeRs232Samsung(QObject *parent, const QString &nomRs232) :
    QObject(parent)
{
    ouvrirRs232(nomRs232);  //ouverture du port série
}

CTelecommandeRs232Samsung::~CTelecommandeRs232Samsung()
{
    delete rs;
}

int CTelecommandeRs232Samsung::ouvrirRs232(const QString &nomRs232)
{
    rs = new CRs232c(this, nomRs232);
    connect(rs, SIGNAL(sigErreur(QSerialPort::SerialPortError)), this, SLOT(onErreur(QSerialPort::SerialPortError)));
    connect(rs, SIGNAL(sigReponse(unsigned char, unsigned char, unsigned char, unsigned char)), this, SLOT(onReponse(unsigned char, unsigned char, unsigned char, unsigned char)));
    rs->initialiser(QSerialPort::Baud9600, QSerialPort::Data8, QSerialPort::NoParity,
                    QSerialPort::OneStop, QSerialPort::NoFlowControl);
    bool res=rs->ouvrirPort();
    if (!res) {
        emit sigErreur(ERREUR, "CTelecommandeRs232Samsung::ouvrirRs232 port non ouvert.");
        delete rs;
    }
    return res;
}

int CTelecommandeRs232Samsung::setAlimentation(int id, bool alim)
{
    if (setId(id) == ERREUR) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::setAlimentation bad id !");
        return ERREUR;
    }
    mTrame[0] = 0xAA;   // entête
    mTrame[1] = 0x11;   // Commande contrôle alimentation
    mTrame[2] = id;     // id écran
    mTrame[3] = 0x01;   // longueur des données
    mTrame[4] = (alim?0x01:0x00);   // ON ou OFF
    mTrame[5] = getSommeDeControle(5);   // somme de contrôle
    debugAffiche(6);

    // emission de la trame
    if (rs->ecrire(mTrame, 6) == -1) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::setAlimentation pb ecrire.");
        return ERREUR;
    } // if ecrire
    return OK;
}

bool CTelecommandeRs232Samsung::getEtatAlimentation(int id)
{
    if (setId(id) == ERREUR) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::getAlimentation bad id !");
        return ERREUR;
    }
    mTrame[0] = 0xAA;   // entête
    mTrame[1] = 0x11;   // Commande contrôle alimentation
    mTrame[2] = id;     // id écran
    mTrame[3] = 0x00;   // longueur des données
    mTrame[4] = getSommeDeControle(4);   // somme de contrôle
    debugAffiche(5);

    // emission de la trame
    if (rs->ecrire(mTrame,5) == -1) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::getEtatAlimentation ecrire.");
        return ERREUR;
    } // if ecrire
    return OK;
}

int CTelecommandeRs232Samsung::setSourceEntree(int id, E_SOURCE_ENTREE src)
{
    if (setId(id) == ERREUR) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::setSourceEntree bad id !");
        return ERREUR;
    }
    mTrame[0] = 0xAA;   // entête
    mTrame[1] = 0x14;   // Commande contrôle source d'entrée
    mTrame[2] = id;     // id écran
    mTrame[3] = 0x01;   // longueur des données
    mTrame[4] = src;   // source
    mTrame[5] = getSommeDeControle(5);   // somme de contrôle
    debugAffiche(6);

    if (rs->ecrire(mTrame,6) == -1) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::setSourceEntree ecrire.");
        return ERREUR;
    } // if ecrire
    return OK;
}

int CTelecommandeRs232Samsung::getSourceEntree(int id)
{
    if (setId(id) == ERREUR) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::getSourceEntree bad id !");
        return ERREUR;
    }
    mTrame[0] = 0xAA;   // entête
    mTrame[1] = 0x14;   // Commande contrôle source d'entrée
    mTrame[2] = id;     // id écran
    mTrame[3] = 0x00;   // longueur des données
    mTrame[4] = getSommeDeControle(4);   // somme de contrôle
    debugAffiche(5);
    if (rs->ecrire(mTrame,5) == -1) {
        sendErreurTc(ERREUR, "CTelecommandeRs232Samsung::getSourceEntree ecrire.");
        return ERREUR;
    } // nbr
    return OK;
}

 QString CTelecommandeRs232Samsung::getSource(E_SOURCE_ENTREE numSource)
{
    QString source;
    switch(numSource) {
    case PC: source="PC"; break;
    case DVI: source="DVI"; break;
    case SOURCE_IN: source="SOURCE_IN"; break;
    case COMPOSANT: source="COMPOSANT"; break;
    case MAGIC_INFO: source="MAGIC_INFO"; break;
    case DVI_VIDEO: source="DVI_VIDEO"; break;
    case HDMI1: source="HDMI1"; break;
    case HDMI2: source="HDMI2"; break;
    case HDMI_PC: source="HDMI_PC"; break;
    case HDMI2_PC: source="HDMI2_PC"; break;
    case HDBT: source="HDBT"; break;
    default: source="INCONNUE";
    } // sw
    return source;
 }

 void CTelecommandeRs232Samsung::onErreur(QSerialPort::SerialPortError noErr)
 {
     emit sigErreur(noErr, "Erreur CRs232c");
 }

 void CTelecommandeRs232Samsung::onReponse(unsigned char octet0, unsigned char octet1, unsigned char octet4, unsigned char octet6)
 {
     qDebug() << QString::number(octet0,16) << QString::number(octet1,16) << " " << QString::number(octet4,16) << QString::number(octet6,16);
     if (octet0 != ENTETE) {
         emit sigErreur(ERREUR, "Mauvais format de trame !");
         return;
     }
     switch (octet1) {
     case 0x11:  // alimentation
         if (octet4 == 'A') emit sigEtatAlimentation(octet6);
         else emit sigErreur(octet6, "Erreur NOK");
         break;
     case 0x14:  // source entrée
         break;
     case 0xFF:
         if (octet4 == 'A') emit sigEtatAlimentation(octet6);
         else emit sigErreur(octet6, "Erreur NOK");
         break;
         break;
     default:
         emit sigErreur(ERREUR, "Fonction de trame inconnue !");
     } //sw
 }

E_SOURCE_ENTREE CTelecommandeRs232Samsung::getCodeSource(QString nomSource)
{
    E_SOURCE_ENTREE code=INCONNU;
    if (nomSource=="PC") code=PC;
    if (nomSource=="DVI") code=DVI;
    if (nomSource=="SOURCE_IN") code=SOURCE_IN;
    if (nomSource=="COMPOSANT") code=COMPOSANT;
    if (nomSource=="MAGIC_INFO") code=MAGIC_INFO;
    if (nomSource=="DVI_VIDEO") code=DVI_VIDEO;
    if (nomSource=="HDMI1") code=HDMI1;
    if (nomSource=="HDMI2") code=HDMI2;
    if (nomSource=="HDMI_PC") code=HDMI_PC;
    if (nomSource=="HDMI2_PC") code=HDMI2_PC;
    if (nomSource=="HDBT") code=HDBT;
    return code;
}

char CTelecommandeRs232Samsung::getSommeDeControle(int lg)
{
 int somme=0;
 for (int i=1 ; i<lg ; i++)
     somme += mTrame[i];
 return (char)somme;
}

int CTelecommandeRs232Samsung::setId(int id)
{
    if ( (id<0) || (id>0xFE) )
        return ERREUR;
    return OK;
}

void CTelecommandeRs232Samsung::debugAffiche(int lg)
{
    QString aff;
    for (int i=0 ; i<lg ; i++)
        aff += QString::number(mTrame[i],16)+" ";
    emit sigAffTrame(aff);
    qDebug() << aff;
}

void CTelecommandeRs232Samsung::sendErreurTc(char noErr, QString lieu)
{
    emit sigErreur(noErr, lieu);
    qDebug() << noErr;
}
