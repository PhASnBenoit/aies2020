#ifndef CTHREADCAPTEURS_H
#define CTHREADCAPTEURS_H

#include <QObject>
#include <QWidget>
#include <QTimer>

#include "csharedmemory.h"
#include "ccapteurpres.h"
#include "ccapteurtemp.h"
#include "ccapteurgazfumeemq_2.h"
#include "cadafruit_sgp30.h"
#include "cutv.h"

class CThreadCapteurs : public QObject
{
    Q_OBJECT
public:
    explicit CThreadCapteurs(QObject *parent = nullptr);
    ~CThreadCapteurs();

private:
    CSharedMemory *shm;
    QTimer *tmr;
    CCapteurPres *captPres;
    CCapteurTemp *captTemp;
    CCapteurGazFumeeMQ_2 *captFumee;
    CAdafruit_SGP30 *captQa;
    CUTv *utv;

signals:

public slots:
    void on_timeout();
    void go();
};

#endif // CTHREADCAPTEURS_H
