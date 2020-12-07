#ifndef CIHMAPPAIES2020_H
#define CIHMAPPAIES2020_H

#include <QMainWindow>
#include <QDebug>
#include <QUrl>
#include <QDate>
#include <QTime>
#include <QTimer>

#include "cbdd.h"
#include "cpa.h"
#include "cconfig.h"

namespace Ui {
class CIhmAppAies2020;
}

class CIhmAppAies2020 : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhmAppAies2020(QWidget *parent = 0);
    ~CIhmAppAies2020();

private:
    Ui::CIhmAppAies2020 *ui;
    CBdd *bdd;
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
    QList <QList<QString> > mTabSlides;
    QTimer *timerPresence;
    QTimer *timerSlide;
    QTimer *timerIdleSlide;
    QTimer *timer;
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
    void onTimerTemperature();
    void onTimerCapteur();
    void onTimerPresence();
    void onTimerOpenHour();
    void onTimerBdd();
    void onTimerUpdate();
    void onTimerSlide();
    void onTimerFlash();
    void onTimerIdleSlide();
    void onSigPresence(int st);
    void onSigPa(QString mess);
};

#endif // CIHMAPPAIES2020_H
