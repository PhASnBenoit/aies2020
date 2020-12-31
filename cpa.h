#ifndef CPA_H
#define CPA_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QTimer>
#include <QDateTime>
#include <QFile>
#include <QThread>

#include "cbdd.h"
#include "cled.h"
#include "cir.h"
#include "cecran.h"
#include "cthreadcapteurs.h"
#include "csharedmemory.h"

// Consignes des ordres ECRAN
#define ALLUMER 1
#define ETEINDRE 0

// etat écran
#define ALLUMED 1
#define ETEINT 0

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
    bool getEtatReelTv();
    void setEtatReelTv(bool state);
    void setIdleTime(int idleTime);
    void switchDiffToPerma();
    bool switchOnTv();
    bool switchOffTv();
    bool switchOnLed();
    bool switchOffLed();
    void calculateSDPlace();
    QByteArray getSDPlace();
    void creationCache();
    bool getConsigne() const;
    void setConsigne(bool consigne);
    CEcran *ecran;

private:
    CIr *emIr;
    CLed *led;
    CSharedMemory *shm;
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
    bool mEtatReelTele;
    CBdd *mBdd;
    QTimer *mTimerU; // toutes les 30s
    QTimer *mTimer;  // toutes les s
    bool mConsigne;
    bool mOrdre;
    bool mOrdrePassed;
    QByteArray mSDPlace;
    QByteArray _pc; // présence des capteurs
    QThread th;
    CThreadCapteurs *thCapt;
    QString getSysMacAddress();

signals:
    void sigPresence(int st);
    void sigPaConsigne(QString mess);
    void sigPaOrdre(QString mess);
    void sigPaEtatReelTv(QString mess);
    void sigQAir(QString mess);
    void sigGo();

private slots:
    void onTimerU();
    void onTimerBdd();
};

#endif // CPA_H
