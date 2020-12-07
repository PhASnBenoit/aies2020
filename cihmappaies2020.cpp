#include "cihmappaies2020.h"
#include "ui_cihmappaies2020.h"

CIhmAppAies2020::CIhmAppAies2020(QWidget *parent) :
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
    connect(pa, SIGNAL(sigPresence(int)), this, SLOT(onSigPresence(int))); // aff * presence
    connect(pa, SIGNAL(sigPa(QString)), this, SLOT(onSigPa(QString)));  // aff on/off tv
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
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerHeure()));
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerTemperature()));
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerBdd()));
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimerFlash()));

    // Timer Slide
    timerSlide = new QTimer();
    timerIdleSlide = new QTimer();
    connect(timerSlide, SIGNAL(timeout()), this, SLOT(onTimerSlide()));
    connect(timerIdleSlide, SIGNAL(timeout()), this, SLOT(onTimerIdleSlide()));

    // Timer d'intervale d'absence
    QString modeFonc = pa->getModeFonc();
    if( modeFonc == "presence")
    {
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimerCapteur())); // toutes les sec
        timerPresence = new QTimer(this);
        connect(timerPresence, SIGNAL(timeout()), this, SLOT(onTimerPresence()));
    }
    else  // heure depart et de fin
    {
        connect(timer, SIGNAL(timeout()), this, SLOT(onTimerOpenHour()));
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

CIhmAppAies2020::~CIhmAppAies2020()
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
void CIhmAppAies2020::onTimerHeure()
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
void CIhmAppAies2020::onTimerTemperature()
{
      float temp = pa->getTemperature();
      if ( (temp<-99) || (temp>99))
           this->ui->lbTemp->setText("Def !");
      else
           this->ui->lbTemp->setText(QString::number(static_cast<double>(temp)));
}

// Gestion présence
void CIhmAppAies2020::onTimerCapteur()  // toutes les sec
{
    mPresence = pa->getPresence();
    if(!mPresence)  // si non présence
    {
       if (!timerPresence->isActive()) {
            ui->lTvState->setText("START");
            qDebug() << "(Ré)activation timer Présence détectée.";
            int duree = pa->getIdleTime();
            timerPresence->start(duree); // (duree) activation chrono interval détection presence (mn)
       } // if isactive
    }
    else  // si présence
    {
       if (timerPresence->isActive()) {
           timerPresence->stop();
       } // isactive
       if (!pa->getEtatTele())
           pa->switchOnTv();
    } // else presence
} // onTimerCapteur

void CIhmAppAies2020::onTimerPresence()   // seulement si mode présence et personne devant depuis x mn
{
    if (pa->getEtatTele()) { // si télé allumée pour envoyer l'ordre qu'une fois
        pa->switchOffTv();
        qDebug() << "Arrêt TV suite à non présence.";
    } // if getetattele
}

// Veille / réveille
void CIhmAppAies2020::onTimerOpenHour()  // seulement si mode heure depart et fin
{
    bool repStart = pa->isItTheMomentToStart();
    if(repStart) {
        pa->switchOnTv();
        qDebug() << "Mode heure dep/arr : Mise en route TV.";
    }

    bool repStop = pa->isItTheMomentToStop();
    if(repStop) {
        qDebug() << "Mode heure dep/arr : Arrêt de la TV.";
        pa->switchOffTv();
    }
}

// Ecrire les données capteurs dans la Bdd
void CIhmAppAies2020::onTimerBdd()
{
    mTemp = pa->captTemp->getTemp();
    QString realtemp = QString::number(static_cast<double>(mTemp),'f',1);
    mPresence = pa->captPres->getPresence();
    QByteArray pourcentage = pa->getSDPlace();
    bdd->setCapteurs("UPDATE pas SET temp='"+realtemp+"', Pourcentage_SD ='"+pourcentage+"', presence='"+(mPresence?"O":"N")+"' WHERE mac='"+mac+"';");
}

void CIhmAppAies2020::getSlide()
{
    bdd->setSliderUpdate(pa->getDateTime()); // met à jour le champ state à actif ou arch pour tous les PA
    mTabSlides = bdd->getActiveSlides(mac); // uniquement celles pour le PA
    mUrgencyState = pa->getUrgency();
    affSlide();
}

void CIhmAppAies2020::affSlide()
{
    QString docs = conf->getHtDocs();
    pa->creationCache();
    qDebug() << docs << endl;
    mUrgencyState = pa->getUrgency();
    qDebug() << "urgence"<<  mUrgencyState << endl;

    if(mUrgencyState != 0) {
       QUrl url("http://" + mServeur + docs + "/rpi/slide/oups.php");
       ui->webViewStarter->load(url);
       timerIdleSlide->start(10000);
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
                    qDebug() << mPriorSlide.url() << endl;
                    ui->webViewStarter->load(mPriorSlide);
                    timerSlide->start(mPriorTime);
                    mPriorSwap = false;
                } else {
                    QUrl slideUrl("http://" + mServeur + docs + mTabSlides.at(mCompteurSlide).at(1)); // by PhA 2019-01-31
                    qDebug() << slideUrl.url() << endl;
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

void CIhmAppAies2020::onTimerFlash()
{
    ui->lInfoFlash->setText("<html><style> h2 {background-color:red;} h2{color:white;}</style><h2>"+bdd->getActiveFlash()+"</h2></html>");
}

void CIhmAppAies2020::onTimerUpdate()  // vérification de la maj du logiciel
{
    QDateTime dateActu = QDateTime::currentDateTime();
    QString strDateDiff = pa->getDiffUpdateDate();

    if(strDateDiff != "none")
    {
        QDateTime dateDiff = QDateTime::fromString(strDateDiff, "yyyyMMddHHmm");
        if(dateDiff <= dateActu)
        {
            pa->switchDiffToPerma();
        }
    }
    QString versionPerma = pa->getPermaUpdateVer();

    if( (versionPerma.toFloat()>mVer.toFloat()) && !mUpdate )
    {
        qDebug() << "[fihm::onTimerUpdate]: Lancement de aies_update";
        mUpdate=true;
        //lancement aies_update
        disconnect(this, SLOT(onTimerUpdate()));  // on reçoit plus les signaux MAJ
        system("/home/pi/aies/update/aies_update");
        // prévoir éventuellement de terminer l'application

    }
}

void CIhmAppAies2020::onTimerSlide()
{
    timerSlide->stop();
    affSlide();
}

void CIhmAppAies2020::onTimerIdleSlide()
{
    timerIdleSlide->stop();
    getSlide();
}

void CIhmAppAies2020::onSigPresence(int st)
{
    ui->lLogoPres->setVisible(st);
    if (st)
        pa->switchOnLed();
    else
        pa->switchOffLed();
}

void CIhmAppAies2020::onSigPa(QString mess)
{
    ui->lTvState->setText(mess);
}
