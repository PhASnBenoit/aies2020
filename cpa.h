#ifndef CPA_H
#define CPA_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QTimer>
#include <QDateTime>

#include "cbdd.h"
#include "cled.h"
#include "ccapteurpres.h"
#include "ccapteurtemp.h"
#include "./biblis/cadafruit_sgp30.h"
#include "cir.h"
#include "cecran.h"

// Consignes des ordres ECRAN
#define ALLUMER 1
#define ETEINDRE 0

// etat écran
#define TV_ON 1
#define TV_OFF 0

// etat d'un ordre à l'écran. améliorer en mettant enum
#define ORDRE_ATTENTE 0
#define ORDRE_EN_COURS 1 // un ordre est émis, attente retour alim via USB
#define ORDRE_PASSED 2  // l'ordre n'a pas été compris par l'écran

class CPa: public QObject
{
       Q_OBJECT
public:
    explicit CPa(QObject *parent= nullptr, CBdd *bdd=nullptr);
    ~CPa();
    int getUrgency();
    QString getZone();
    QString getNom();
    QString getModeFonc();
    QString getMac();
    QString getCtrlTv();
    QString getHstart();
    QString getHstop();
    QString getPermaUpdateVer();
    QString getPermaUpdateDate();
    QString getDiffUpdateVer();
    QString getDiffUpdateDate();
    QDateTime getDateTime();
    int getIdleTime();
    bool isItTheMomentToStart();
    bool isItTheMomentToStop();
    bool getEtatTele();
    void setEtatTele(bool state);
    void setIdleTime(int idleTime);
    void switchDiffToPerma();
    bool switchOnTv();
    bool switchOffTv();
    bool switchOnLed();
    bool switchOffLed();
    bool getPresence();
    float getTemperature();
    CCapteurPres *captPres;
    CCapteurTemp *captTemp;
    CAdafruit_SGP30 *captQa;
    QByteArray getSDPlace();
    void creationCache();
    void setEtatOrdre(int etatOrdre);

    int getEtatOrdre() const;

private:
    CEcran *ecran;
    CIr *emIr;
    CLed *led;
    QString mZone;
    QString mNom;
    QString mModeFonc;
    QString mMac;
    QString mHstart;
    QString mHstop;
    QString mCtrlTv;
    QString mPermaUpdateVer;
    QString mPermaUpdateDate;
    QString mDiffUpdateVer;
    QString mDiffUpdateDate;
    QString mDateIs;
    int mIdleTime;
    bool mEtatTele;
    QString getSysMacAddress();
    CBdd *mBdd;
    QTimer *mTimerU;
    bool mConsigne;
    int m_etatOrdre;

signals:
    void sigPresence(int st);
    void sigPa(QString mess);

private slots:
    void onSigPresence(int st);
    void onTimerU();
};

#endif // CPA_H
