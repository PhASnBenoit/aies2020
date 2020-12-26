#ifndef CIHM_H
#define CIHM_H

#include <QMainWindow>
#include <QDebug>
#include <QUrl>
#include <QDate>
#include <QTime>
#include <QTimer>

#include "cbdd.h"
#include "cpa.h"
#include "cconfig.h"
#include "csharedmemory.h"

namespace Ui {
class CIhm;
}

class CIhm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhm(QWidget *parent = nullptr);
    ~CIhm();

private:
    Ui::CIhm *ui;
    CBdd *bdd;
    CSharedMemory *shm;
    CPa *pa;
    CConfig *conf;
    QString mServeur;
    QString mac;
    QString Zone_Id;
    QString currentName;
    QString Url;
    QString mVer;
    int mUrgencyState;
    QUrl mPriorSlide; // URL de la dernière slide prioritaire
    bool mPriorSwap;  // bascule avec slide prioritaire
    int mPriorTime;  // durée slide prioritaire
    bool mUpdate;
    bool mPrior;
    QList <QList<QString> > mTabSlides;
    QTimer *timerNonPresence;
    QTimer *timerSlide;
    QTimer *timerOupsSlide;
    QTimer *timer;
    QTimer *timerAffHeureTempMaj;
    QTimer *timerCapaSd;
    QTimer *maj;
    QTimer *flash;
    int idEnCours;
    int idFEnCours;
    int mCompteurSlide;
    float mTemp;
    bool mPresence;
    void getSlide();
    void affSlide();

private slots:
    void onTimerHeure();
    void onTimerCapteurPresence();
    void onTimerAff();
    void onTimerNonPresence();
    void onTimerOpenHour();
    void onTimerBdd();
    void onTimerUpdate();
    void onTimerSlide();
    void onTimerFlash();
    void onTimerOupsSlide();
    void onSigPresence(int st);
    void onSigPaConsigne(QString mess);
    void onSigQAir(QString mess);
};

#endif // CIHM_H
