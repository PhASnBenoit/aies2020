#include "cpa.h"

CPa::CPa(QObject *parent, CBdd *bdd):
    QObject(parent)
{
    led = new CLed(this);
    ecran = new CEcran(this);

    captPres = new CCapteurPres(this);
    connect(captPres, SIGNAL(sigAffPres(int)), this, SLOT(onSigPresence(int)));

    captTemp = new CCapteurTemp();

    emIr = new CIr(this);
    mTimerU = new QTimer(this); // timer de vérif de la présence tension.
    mTimerU->setInterval(30000);  // 20s
    connect(mTimerU, SIGNAL(timeout()), this, SLOT(onTimerU()));
    mConsigne = ALLUMER;

    mBdd = bdd;
    mMac = getSysMacAddress();
    mNom = bdd->getNomPa(mMac);
    mZone = bdd->getNomZone(bdd->getNoZone(mMac).toInt()); // Tester simplement bdd->getNoZone(mac)
    mEtatTele = true;  // par défaut allumée
    mIdleTime = bdd->getIdleTime(mMac);

}

CPa::~CPa()
{
    delete mTimerU;
    delete captTemp;
    delete captPres;
    delete ecran;
    delete led;
}

void CPa::onSigPresence(int st)
{
    emit sigPresence(st);
}

void CPa::onTimerU()
{
    mEtatTele = ecran->getU();
    if (mEtatTele != mConsigne) {
        emIr->irOnOff();
        emit sigPa("IR(Rappel)");
        qDebug() << "[CPa::onTimerU]: Bascule rappel IR";
        if (mConsigne) { // si consigne=allumer
            ecran->putOnCec();
            ecran->putOnRs();
        } else {
            ecran->putOffCec();
            ecran->putOffRs();
        } // else
    } else
        mTimerU->stop();
} // onTimerU

// Récupére l'adresse mac de la carte
QString CPa::getSysMacAddress()
{
    char buffer[1000];
    FILE *file_macAddress;
    if ((file_macAddress=fopen("/sys/class/net/eth0/address","r"))==nullptr){
        exit(1);
    } // if lecture du fichier contenant l'adresse mac
    fscanf(file_macAddress,"%[^\n]",buffer);
    fclose(file_macAddress);
    return buffer; // renvoie l'adresse mac
}

QString CPa::getZone()
{
    return mZone;
}

QString CPa::getNom()
{
    return mNom;
}

QString CPa::getModeFonc()
{
    mModeFonc = mBdd->getModeFonc(mMac);
    return mModeFonc;
}

QString CPa::getCtrlTv()
{
    mCtrlTv = mBdd->getCtrlTv(mMac);
    return mCtrlTv;
}

QString CPa::getHstart()
{
    mHstart = mBdd->getHstart(mMac);
    return mHstart;
}

QString CPa::getHstop()
{
    mHstop = mBdd->getHstop(mMac);
    return mHstop;
}

QString CPa::getPermaUpdateVer()
{
    mPermaUpdateVer = mBdd->getPermaUpdateVer();
    return mPermaUpdateVer;
}

QString CPa::getDiffUpdateVer()
{
    mDiffUpdateVer = mBdd->getDiffUpdateVer();
    return mDiffUpdateVer;
}

QString CPa::getPermaUpdateDate()
{
    mPermaUpdateDate = mBdd->getPermaUpdateDate();
    return mPermaUpdateDate;
}

QString CPa::getDiffUpdateDate()
{
    mDiffUpdateDate = mBdd->getDiffUpdateDate();
    return mDiffUpdateDate;
}

void CPa::switchDiffToPerma()
{
    mBdd->switchDiffToPerma();
}

bool CPa::switchOnTv()
{
//     setEtatTele(true);
     QString ctrl=getCtrlTv();
     mConsigne=ALLUMER;

     if(ctrl == "cec") {
        ecran->putOnCec();
        emit sigPa("CEC_On");
        qDebug() << "[CPa::switchOnTv]: On par CEC";
     }
     if(ctrl == "ir") {
         emIr->irOnOff();
         emit sigPa("IR_On");
         qDebug() << "[CPa::switchOnTv]: On par IR";
     }
     if(ctrl == "rs") {
         ecran->putOnRs();
         emit sigPa("RS_On");
         qDebug() << "[CPa::switchOnTv]: On par RS";
     } //if rs
     mTimerU->start();
     return true;
}

bool CPa::switchOffTv()
{
    //    setEtatTele(false);
    mConsigne = ETEINDRE;
    QString ctrl=getCtrlTv();
    if(ctrl == "cec") {
        ecran->putOffCec();
        emit sigPa("CEC_Off");
        qDebug() << "[CPa::switchOffTv]: Off par CEC";
    }
    if(ctrl == "ir") {
        emIr->irOnOff();
        emit sigPa("IR_Off");  // pour affichage
        qDebug() << "[CPa::switchOffTv]: Off par IR";
    }
    if(ctrl == "rs") {
        ecran->putOffRs();
        emit sigPa("RS_Off");
        qDebug() << "[CPa::switchOffTv]: Off par RS";
    }
    mTimerU->start();
    return false;
}

bool CPa::switchOnLed()
{
    return led->ledOn();
}

bool CPa::switchOffLed()
{
    return led->ledOff();
}


bool CPa::getPresence()
{
    return captPres->getPresence();
}

float CPa::getTemperature()
{
    float temp = captTemp->getTemp();
    return temp;
}

int CPa::getIdleTime()
{
    return mIdleTime;
}

bool CPa::isItTheMomentToStart()
{
    QDateTime dateTime;
    dateTime = QDateTime::currentDateTime();
    QDateTime day = QDateTime::fromString(dateTime.toString(), "ddd");
    QString hStart = getHstart();
    QDateTime qhStart = QDateTime::fromString(hStart,"hh:mm");

    if((day.toString() != "Sat") && (day.toString() != "Sun"))
    {
        if((dateTime.toString("hh:mm") == qhStart.toString("hh:mm")) && (!mEtatTele))
        {
            return true;
        }
    }
    return false;
}

bool CPa::isItTheMomentToStop()
{
    QDateTime dateTime;
    dateTime = QDateTime::currentDateTime();
    QDateTime day = QDateTime::fromString(dateTime.toString(), "ddd");
    QString hStop = getHstop();
    QDateTime qhStop = QDateTime::fromString(hStop,"hh:mm");

    if((day.toString() != "Sat") && (day.toString() != "Sun"))
    {
        qDebug() << "CPa::isTheMomentToStop : mEtatTele :" << mEtatTele;
        if((dateTime.toString("hh:mm") == qhStop.toString("hh:mm")) && (mEtatTele))
        {
            return true;
        }
    }
    return false;
}

bool CPa::getEtatTele()
{
    return mEtatTele;
}

void CPa::setEtatTele(bool state)
{
    mConsigne = state;
}

void CPa::setIdleTime(int idleTime)
{
    mIdleTime = idleTime;
}

QDateTime CPa::getDateTime()
{
    QDateTime dateTime;
    dateTime = QDateTime::currentDateTime();
    return dateTime;
}

QString CPa::getMac()
{
    return mMac;
}

QByteArray CPa::getSDPlace()
{
 QProcess sh; //on crée ici sh qui est le processus qui va nous permettre de faire une commande en shell
 sh.start("sh", QStringList() << "-c" << "df / |tr -s ' ' |cut -d ' ' -f5 |head -n 2 |tail -n 1 |tr -d '%'l");
 // nous faisons ci-dessus la commande qui nous permet de récuperer seulement le pourcentage de la place restante dans la SD
 sh.waitForFinished(); //nous attendons le retour de la commande
 QByteArray output = sh.readAll();
 sh.close();//kill le processus
 qDebug() << output;
 return output;
}

int CPa::getUrgency()
{
    int mUrgency = mBdd->getUrgencyState();
    return mUrgency;
}

void CPa::creationCache()
{
  int i =0;
  QList<QString> videoSlide = mBdd->creationCacheVideo(mMac);
  //QString command("scp root@192.168.1.39:"+path+" /home/pi/cache/");
  QProcess scp;

  for(i =0;i < videoSlide.size();i++)
  {
    scp.startDetached("sh", QStringList() << "-c" << "scp root@192.168.1.15:/srv/www/htdocs/2020"+videoSlide.at(i)+" /home/pi/cache/");
    //ici nous utilison la commande scp pout récupérer les vidéos stocker sur le serveur selons leurs chemin d accès
    qDebug() << "Je suis passé" << i;
    scp.close();
  }

}
