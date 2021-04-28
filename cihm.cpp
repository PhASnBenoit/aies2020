#include "cihm.h"
#include "ui_cihm.h"

CIhm::CIhm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CIhm)
{
    // initialisation IHM
    ui->setupUi(this);
    ui->lLogoIr->setVisible(false);
    ui->lLogoPres->setVisible(true); // par défaut
    QPixmap logoPath("/opt/aies/logoLAB.png");
    QPixmap logoPath1("/opt/aies/presence1.png");
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));  // Rend invisible le curseur

    // Initialisation des objets
    mUpdate=false;
    bdd = new CBdd();
    pa = new CPa(this, bdd);
    connect(pa, &CPa::sigPresence, this, &CIhm::onSigPresence); // aff * presence
    connect(pa, &CPa::sigPaConsigne, this, &CIhm::onSigPaConsigne);  // aff on/off tv
    conf = new CConfig();
    mPresence = false;
    mPriorSwap=false;
    mPrior=false;  // Pas de dispo prioritaire
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
    ui->lConsigne->setText((pa->getConsigne()?"C-ON":"C-OFF"));
    ui->lTvState->setText((pa->getEtatReelTv()?"TV-ON":"TV-OFF"));

    // timer surveillance capacité de la SD
    timerCapaSd = new QTimer();

    // timer 30s pour horloge et température
    timerAffHeureTempMaj = new QTimer();
    connect(timerAffHeureTempMaj, &QTimer::timeout, this, &CIhm::onTimerHeure);  // affichage de l'heure
    connect(timerAffHeureTempMaj, &QTimer::timeout, this, &CIhm::onTimerTemperature); // affichage température
    connect(timerAffHeureTempMaj, &QTimer::timeout, this, &CIhm::onTimerUpdate); // mise à jour du logiciel
    //connect(timerAffHeureTempMaj, &QTimer::timeout, this, &CIhm::onTimerBdd); // mise à jour du logiciel

    // Timer Slide
    timerSlide = new QTimer(); // changement de  slide
    connect(timerSlide, &QTimer::timeout, this, &CIhm::onTimerSlide);

    timerOupsSlide = new QTimer();  // pour les diapos oups !
    connect(timerOupsSlide, &QTimer::timeout, this, &CIhm::onTimerOupsSlide);

    // Timer général
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerFlash);  // affichage de l'info flash
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerBdd); // sauve temp, presence, capcite SD dans bdd

    // Timer d'intervale d'absence
    QString modeFonc = pa->getModeFonc();
    if( modeFonc == "presence")
    {
        connect(timer, &QTimer::timeout, this, &CIhm::onTimerCapteurPresence); // toutes les sec
        timerNonPresence = new QTimer(this);
        int duree = pa->getIdleTime();
        timerNonPresence->setInterval(duree);
        connect(timerNonPresence, &QTimer::timeout, this, &CIhm::onTimerNonPresence);
    } else { // heure depart et de fin
        connect(timer, &QTimer::timeout, this, &CIhm::onTimerOpenHour);
    }

    onTimerHeure();  // premier affichage de l'heure
    onTimerTemperature();

    timer->start(1000); // Timer général 1s
    timerAffHeureTempMaj->start(30000);  //30s
    timerCapaSd->start(300000);  // 5mn

    // affichage mode fonc et type commande écran
    ui->lModeFonc->setText(modeFonc);
    ui->lType->setText(pa->getCtrlTv());
    pa->switchOnTv();  // ALLUME LA TV
qDebug() << "[CIhm::CIhm] allume la TV";
    //Initialisation Première Slide
    ui->webViewStarter->page()->settings()->setObjectCacheCapacities(0,0,0); // suppression cache
    mCompteurSlide = 0;
    getSlide();
} // fihm

CIhm::~CIhm()
{
    delete timer;
    delete timerAffHeureTempMaj;
    delete timerCapaSd;
    delete timerNonPresence;
    delete timerSlide;
    delete timerOupsSlide;
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
void CIhm::onTimerCapteurPresence()  //  si mode presence
{
    mPresence = pa->getPresence();
    qDebug() << "[CIhm::onTimerCapteur] mPresence="<<mPresence;
    if(!mPresence) { // si non présence
        if (pa->getEtatReelTv()) {
           if (!timerNonPresence->isActive()) {
                timerNonPresence->start(); // relance durée de non présence devant l'écran
           } // if isactive
        } // if getetatreel
    } else { // si présence
       qDebug() << "[CIhm::onTimerCapteur] Détection présence.";
       if (timerNonPresence->isActive()) {
           timerNonPresence->stop();
       } // isactive
       if (!pa->getEtatReelTv()) {
           pa->switchOnTv();
       } // if getEtat
    } // else presence
} // onTimerCapteur

void CIhm::onTimerNonPresence()   // seulement si mode présence et personne devant depuis x mn
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
void CIhm::onTimerBdd()  // toutes les secs
{
    bool u=pa->ecran->getU();
    pa->setEtatReelTv(u);
    ui->lTvState->setText((u?"TV-ON":"TV-OFF"));

//    ui->lConsigne->setText((pa->getConsigne()?"TV-ON":"TV-OFF"));

    mTemp = pa->captTemp->getTemp();
    QString realtemp = QString::number(static_cast<double>(mTemp),'f',1);
    mPresence = pa->captPres->getPresence();
    QByteArray pourcentage = pa->getSDPlace();
    bdd->setCapteurs("UPDATE pas SET temp='"+realtemp+"', Pourcentage_SD ='"+pourcentage+"', presence='"+(pa->getEtatReelTv()?"O":"N")+"' WHERE mac='"+mac+"';");
}

void CIhm::getSlide()
{
    qDebug() << endl << "CIhmAppAies2020::getSlide() : On recommence.";
    pa->calculateSDPlace();
    pa->creationCache();
    bdd->setSliderUpdate(pa->getDateTime()); // met à jour le champ state à actif ou arch pour tous les PA
    mTabSlides = bdd->getActiveSlides(mac); // uniquement celles pour le PA
    qDebug() << "CIhm::getSlide Nbe de diapo : " << mTabSlides.size();
    mUrgencyState = pa->getUrgency();
    affSlide();
}

void CIhm::affSlide()
{
    QString docs = conf->getHtDocs();
    mUrgencyState = pa->getUrgency();

    if(mUrgencyState) {
        qDebug() << "[CIhm::affSlide] FIN URGENTE DE L'AFFICHAGE";
        QUrl url("http://" + mServeur + docs + "/rpi/slide/oups.php");
       ui->webViewStarter->load(url);
       timerOupsSlide->start(10000); //
    } else {
        if(mTabSlides.at(0).at(0) != "none") {
            if (timerOupsSlide->isActive()) timerOupsSlide->stop();
            if(mCompteurSlide < mTabSlides.size()) {
                if (mTabSlides.at(mCompteurSlide).at(3) == "yes") { // si diapo prioritaire
                    mPriorSlide.setUrl("http://" + mServeur + docs + mTabSlides.at(mCompteurSlide).at(1));// by PhA 2019-01-31
                    mPriorSwap=true;
                    mPrior=true;
                    mPriorTime=mTabSlides.at(mCompteurSlide).at(2).toInt();
                    mCompteurSlide++; // j'ai des doutes ! PhA 20201217
                } // if
                if (mPriorSwap && mPrior) { // aff de la prioritaire si existe
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
        else { // Pas URGENCE mais pas de diapo à afficher.
            QUrl url("http://" + mServeur + docs + "/rpi/slide/oups.php");  // by PhA 2019-01-31
            ui->webViewStarter->load(url);
            timerOupsSlide->start(10000);
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
        system("/opt/aies/aies_update");
        // prévoir éventuellement de terminer l'application
    }
}

void CIhm::onTimerSlide()
{
    timerSlide->stop();
    affSlide();
}

void CIhm::onTimerOupsSlide()
{
    timerOupsSlide->stop();
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

void CIhm::onSigPaConsigne(QString mess)
{
    ui->lConsigne->setText(mess);
}
