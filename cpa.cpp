#include "cpa.h"

CPa::CPa(QObject *parent, CBdd *bdd):
    QObject(parent)
{
    led = new CLed(this);
    ecran = new CEcran(this);

    captPres = new CCapteurPres(this);
    connect(captPres, &CCapteurPres::sigAffPres, this, &CPa::onSigPresence);

    captTemp = new CCapteurTemp();

    emIr = new CIr(this);

    mTimerU = new QTimer(this); // timer de vérif de la présence tension.
    mTimerU->setInterval(30000);  // 30s
    connect(mTimerU, &QTimer::timeout, this, &CPa::onTimerU);
    mConsigne = ALLUMER;

    m_etatOrdre = ORDRE_ATTENTE;
    mBdd = bdd;
    mMac = getSysMacAddress();
    mNom = bdd->getNomPa(mMac);
    mZone = bdd->getNomZone(bdd->getNoZone(mMac).toInt()); // Tester simplement bdd->getNoZone(mac)
    mEtatTele = TV_ON;  // par défaut allumée
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

int CPa::getEtatOrdre() const
{
    return m_etatOrdre;
}

void CPa::setEtatOrdre(int etatOrdre)
{
    m_etatOrdre = etatOrdre;
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

void CPa::onTimerU()
{
    mEtatTele = ecran->getU();
    if (mEtatTele != mConsigne) { // si au bout de 30s l'ordre est toujours pas exécuté
        m_etatOrdre = ORDRE_PASSED;
        emit sigPa("[CPa::onTimerU]: Rappel ordre TV ! Etat TV : " + QString::number(mEtatTele));
        switch(static_cast<int>(mConsigne)) {
        case ALLUMER:
            switchOnTv();
            break;
        case ETEINDRE:
            switchOffTv();
            break;
        } // sw
    } else {
        mTimerU->stop();
        m_etatOrdre = ORDRE_ATTENTE;
    } // else
} // onTimerU

bool CPa::switchOnTv()
{
     QString ctrl=getCtrlTv();
     mConsigne=ALLUMER;
     if (m_etatOrdre != ORDRE_EN_COURS) {
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
         m_etatOrdre = ORDRE_EN_COURS;
         mTimerU->start();
    } // if m_etatOrdre
    return true;
} // switchOnTv

bool CPa::switchOffTv()
{
    QString ctrl=getCtrlTv();
    mConsigne = ETEINDRE;

    if (m_etatOrdre != ORDRE_EN_COURS) {
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
        m_etatOrdre = ORDRE_EN_COURS;
        mTimerU->start();
    } // if m_etatOrdre
    return false;
} // switchOffTv

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

    if((day.toString() != "Sat") && (day.toString() != "Sun")) {
        if((dateTime.toString("hh:mm") == qhStart.toString("hh:mm")) && (!mEtatTele)) {
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

    //qDebug() << "CPa::isTheMomentToStop : mEtatTele :" << mEtatTele;
    if((dateTime.toString("hh:mm") == qhStop.toString("hh:mm")) && (mEtatTele)) {
        return true;
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

void CPa::calculateSDPlace()
{
    qDebug() << "[CPa::getSDPlace] Calcul de SD restant.";
    QProcess sh; //on crée ici sh qui est le processus qui va nous permettre de faire une commande en shell
    //sh.setStandardOutputFile("/opt/aies/sd.txt");
    sh.start("sh", QStringList() <<  "-c" << "df / |tr -s ' ' |cut -d ' ' -f5 |head -n 2 |tail -n 1 |tr -d '%'");
    //sh.start("sh -c df / |tr -s ' ' |cut -d ' ' -f5 | head -n 2 | tail -n 1 | tr -d '%'");
    //nous faisons ci-dessus la commande qui nous permet de récuperer seulement le pourcentage de la place restante dans la SD
    sh.waitForFinished(); // pas besoin d'attendre, on lira le fichier produit
    mSDPlace = sh.readAll();
    qDebug() << "[CPa::getSDPlace] " << mSDPlace;
    sh.close(); //kill le processus
} // caculateSDPlace

QByteArray CPa::getSDPlace()
{
 qDebug() << "[CPa::getSDPlace] Lecture SD restant dans /opt/aies/sd.txt";
 /*
 QFile fic("/opt/aies/sd.txt");
 if (!fic.open(QIODevice::ReadOnly | QIODevice::Text))
    return QByteArray("0");
 QByteArray line = fic.readAll();
 fic.close();
 return line;
 */
 return mSDPlace;
} // getSDPlace

int CPa::getUrgency()
{
    int mUrgency = mBdd->getUrgencyState();
    return mUrgency;
}

void CPa::creationCache()
{
  int i =0;
  QList<QString> videoSlide = mBdd->getImagesVideos(mMac);
  QProcess scp;
  CConfig conf;

  qDebug() << "Chargement du cache.";
  for(i=0 ; i<videoSlide.size() ; i++)
  {
    QString command="scp root@"+conf.getBddHostname()+":/srv/www/htdocs"+
            conf.getHtDocs()+videoSlide.at(i)+" /opt/aies/cache/";
    qDebug() << "CPa::creationCache: " << command;
    scp.startDetached(command);
    //ici nous utilisons la commande scp pour récupérer les vidéos stockées sur le serveur selon leur chemin d accès
    //qDebug() << "CPa::creationCache: Met en cache ress : " << videoSlide.at(i);
    scp.close();
  }

}
