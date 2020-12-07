#include "crs232c.h"

CRs232c::CRs232c(QObject *parent, const QString &nomPort)
{
  qDebug() << nomPort;
  mSp = new QSerialPort(parent);
  mSp->setPortName(nomPort);
  connect(mSp, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(mSp, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onErreur(QSerialPort::SerialPortError)));
}

CRs232c::~CRs232c()
{
    mSp->close();
    delete mSp;
}

int CRs232c::initialiser(QSerialPort::BaudRate vitesse, QSerialPort::DataBits data,
                         QSerialPort::Parity parity, QSerialPort::StopBits nbStop,
                         QSerialPort::FlowControl flow)
{
    mSp->setBaudRate(vitesse);
    mSp->setDataBits(data);
    mSp->setParity(parity);
    mSp->setStopBits(nbStop);
    mSp->setFlowControl(flow);
    return 0;
}

int CRs232c::ouvrirPort()
{
    bool res=false;
    res=mSp->open(QIODevice::ReadWrite);
    if (!res) {
        qDebug() << "CRs232c::ouvrirPort Erreur ouverture port.";
        mSp->close();
    } // if res
    return mSp->isOpen();
}

char CRs232c::lire(char *trame, int nbOctets)
{
    if (nbOctets < 1) return ERREUR;
/*
    int res;
    if (to)
        res = mSp->waitForReadyRead(TO); // 3s d'attente max
    if (!res) {
        mRec=false;
        qDebug() << "CRs232c::lire rien à lire.";
        return ERREUR;
    } // if res
    QThread::msleep(1000);

#define ATTENTE 50

    quint32 cpt=0;  // on attend le bon nombre de caractères
    int nba=0;
    while( ((nba=mSp->bytesAvailable())<nbOctets) && cpt<ATTENTE) {
        cpt++;
        qDebug() << nba << "cars arrivés";
        QThread::msleep(100);
    } // while
    qDebug() << "cpt=" << cpt;
    if (cpt==ATTENTE) {
        mRec=false;
        qDebug() << "CRs232c::lire temps écoulé.";
        return ERREUR;
    } // if si trop attendu
    int nb = mSp->read(trame, nbOctets);
    qDebug() << nb << "CRs232c::lire cars lus";
    if (nb < nbOctets) {
        mRec=false;
        qDebug() << "CRs232c::lire Pas assez de caractères.";
        return ERREUR;
    } // if res
    mRec=false;
*/    return OK;

}

int CRs232c::ecrire(char *trame, int nbOctets)
{
    mRec = true; // les futurs cars arrivant sont valides
    int lg = mSp->write(trame, nbOctets);
    mSp->waitForBytesWritten(2000);
    if ( lg < nbOctets) {
        qDebug() << "CRs232c::ecrire write KO...";
        return ERREUR;
    } // if erreur
    qDebug() << "Envoyé";
    return OK;
}

void CRs232c::onReadyRead()
{
    int nb=0;
    if (mRec) {
       nb = mSp->bytesAvailable();
       qDebug() << nb << "CRs232c::onReadyRead arrivés.";
       if (nb >= 8) { // réponse complète
           QByteArray ba = mSp->read((quint64)8);
           for (int i=0 ; i<8 ; i++) qDebug() << QString::number((unsigned char)ba[i],16) << " ";
           mRec = false;
           emit sigReponse(ba[0], ba[1], ba[4], ba[6]);
       } // if nb
    } else {
        qDebug() << mSp->bytesAvailable() << "CRs232c::onReadyRead cars parasites arrivés, je purge.";
        mSp->readAll();
    } // else mRec
}

void CRs232c::onErreur(QSerialPort::SerialPortError err)
{
    emit sigErreur(err);
    qDebug() << "CRs232c::onErreur " << err;
}
