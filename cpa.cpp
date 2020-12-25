#include "cpa.h"

CPa::CPa(QObject *parent, CBdd *bdd):
    QObject(parent)
{
    led = new CLed(this);
    ecran = new CEcran(this);

    mConsigne = ALLUMER;
    mOrdrePassed=false;
    mEtatReelTele = ecran->getU();
    //m_etatOrdre = ORDRE_ATTENTE;

//    captPres = new CCapteurPres(this);
    connect(captPres, &CCapteurPres::sigAffPres, this, &CPa::onSigPresence);
//    captTemp = new CCapteurTemp();

    emIr = new CIr(this);
    mSgp30.begin();
    emit sigQAir(getQSerialNumber());

    mBdd = bdd;
    mMac = getSysMacAddress();
    mNom = bdd->getNomPa(mMac);
    mZone = bdd->getNomZone(bdd->getNoZone(mMac).toInt()); // Tester simplement bdd->getNoZone(mac)
    mIdleTime = bdd->getIdleTime(mMac);

    mTimerU = new QTimer(this); // timer de vérif de la présence tension.
    mTimerU->setInterval(30000);  // 30s
    connect(mTimerU, &QTimer::timeout, this, &CPa::onTimerU);
    mTimerU->start();
}

CPa::~CPa()
{
    delete mTimerU;
//    delete captTemp;
//    delete captPres;
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

bool CPa::getConsigne() const
{
    return mConsigne;
}

void CPa::setConsigne(bool consigne)
{
    mConsigne = consigne;
}

QString CPa::getQSerialNumber()
{
    return QString::number(mSgp30.serialnumber[0],16)+QString::number(mSgp30.serialnumber[1],16)+QString::number(mSgp30.serialnumber[2],16);
}

QString CPa::getQuality(QString choix)
{
    static int cpt=0;
    QString chSig, chRaw, chBase;

    if (! mSgp30.IAQmeasure())
       return("Mes failed");
    chSig = "TVOC:"+QString::number(mSgp30.TVOC)+" ppb ";
    chSig += "eCO2:"+QString::number(mSgp30.eCO2)+" ppm";

    // Signaux utilisés pour la calibration
    if (! mSgp30.IAQmeasureRaw())
        return("Raw failed");
    chRaw = "Q: H2:"+QString::number(mSgp30.rawH2)+" ";
    chRaw += "Eth:"+QString::number(mSgp30.rawEthanol);

    cpt++;
    if (cpt == 20) {
        uint16_t TVOC_base, eCO2_base;
        if (! mSgp30.getIAQBaseline(&eCO2_base, &TVOC_base))
          return("Base Failed");
        chBase = "BS: eCO2:"+QString::number(eCO2_base)+" ppm ";
        chBase +="TVOC:"+QString::number(TVOC_base)+" ppb";
    } // if cpt
    if(choix=="tous")
        switch(cpt) {
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:return chSig;
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:cpt=0;
            return chBase;
        default:
            return chRaw;
        } // sw
    if(choix=="sig")
        return chSig;
    if(choix=="raw")
        return chRaw;
    if(choix=="base")
        return chBase;
return "-"; //sert à rien !
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
    mOrdrePassed=false;
    qDebug() << "[CPa::onTimerU] mEtatReelTele:"<<mEtatReelTele
             << " mConsigne:"<< mConsigne;
    if (mEtatReelTele != mConsigne) { // si au bout de 30s l'ordre est toujours pas exécuté
        qDebug() << "[CPa::onTimerU]: Relance ordre TV !";
        if  (mConsigne == ALLUMER)
            switchOnTv();
        else // ETEINDRE:
            switchOffTv();
    } // if != consigne
} // onTimerU

bool CPa::switchOnTv()
{
     QString ctrl=getCtrlTv();
     mConsigne=ALLUMER;

     if (!mOrdrePassed && !getEtatReelTv()) {
         mOrdrePassed=true;
         if(ctrl == "cec") {
            ecran->putOnCec();
            emit sigPaConsigne("CEC_On");
            qDebug() << "[CPa::switchOnTv]: On par CEC";
         }
         if(ctrl == "ir") {
             emIr->irOnOff();
             emit sigPaConsigne("IR_On");
             qDebug() << "[CPa::switchOnTv]: On par IR";
         }
         if(ctrl == "rs") {
             ecran->putOnRs();
             emit sigPaConsigne("RS_On");
             qDebug() << "[CPa::switchOnTv]: On par RS";
         } //if rs
         mTimerU->start();
    } // if m_etatOrdre
    return true;
} // switchOnTv

bool CPa::switchOffTv()
{
    QString ctrl=getCtrlTv();
    mConsigne = ETEINDRE;

    if (!mOrdrePassed && getEtatReelTv()) {  // PhA 12/2020
        mOrdrePassed=true;
        if(ctrl == "cec") {
            ecran->putOffCec();
            emit sigPaConsigne("CEC_Off");
            qDebug() << "[CPa::switchOffTv]: Off par CEC";
        }
        if(ctrl == "ir") {
            emIr->irOnOff();
            emit sigPaConsigne("IR_Off");  // pour affichage
            qDebug() << "[CPa::switchOffTv]: Off par IR";
        }
        if(ctrl == "rs") {
            ecran->putOffRs();
            emit sigPaConsigne("RS_Off");
            qDebug() << "[CPa::switchOffTv]: Off par RS";
        }
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
        if((dateTime.toString("hh:mm") == qhStart.toString("hh:mm")) && (!mEtatReelTele)) {
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
    if((dateTime.toString("hh:mm") == qhStop.toString("hh:mm")) && (mEtatReelTele)) {
        return true;
    }
    return false;
}

bool CPa::getEtatReelTv()
{
    return mEtatReelTele;
}

void CPa::setEtatReelTv(bool state)
{
    mEtatReelTele = state;
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
 //qDebug() << "[CPa::getSDPlace] Lecture SD restant.";
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
  for(i=0 ; i<videoSlide.size() ; i++) {
    QString command="scp root@"+conf.getBddHostname()+":/srv/www/htdocs"+
            conf.getHtDocs()+videoSlide.at(i)+" /opt/aies/cache/";
    qDebug() << "CPa::creationCache: " << command;
    scp.startDetached(command);
    //ici nous utilisons la commande scp pour récupérer les vidéos stockées sur le serveur selon leur chemin d accès
    //qDebug() << "CPa::creationCache: Met en cache ress : " << videoSlide.at(i);
    scp.close();
  } // for
}
