#include "cihm.h"
#include "ui_cihm.h"

CIhm::CIhm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CIhm)
{
    // initialisation IHM
    ui->setupUi(this);
    ui->lLogoIr->setVisible(false); // futur ???
    ui->lLogoPres->setVisible(true); // par défaut
    QPixmap logoPath("/opt/aies/logoLAB.png");
    QPixmap logoPath1("/opt/aies/presence1.png");
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));  // Rend invisible le curseur

    // Initialisation des objets
    mUpdate=false;  // maj appli
    conf = new CConfig();
    bdd = new CBdd();
    shm = new CSharedMemory(this);
    pa = new CPa(this, bdd); // lance aussi le thread capteurs
    connect(pa, &CPa::sigPresence, this, &CIhm::onSigPresence); // aff * presence
    connect(pa, &CPa::sigPaConsigne, this, &CIhm::onSigPaConsigne);  // aff on/off tv
    connect(pa, &CPa::sigPaOrdre, this, &CIhm::onSigPaOrdre);  // aff on/off tv
    connect(pa, &CPa::sigPaEtatReelTv, this, &CIhm::onSigPaEtatReelTv);  // aff on/off tv
    connect(pa, &CPa::sigQAir, this, &CIhm::onSigQAir);  // aff serial numb Q air

    mPresence = shm->getCapteurPresence();
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

    // timer surveillance capacité de la SD
    timerCapaSd = new QTimer();

    // Timer Slide
    timerSlide = new QTimer(); // changement de  slide
    connect(timerSlide, &QTimer::timeout, this, &CIhm::onTimerSlide);

    timerOupsSlide = new QTimer();  // pour les diapos oups !
    connect(timerOupsSlide, &QTimer::timeout, this, &CIhm::onTimerOupsSlide);

    // Timer général
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerFlash);  // affichage de l'info flash
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerAff); // affichages
    connect(timer, &QTimer::timeout, this, &CIhm::onTimerUpdate); // mise à jour du logiciel

    pa->switchOnTv();  // ALLUME LA TV
    QString modeFonc = pa->getModeFonc();
    ui->lModeFonc->setText(modeFonc);
    ui->lType->setText(pa->getCtrlTv());
    if( modeFonc == "presence")
    {
        connect(timer, &QTimer::timeout, this, &CIhm::onTimerCapteurPresence); // toutes les sec
        timerNonPresence = new QTimer(this);     // Timer d'intervale d'absence
        int duree = pa->getIdleTime();
        timerNonPresence->setInterval(duree);
        connect(timerNonPresence, &QTimer::timeout, this, &CIhm::onTimerNonPresence);
    } else { // heure depart et de fin
        connect(timer, &QTimer::timeout, this, &CIhm::onTimerOpenHour);
    }

    timer->start(1000); // Timer général 1s
    timerCapaSd->start(300000);  // 5mn

    //Initialisation Première Slide
    ui->webViewStarter->page()->settings()->setObjectCacheCapacities(0,0,0); // suppression cache
    mCompteurSlide = 0;
    getSlide();
} // fihm

CIhm::~CIhm()
{
    delete timer;
    delete timerCapaSd;
    delete timerNonPresence;
    delete timerSlide;
    delete timerOupsSlide;
    delete maj;
    delete flash;
    delete pa;
    delete shm;
    delete bdd;
    delete conf;
    delete ui;
}

// Gestion présence
void CIhm::onTimerCapteurPresence()  //  si mode presence
{
    mPresence = shm->getCapteurPresence();
//    qDebug() << "[CIhm::onTimerCapteurPresence] mPresence="<<mPresence;
    if(!mPresence) { // si non présence
        if (shm->getCapteurUTv()) { // si TV allumée
           if (!timerNonPresence->isActive()) {
                timerNonPresence->start(); // relance durée de non présence devant l'écran
           } // if isactive
        } // if getetatreel
    } else { // si présence
//       qDebug() << "[CIhm::onTimerCapteurPresence] Détection présence.";
       if (timerNonPresence->isActive()) {
           timerNonPresence->stop();
       } // isactive
       pa->setConsigne(ALLUMER);
       if (!shm->getCapteurUTv()) {
           pa->switchOnTv();
       } // if getEtat
    } // else presence
}

void CIhm::onTimerAff()
{
    // heure
    QTime Heure;
    Heure = QTime::currentTime();
    if (ui->lbPoints->isVisible())
        ui->lbPoints->setVisible(false);    //
    else                                    // clignotement des 2 points de l'heure
        ui->lbPoints->setVisible(true);     //
    ui->lbHeure->setText(Heure.toString("hh"));     // affiche l'heure
    ui->lbMinute->setText(Heure.toString("mm"));
    // U TV
    ui->lTvState->setText((shm->getCapteurUTv()?"TV-ON":"TV-OFF"));
    // présence
    ui->lLogoPres->setVisible(shm->getCapteurPresence());
    // Température
    ui->lbTemp->setText(QString::number(static_cast<double>(shm->getMesTemp())));
    // Fumée
    ui->lFumee->setVisible(shm->getCapteurGazFumee());
    // qualité air
    affQuality("tous");
//    qDebug() << "[CIhm::onTimerAff] Affichages ";
}

void CIhm::affQuality(QString choix)
{
    static int cpt=0;
    cpt++;
    if(choix=="tous") {
        switch(cpt) {
        case 15:case 16:case 17:
            ui->lQa->setText("Comp TVOC:"+QString::number(shm->getMesBaselineTVOC())+
                             " eCO2:"+QString::number(shm->getMesBaselineECO2()));
            break;
        case 18:case 19:case 20:
            ui->lQa->setText("Raw H2:"+QString::number(shm->getMesRawH2())+
                             " Eth:"+QString::number(shm->getMesRawEthanol()));
            break;
        default:
            ui->lQa->setText("QAir TVOC:"+QString::number(shm->getMesTVOC())+
                             " eCO2:"+QString::number(shm->getMesECO2()));
            break;
        } // sw
    } // if tous
    if(choix=="sig")
        ui->lQa->setText("QAir TVOC:"+QString::number(shm->getMesTVOC())+
                         " eCO2:"+QString::number(shm->getMesECO2()));
    if(choix=="raw")
        ui->lQa->setText("Raw H2:"+QString::number(shm->getMesRawH2())+
                         " Eth:"+QString::number(shm->getMesRawEthanol()));
    if(choix=="base")
        ui->lQa->setText("BL TVOC:"+QString::number(shm->getMesBaselineTVOC())+
                         " eCO2:"+QString::number(shm->getMesBaselineECO2()));
    if (cpt==20) cpt = 0;
}

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

void CIhm::getSlide()
{
    QString docs = conf->getHtDocs();

    qDebug() << endl << "CIhmAppAies2020::getSlide() : On recommence.";
    pa->calculateSDPlace();
    pa->creationCache();
    bdd->setSliderUpdate(pa->getDateTime()); // met à jour le champ state à actif ou arch pour tous les PA
    mPrior = false;
    mTabSlides = bdd->getActiveSlides(mac); // uniquement celles pour le PA
    // recherche de la diapo prioritaire
    for (int i=0 ; i<mTabSlides.size() ; i++) {
        if (mTabSlides.at(i).at(3) == "yes") { // si diapo prioritaire
            mPriorSlide.setUrl("http://" + mServeur + docs + mTabSlides.at(i).at(1));// by PhA 2019-01-31
            mPriorSwap=true;
            mPrior=true;
            mPriorTime=mTabSlides.at(i).at(2).toInt();
            break;
        } // if
    } // for i
qDebug() << "[CIhm::getSlide] Nbe de diapo : " << mTabSlides.size();
    //mUrgencyState = pa->getUrgency();
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
            if (timerOupsSlide->isActive())
                timerOupsSlide->stop();
            if(mCompteurSlide < mTabSlides.size()) {
                if (mTabSlides.at(mCompteurSlide).at(3) == "yes")
                    mCompteurSlide++; // pour éviter l'affichage supplémentaire de la diapo prioritaire
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
        } else { // Pas URGENCE mais pas de diapo à afficher.
            QUrl url("http://" + mServeur + docs + "/rpi/slide/oups.php");  // by PhA 2019-01-31
            ui->webViewStarter->load(url);
            timerOupsSlide->start(10000);
        } // else rien a afifcher
    } // else urgence
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

void CIhm::onSigPaOrdre(QString mess)
{
    ui->lOrdre->setText(mess);
}

void CIhm::onSigPaEtatReelTv(QString mess)
{
    ui->lTvState->setText(mess);
}

void CIhm::onSigQAir(QString mess)
{
    ui->lQa->setText(mess);
}
