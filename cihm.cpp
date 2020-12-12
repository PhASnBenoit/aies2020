#include "cihm.h"
#include "ui_cihmappaies2020.h"

CIhm::CIhm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CIhmAppAies2020)
{
    // initialisation IHM
    ui->setupUi(this);
    ui->lLogoIr->setVisible(false);
    ui->lLogoPres->setVisible(true); // par défaut
    QPixmap logoPath("logoLAB.png");
    QPixmap logoPath1("presence1.png");
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));  // Rend invisible le curseur

    // Initialisation des objets
    mUpdate=false;
    bdd = new CBdd();
    pa = new CPa(this, bdd);
    connect(pa, &CPa::sigPresence, this, &CIhm::onSigPresence); // aff * presence
    connect(pa, &CPa::sigPa, this, &CIhm::onSigPa);  // aff on/off tv
    conf = new CConfig();
    mPresence = false;
    mPriorSwap=false; // pas de priorité pour le moment
    mVer = conf->getNumVersion();
    mServeur = conf->getBddHostname();

    mac = pa->getMac();  // Stockage de l'adresse MAC dans une variable global

    currentName = pa->getNom();   // Acquisition nom de la Rpi par l'adresse MAC
    //Initialisation UI fihm
    ui->lLogo->setPixmap(logoPath);
    ui->lPa->setText(pa->getNom());
    ui->lZone->setText(pa->getZone());
    ui->lVersion->setText(mVer);
    ui->lLogoPres->setPixmap(logoPath1);

    // Timer général
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerHeure);  // affichage de l'heure
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerTemperature); // affichage température
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerBdd); // sauve temp, presence, capcite SD dans bdd
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerUpdate); // mise à jour du logiciel
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerFlash);  // affichage de l'info flash

    // Timer Slide
    timerSlide = new QTimer(); // changement de  slide
    connect(timerSlide, &QTimer::timeout, this, &CIhm::onTimerSlide);

    timerIdleSlide = new QTimer();  // pour les diapos oups !
    connect(timerIdleSlide, &QTimer::timeout, this, &CIhm::onTimerIdleSlide);

    // Timer d'intervale d'absence
    QString modeFonc = pa->getModeFonc();
    if( modeFonc == "presence")
    {
        connect(timer, &QTimer::timeout, this, &CIhm::onTimerCapteur); // toutes les sec
        timerPresence = new QTimer(this);
        connect(timerPresence, &QTimer::timeout, this, &CIhm::onTimerPresence);
    } else { // heure depart et de fin
        connect(timer, &QTimer::timeout, this, &CIhm::onTimerOpenHour);
    }

    timer->start(1000); // Timer général

    // affichage mode fonc et type commande écran
    ui->lModeFonc->setText(modeFonc);
    ui->lType->setText(pa->getCtrlTv());

    //Initialisation Première Slide
    ui->webViewStarter->page()->settings()->setObjectCacheCapacities(0,0,0); // suppression cache
    mCompteurSlide = 0;
    getSlide();
} // fihm

CIhm::~CIhm()
{
    delete timer;
    delete timerPresence;
    delete timerSlide;
    delete timerIdleSlide;
    delete maj;
    delete flash;
    delete pa;
    delete bdd;
    delete conf;
    delete ui;
}

// Récupere l'heure actuel
void CIhm::onTimerHeure()
{
    QTime Heure;
    Heure = QTime::currentTime();
    if (ui->lbPoints->isVisible())
        ui->lbPoints->setVisible(false);    //
    else                                    // clignotement des 2 points de l'heure
        ui->lbPoints->setVisible(true);     //
    ui->lbHeure->setText(Heure.toString("hh"));     // affiche l'heure
    ui->lbMinute->setText(Heure.toString("mm"));    // affiche les minutes
}

// Affiche la temperature
void CIhm::onTimerTemperature()
{
      float temp = pa->getTemperature();
      if ( (temp<-99) || (temp>99))
           this->ui->lbTemp->setText("Def !");
      else
           this->ui->lbTemp->setText(QString::number(static_cast<double>(temp)));
}

// Gestion présence
void CIhm::onTimerCapteur()  // toutes les sec si presence
{
    mPresence = pa->getPresence();
    if(!mPresence) { // si non présence
       if (!timerPresence->isActive()) {
            ui->lTvState->setText("START");
            qDebug() << "(Ré)activation timer Présence détectée.";
            int duree = pa->getIdleTime();
            timerPresence->start(duree); // (duree) activation chrono interval détection presence (mn)
       } // if isactive
    } else { // si présence
       if (timerPresence->isActive()) {
           timerPresence->stop();
       } // isactive
       if (!pa->getEtatTele())
           pa->switchOnTv();
    } // else presence
} // onTimerCapteur

void CIhm::onTimerPresence()   // seulement si mode présence et personne devant depuis x mn
{
        pa->switchOffTv();  // tente d'éteindre l'écran
}

// Veille / réveille
void CIhm::onTimerOpenHour()  // seulement si mode heure depart et fin
{
    bool repStart = pa->isItTheMomentToStart();
    if(repStart) {
        pa->switchOnTv();
//        qDebug() << "Mode heure dep/arr : Mise en route TV.";
    }

    bool repStop = pa->isItTheMomentToStop();
    if(repStop) {
//        qDebug() << "Mode heure dep/arr : Arrêt de la TV.";
        pa->switchOffTv();
    }
}

// Ecrire les données capteurs dans la Bdd
void CIhm::onTimerBdd()
{
    mTemp = pa->captTemp->getTemp();
    QString realtemp = QString::number(static_cast<double>(mTemp),'f',1);
    mPresence = pa->captPres->getPresence();
    QByteArray pourcentage = pa->getSDPlace();
    bdd->setCapteurs("UPDATE pas SET temp='"+realtemp+"', Pourcentage_SD ='"+pourcentage+"', presence='"+(mPresence?"O":"N")+"' WHERE mac='"+mac+"';");
}

void CIhm::getSlide()
{
    qDebug() << "CIhmAppAies2020::getSlide() : On recommence.";
    bdd->setSliderUpdate(pa->getDateTime()); // met à jour le champ state à actif ou arch pour tous les PA
    mTabSlides = bdd->getActiveSlides(mac); // uniquement celles pour le PA
    mUrgencyState = pa->getUrgency();
    affSlide();
}

void CIhm::affSlide()
{
    QString docs = conf->getHtDocs();
    pa->creationCache();
    mUrgencyState = pa->getUrgency();

    if(mUrgencyState) {
       QUrl url("http://" + mServeur + docs + "/rpi/slide/oups.php");
       ui->webViewStarter->load(url);
       timerIdleSlide->start(10000); //
    } else {
        if(mTabSlides.at(0).at(0) != "none") {
            if(mCompteurSlide < mTabSlides.size()) {
                if (mTabSlides.at(mCompteurSlide).at(3) == "yes") {
                    mPriorSlide.setUrl("http://" + mServeur + docs + mTabSlides.at(mCompteurSlide).at(1));// by PhA 2019-01-31
                    mPriorSwap=true;
                    mPriorTime=mTabSlides.at(mCompteurSlide).at(2).toInt();
                    mCompteurSlide++;
                } // if
                if (mPriorSwap) {
                    qDebug() << mPriorSlide.url();
                    ui->webViewStarter->load(mPriorSlide);
                    timerSlide->start(mPriorTime);
                    mPriorSwap = false;
                } else {
                    QUrl slideUrl("http://" + mServeur + docs + mTabSlides.at(mCompteurSlide).at(1)); // by PhA 2019-01-31
                    qDebug() << slideUrl.url();
                    ui->webViewStarter->load(slideUrl);
                    timerSlide->start(mTabSlides.at(mCompteurSlide).at(2).toInt());
                    mPriorSwap = true;
                    mCompteurSlide++;
                } // else mPriorSwap
            } else {
                mCompteurSlide = 0;
                getSlide();
            } // else mCompteurSlide
        }
        else {
            QUrl url("http://" + mServeur + docs + "/rpi/slide/oups.php");  // by PhA 2019-01-31
            ui->webViewStarter->load(url);
            timerIdleSlide->start(10000);
        } // else
    }
} // affSlide

void CIhm::onTimerFlash()
{
    ui->lInfoFlash->setText("<html><style> h2 {background-color:red;} h2{color:white;}</style><h2>"+bdd->getActiveFlash()+"</h2></html>");
}

void CIhm::onTimerUpdate()  // vérification de la maj du logiciel
{
    QDateTime dateActu = QDateTime::currentDateTime();
    QString strDateDiff = pa->getDiffUpdateDate();

    if(strDateDiff != "none") {
        QDateTime dateDiff = QDateTime::fromString(strDateDiff, "yyyyMMddHHmm");
        if(dateDiff <= dateActu) {
            pa->switchDiffToPerma();
        }
    }
    QString versionPerma = pa->getPermaUpdateVer();

    if( (versionPerma.toFloat()>mVer.toFloat()) && !mUpdate ) {
        qDebug() << "[fihm::onTimerUpdate]: Lancement de aies_update";
        mUpdate=true;
        //lancement aies_update
        disconnect(this, SLOT(onTimerUpdate()));  // on reçoit plus les signaux MAJ
        system("/home/pi/aies/update/aies_update");
        // prévoir éventuellement de terminer l'application
    }
}

void CIhm::onTimerSlide()
{
    timerSlide->stop();
    affSlide();
}

void CIhm::onTimerIdleSlide()
{
    timerIdleSlide->stop();
    getSlide();
}

void CIhm::onSigPresence(int st)
{
    ui->lLogoPres->setVisible(st);
    if (st)
        pa->switchOnLed();
    else
        pa->switchOffLed();
}

void CIhm::onSigPa(QString mess)
{
    ui->lTvState->setText(mess);
}
